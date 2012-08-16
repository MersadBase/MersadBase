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
 *  This file is created by: Pooria Kaviani
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <iostream>

#include <Share.h>
#include <SyncedCoach.h>

using namespace std;

SyncedCoach::SyncedCoach(int argc, char **argv): BasicCoach(argc, argv)
{
	freeFormCycleCommandSentFlag = false;
	changePlayerCycleCommandSentFlag = false;
}

SyncedCoach::~SyncedCoach()
{
}

void SyncedCoach::seeGlobal(const SExpression &exp)
{
	BasicCoach::seeGlobal(exp);

	delete freeFormCycleCommand;
	freeFormCycleCommand = NULL;
	setFreeFormCycleCommandSent(false);

	changePlayerCycleCommand = "";
	setChangePlayerCycleCommandSent(false);

	setSeeGlobalReceived(true);
}

void SyncedCoach::hear(const SExpression &exp)
{
	BasicCoach::hear(exp);
}

void SyncedCoach::fullState(const SExpression &exp)
{
	BasicCoach::fullState(exp);
}

void SyncedCoach::serverParam(const SExpression &exp)
{
	BasicCoach::serverParam(exp);
}

void SyncedCoach::playerParam(const SExpression &exp)
{
	BasicCoach::playerParam(exp);
}

void SyncedCoach::playerType(const SExpression &exp)
{
	BasicCoach::playerType(exp);
}

void SyncedCoach::init(const SExpression &exp)
{
	BasicCoach::init(exp);
}

void SyncedCoach::think(const SExpression &exp)
{
	BasicCoach::think(exp);
}

void SyncedCoach::changePlayerType(const SExpression &exp)
{
	BasicCoach::changePlayerType(exp);
}

bool SyncedCoach::decide()
{
	if (!isSeeGlobalReceived())
		return true;

	string queue = "";

	// bodyCycleCommand Deciding
	if (!freeFormCycleCommand || changePlayerCycleCommand == "")
	{
		process();
	}

	// bodyCycleCommand Sending
	if (freeFormCycleCommand and !isFreeFormCycleCommandSent())
	{
		queue += freeFormCycleCommand->toString();
		setFreeFormCycleCommandSent(true);
	}

	if (changePlayerCycleCommand != "" and !isChangePlayerCycleCommandSent())
	{
		queue += changePlayerCycleCommand;
		setChangePlayerCycleCommandSent(true);
	}

	if (queue != "")
	{
		connection->send(queue);
	}

	return true;
}

bool SyncedCoach::synchronize()
{
	if (worldModel->getTimer().now() > SERVER_TIME_OUT)
	{
		cout << "Server does not respond in the expected time." << endl;
		return false;
	}

	return true;
}

void SyncedCoach::logEndInf()
{
}

bool SyncedCoach::isSeeGlobalReceived() const
{
	return seeGlobalReceivedFlag;
}

bool SyncedCoach::isFreeFormCycleCommandSent() const
{
	return freeFormCycleCommandSentFlag;
}

bool SyncedCoach::isChangePlayerCycleCommandSent() const
{
	return changePlayerCycleCommandSentFlag;
}

void SyncedCoach::setSeeGlobalReceived(bool seeGlobalReceivedFlagArg)
{
	seeGlobalReceivedFlag = seeGlobalReceivedFlagArg;
}

void SyncedCoach::setFreeFormCycleCommandSent(bool freeFormCycleCommandSentFlagArg)
{
	freeFormCycleCommandSentFlag = freeFormCycleCommandSentFlagArg;
}

void SyncedCoach::setChangePlayerCycleCommandSent(bool changePlayerCycleCommandSentFlagArg)
{
	changePlayerCycleCommandSentFlag = changePlayerCycleCommandSentFlagArg;
}
