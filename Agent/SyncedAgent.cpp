/*
 *  Copyright 2002-2010, Mersad Team, Allameh Helli High School (NODET).
 *
 *  This program is free software, you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Library General Public License for more details.
 *
 *  This file is created by: Ahmad Boorghany
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <iostream>
#include <unistd.h>

#include <Share.h>
#include <Logger.h>
#include <SyncedAgent.h>
#include <OnlineConnection.h>

using namespace std;

SyncedAgent::SyncedAgent(int argc, char **argv): BasicAgent(argc, argv)
{
	seeReceiveMS = 0;
	seeReceivedFlag = false;
	quickDecideFlag = false;
	bodyCycleCommandSentFlag = false;
	headCycleCommandSentFlag = false;
	bodyCycleCommandLostFlag = false;
	headCycleCommandLostFlag = false;
}

SyncedAgent::~SyncedAgent()
{
}

void SyncedAgent::senseBody(const SExpression &exp)
{
	BasicAgent::senseBody(exp);

	if (isHeadCycleCommandLost() && isHeadCycleCommandSent())
		recoverSeeReceiveMSAfterChangeView(changeViewCycleCommand);

	delete lastBodyCycleCommand;
	lastBodyCycleCommand = bodyCycleCommand;
	bodyCycleCommand = NULL;
	if (!lastBodyCycleCommand) // Just happen when we hadn't any time
							   // in last cycle to start deciding at least.
		lastBodyCycleCommand = new EmptyCommand();
	setBodyCycleCommandSent(false);

	delete sayCycleCommand;
	sayCycleCommand = NULL;

	delete pointToCycleCommand;
	pointToCycleCommand = NULL;

	delete changeViewCycleCommand;
	changeViewCycleCommand = NULL;

	delete attentionCycleCommand;
	attentionCycleCommand = NULL;

	delete headCycleCommand;
	headCycleCommand = NULL;
	setHeadCycleCommandSent(false);

	setSenseBodyReceived(true);
	setQuickDecide(false);
	setSeeReceived(false);
	setSeeReceiveMSAfterSenseBody();
}

void SyncedAgent::see(const SExpression &exp)
{
	BasicAgent::see(exp);

	setSeeReceived(true);
	setSeeReceiveMSAfterSee();
}

void SyncedAgent::hear(const SExpression &exp)
{
	BasicAgent::hear(exp);
}

void SyncedAgent::fullState(const SExpression &exp)
{
	BasicAgent::fullState(exp);
}

void SyncedAgent::serverParam(const SExpression &exp)
{
	BasicAgent::serverParam(exp);
}

void SyncedAgent::playerParam(const SExpression &exp)
{
	BasicAgent::playerParam(exp);
}

void SyncedAgent::playerType(const SExpression &exp)
{
	BasicAgent::playerType(exp);
}

void SyncedAgent::init(const SExpression &exp)
{
	BasicAgent::init(exp);
}

void SyncedAgent::think(const SExpression &exp)
{
	BasicAgent::think(exp);
}

void SyncedAgent::changePlayerType(const SExpression &exp)
{
	BasicAgent::changePlayerType(exp);
}

bool SyncedAgent::decide()
{
	LOG << "SyncedAgent::decide" << endl;

	if (!isSenseBodyReceived())
		return true;

	string queue = "";

	// bodyCycleCommand Deciding
	if (worldModel->getCurTime() < BODY_CYCLE_COMMAND_LATE || worldModel->getBody().getTackleExpires() > 0)
		bodyCycleCommand = new EmptyCommand();

	if (isBodyCycleCommandDecidePermitted())
	{
		if (!bodyCycleCommand)
		{
			if (isSeeReceived())
			{
				bodyDecide(0);
			}
			else if (getSeeReceiveMS() > (SENSE_BODY_STEP / 2 - 3))
		    {
				bool wmUpdateNecessary = bodyDecide(0);

				if (getSeeReceiveMS() < (SENSE_BODY_STEP - 3) && wmUpdateNecessary)
					setQuickDecide(true);
			}
		}
	    else if (isSeeReceived() &&
				 isQuickDecide() &&
				 !isBodyCycleCommandSent())
		{
			bodyDecide(1);
			setQuickDecide(false);
		}
	}

	// bodyCycleCommand Sending
	if (bodyCycleCommand &&
		isBodyCycleCommandSendPermitted() &&
		!isBodyCycleCommandSent() &&
		!isQuickDecide() &&
		!isLateForSending())
	{
		queue += bodyCycleCommand->toString();
		setBodyCycleCommandSent(true);
	}

	// headCycleCommand and sayCycleCommand and PointToCycleCommand Deciding
	if (worldModel->getCurTime() < HEAD_CYCLE_COMMAND_LATE)
	{
		headCycleCommand = new EmptyCommand();
		changeViewCycleCommand = new EmptyCommand();

		sayCycleCommand = new EmptyCommand();
		attentionCycleCommand = new EmptyCommand();

		pointToCycleCommand = new EmptyCommand();
	}

	if (!headCycleCommand && isHeadCycleCommandDecidePermitted())
	{
		if (isSeeReceived())// || getSeeReceiveMS() > (SENSE_BODY_STEP - 3)) BY POORIA
		{
			headDecide();
			sayDecide();
			pointToDecide();
		}
	}

	// headCycleCommand and sayCycleCommand and PointToCycleCommand Sending
	if (headCycleCommand &&
		isHeadCycleCommandSendPermitted() &&
		!isHeadCycleCommandSent() &&
		!isLateForSending())
	{
		queue += headCycleCommand->toString();
		queue += changeViewCycleCommand->toString();
		setHeadCycleCommandSent(true);

		queue += sayCycleCommand->toString();
		queue += attentionCycleCommand->toString();

		queue += pointToCycleCommand->toString();

		// Added by Pooria
		if (not worldModel->isSeeSynch())
			queue += "(synch_see)";

		setSeeReceiveMSAfterChangeView(changeViewCycleCommand);
	}

	if (queue != "")
	{
		connection->send(queue);
		LOG << "SendStr: " << queue << endl;
	}

	return true;
}

bool SyncedAgent::synchronize()
{
//	LOG << "SyncedAgent::synchronize" << endl;
	string message;
	if (dynamic_cast<OnlineConnection *>(connection) && connection->receive(message) == 1)
	{
		dynamic_cast<OnlineConnection *>(connection)->setLocalBuffer(message);
		return true;
	}

	string queue = "";
	bool emergencySendFlag = false;

	// Emergency sending system
	if (isSenseBodyReceived() && worldModel->getTimer().now() < (SENSE_BODY_STEP - 2) &&
		((worldModel->getTimer().now() > getSeeReceiveMS() + (SENSE_BODY_STEP / 6.00) &&
		  getSeeReceiveMS() > (SENSE_BODY_STEP / 2.00)) || worldModel->getTimer().now() > (SENSE_BODY_STEP * 0.90)))
	{
		if (bodyCycleCommand && !isBodyCycleCommandSent())
		{
			LOG << "Emergency Sending bodyCycleCommand." << endl;
			queue += bodyCycleCommand->toString();
			setBodyCycleCommandSent(true);

			emergencySendFlag = true;
		}

		if (!headCycleCommand &&
			worldModel->getCurTime() >= HEAD_CYCLE_COMMAND_LATE)
		{
			LOG << "Emergency Sending headCycleCommand." << endl;

			bool shouldChangeView = true; // Added By Pooria
			if (seeReceiveMS > 100)
				shouldChangeView = false;
			headDecide();
			queue += headCycleCommand->toString();
			Share::lastCycleViewChanged = false;
			if (shouldChangeView) // Added By Pooria
			{
				Share::lastCycleViewChanged = true;
				queue += changeViewCycleCommand->toString();
			}
			setHeadCycleCommandSent(true);

			sayDecide();
			queue += sayCycleCommand->toString();
			queue += attentionCycleCommand->toString();

			pointToDecide();
			queue += pointToCycleCommand->toString();

			if (shouldChangeView) // Added By Pooria
				setSeeReceiveMSAfterChangeView(changeViewCycleCommand);
			emergencySendFlag = true;
		}

	}

	if (queue != "")
	{
		connection->send(queue);
		LOG << "SendStr: " << queue << endl;
	}

	if (emergencySendFlag)
		OFFLOG << "EmgSend" << endl;

	if (worldModel->getTimer().now() > SERVER_TIME_OUT)
	{
		LOG << "Connection timed out." << endl;
		cout << "Connection timed out." << endl;
		return false;
	}

	return true;
}

bool SyncedAgent::isLateForSending() const
{
	if (worldModel->getTimer().now() > (SENSE_BODY_STEP - 2))
		return true;
	return false;
}

void SyncedAgent::setSeeReceiveMSAfterSee()
{
	switch (worldModel->getBody().getVMWidth())
	{
	case VMW_NARROW:
		seeReceiveMS = curReceiveMS + (SEE_STEP / 2);
		break;
	case VMW_NORMAL:
		seeReceiveMS = curReceiveMS + SEE_STEP;
		break;
	case VMW_WIDE:
		seeReceiveMS = curReceiveMS + (SEE_STEP * 2);
		break;
	}

//	Added by Pooria
	if (worldModel->isSeeSynch())
	{
		switch (worldModel->getBody().getVMWidth())
		{
		case VMW_NARROW:
			seeReceiveMS = 30;
			break;
		case VMW_NORMAL:
			seeReceiveMS = 130;
			break;
		case VMW_WIDE:
			seeReceiveMS = 230;
			break;
		}
		seeReceiveMS += 100;
	}
//	if (worldModel->isSeeSynch() and isSeeReceived())
//		seeReceiveMS += 100;

	LOG << "seeReceiveMS After See: " << seeReceiveMS << endl;
}

void SyncedAgent::setSeeReceiveMSAfterSenseBody()
{
//	seeReceiveMS -= SENSE_BODY_STEP;
	seeReceiveMS -= 100;
/*
//	Added by Pooria
	if (worldModel->isSeeSynch())
	{
		switch (worldModel->getBody().getVMWidth())
		{
		case VMW_NARROW:
			seeReceiveMS = 30;
			break;
		case VMW_NORMAL:
			seeReceiveMS = 130;
			break;
		case VMW_WIDE:
			seeReceiveMS = 230;
			break;
		}
	}
	if (worldModel->isSeeSynch() and isSeeReceived())
	{
		seeReceiveMS += 100;
	}
*/
	LOG << "seeReceiveMS After Sense Body: " << seeReceiveMS << endl;
}

void SyncedAgent::setSeeReceiveMSAfterChangeView(const Command *command)
{
	const ChangeViewCommand *changeViewCommand = dynamic_cast<const ChangeViewCommand *>(command);

	if (!changeViewCommand)
		return;

	LOG << "SeeReceiveMSAfterChangeView" << endl;
// NOTICE
	seeReceiveMS = getLastSeeReceiveMS();

	switch (changeViewCommand->getWidth())
	{
	case VMW_NARROW:
		seeReceiveMS += (int)(SEE_STEP / 2);
		break;

	case VMW_NORMAL:
		seeReceiveMS += SEE_STEP;
		break;

	case VMW_WIDE:
		seeReceiveMS += SEE_STEP * 2;
		break;
	}

//	Added by Pooria
	if (worldModel->isSeeSynch())
	{
		switch (worldModel->getBody().getVMWidth())
		{
		case VMW_NARROW:
			seeReceiveMS = 30;
			break;
		case VMW_NORMAL:
			seeReceiveMS = 130;
			break;
		case VMW_WIDE:
			seeReceiveMS = 230;
			break;
		}
		seeReceiveMS += 100;
	}
//	if (worldModel->isSeeSynch() and isSeeReceived())
//		seeReceiveMS += 100;

	LOG << "seeReceiveMS After Change View: " << seeReceiveMS << endl;
}

void SyncedAgent::recoverSeeReceiveMSAfterChangeView(const Command *command)
{
	const ChangeViewCommand *changeViewCommand = dynamic_cast<const ChangeViewCommand *>(command);

	if (!changeViewCommand)
		return;

// NOTICE
	seeReceiveMS = getLastSeeReceiveMS();

/*	switch(changeViewCommand->getWidth())
	{
	case VMW_NARROW:
		seeReceiveMS -= (int)(SEE_STEP / 2);
		break;

	case VMW_NORMAL:
		seeReceiveMS -= SEE_STEP;
		break;

	case VMW_WIDE:
		seeReceiveMS -= SEE_STEP * 2;
		break;
	}
*/
//	Added by Pooria
	if (worldModel->isSeeSynch() and Share::lastCycleViewChanged)
	{
		switch (worldModel->getBody().getVMWidth())
		{
		case VMW_NARROW:
			seeReceiveMS -= 30;
			break;
		case VMW_NORMAL:
			seeReceiveMS -= 130;
			break;
		case VMW_WIDE:
			seeReceiveMS -= 230;
			break;
		}
	}
//	if (worldModel->isSeeSynch() and isSeeReceived())
//		seeReceiveMS += 100;

	LOG << "seeReceiveMS After Recover View: " << seeReceiveMS << endl;
}

int SyncedAgent::getLastSeeReceiveMS() const
{
// NOTICE
	return seeReceiveMS - 100;
}

void SyncedAgent::logEndInf()
{
	LOG << "End of cycle." << endl;

	OFFLOG << "EndInf: " << (int)(bodyCycleCommand != NULL)
						 << (int)(isBodyCycleCommandSent())
						 << (int)(headCycleCommand != NULL)
						 << (int)(isHeadCycleCommandSent()) << endl;
}

ViewModeWidth SyncedAgent::getSyncedWide() const
{
	return VMW_WIDE;
}

ViewModeWidth SyncedAgent::getSyncedNormal() const
{
	return VMW_NORMAL;
}

ViewModeWidth SyncedAgent::getSyncedNarrow() const
{
	return VMW_NARROW;
}

void SyncedAgent::checkForLosts()
{
	// Checking for bodyCycleCommand
	setBodyCycleCommandLost(false);

	if (bodyCycleCommand && !isBodyCycleCommandSent())
	{
		setBodyCycleCommandLost(true);
		LOG << "*** Body command didn't send ***" << endl;
	}
	else if (isBodyCycleCommandSent())
	{
		if ((dynamic_cast<KickCommand *>(bodyCycleCommand) &&
			 !worldModel->getBody().isKickCountChanged()) ||
			(dynamic_cast<DashCommand *>(bodyCycleCommand) &&
			 !worldModel->getBody().isDashCountChanged()) ||
			(dynamic_cast<TurnCommand *>(bodyCycleCommand) &&
			 !worldModel->getBody().isTurnCountChanged()) ||
			(dynamic_cast<CatchCommand *>(bodyCycleCommand) &&
			 !worldModel->getBody().isCatchCountChanged()) ||
			(dynamic_cast<MoveCommand *>(bodyCycleCommand) &&
			 !worldModel->getBody().isMoveCountChanged()) ||
			(dynamic_cast<TackleCommand *>(bodyCycleCommand) &&
			 !worldModel->getBody().isTackleCountChanged()))
		{
			if ((dynamic_cast<KickCommand *>(lastBodyCycleCommand) &&
				 worldModel->getBody().isKickCountChanged()) ||
				(dynamic_cast<DashCommand *>(lastBodyCycleCommand) &&
				 worldModel->getBody().isDashCountChanged()) ||
				(dynamic_cast<TurnCommand *>(lastBodyCycleCommand) &&
				 worldModel->getBody().isTurnCountChanged()) ||
				(dynamic_cast<CatchCommand *>(lastBodyCycleCommand) &&
				 worldModel->getBody().isCatchCountChanged()) ||
				(dynamic_cast<MoveCommand *>(lastBodyCycleCommand) &&
				 worldModel->getBody().isMoveCountChanged()) ||
				(dynamic_cast<TackleCommand *>(lastBodyCycleCommand) &&
				 worldModel->getBody().isTackleCountChanged()))
			{
				delete bodyCycleCommand;
				bodyCycleCommand = lastBodyCycleCommand;
				lastBodyCycleCommand = new EmptyCommand();

				LOG << "*** Last losted Body command executed ***" << endl;
			}
			else
			{
				setBodyCycleCommandLost(true);
				LOG << "*** Body command lost ***" << endl;
			}
		}
	}

	// Checking for headCycleCommand
	setHeadCycleCommandLost(false);

	if (headCycleCommand && !isHeadCycleCommandSent())
	{
		setHeadCycleCommandLost(true);
		LOG << "*** Head command didn't send ***" << endl;
	}
	else if (isHeadCycleCommandSent() &&
		!worldModel->getBody().isFocusCountChanged())
	{
		if (dynamic_cast<TurnNeckCommand *>(headCycleCommand) &&
			 !worldModel->getBody().isTurnNeckCountChanged())
		{
			setHeadCycleCommandLost(true);
			LOG << "*** Head command lost ***" << endl;
		}
	}
}

// Getting functions
int SyncedAgent::getSeeReceiveMS() const
{
	return seeReceiveMS;
}

bool SyncedAgent::isSeeReceived() const
{
	return seeReceivedFlag;
}

bool SyncedAgent::isSenseBodyReceived() const
{
	return senseBodyReceivedFlag;
}

bool SyncedAgent::isQuickDecide() const
{
	return quickDecideFlag;
}

bool SyncedAgent::isBodyCycleCommandSent() const
{
	return bodyCycleCommandSentFlag;
}

bool SyncedAgent::isHeadCycleCommandSent() const
{
	return headCycleCommandSentFlag;
}

bool SyncedAgent::isBodyCycleCommandLost() const
{
	return bodyCycleCommandLostFlag;
}

bool SyncedAgent::isHeadCycleCommandLost() const
{
	return headCycleCommandLostFlag;
}

// Setting functions
void SyncedAgent::setSeeReceiveMS(int seeReceiveMSArg)
{
	seeReceiveMS = seeReceiveMSArg;
}

void SyncedAgent::setSeeReceived(bool seeReceivedFlagArg)
{
	seeReceivedFlag = seeReceivedFlagArg;
}

void SyncedAgent::setSenseBodyReceived(bool senseBodyReceivedFlagArg)
{
	senseBodyReceivedFlag = senseBodyReceivedFlagArg;
}
void SyncedAgent::setQuickDecide(bool quickDecideFlagArg)
{
	quickDecideFlag = quickDecideFlagArg;
}

void SyncedAgent::setBodyCycleCommandSent(bool bodyCycleCommandSentFlagArg)
{
	bodyCycleCommandSentFlag = bodyCycleCommandSentFlagArg;
}

void SyncedAgent::setHeadCycleCommandSent(bool headCycleCommandSentFlagArg)
{
	headCycleCommandSentFlag = headCycleCommandSentFlagArg;
}

void SyncedAgent::setBodyCycleCommandLost(bool bodyCycleCommandLostFlagArg)
{
	bodyCycleCommandLostFlag = bodyCycleCommandLostFlagArg;
}

void SyncedAgent::setHeadCycleCommandLost(bool headCycleCommandLostFlagArg)
{
	headCycleCommandLostFlag = headCycleCommandLostFlagArg;
}
