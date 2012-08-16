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

#ifndef __SYNCED_AGENT_H
#define __SYNCED_AGENT_H

#include <BasicAgent.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

#define SERVER_TIME_OUT 9990 // ms
#define BODY_CYCLE_COMMAND_LATE 7 // cycle
#define HEAD_CYCLE_COMMAND_LATE 4 // cycle

#define SEE_STEP \
		(int)(worldModel->getServerParam()["send_step"].asFloat() * \
			  worldModel->getServerParam()["slow_down_factor"].asFloat())
#define SENSE_BODY_STEP \
		(int)(worldModel->getServerParam()["sense_body_step"].asFloat() * \
			  worldModel->getServerParam()["slow_down_factor"].asFloat())

class SyncedAgent: public BasicAgent
{
protected:
	int seeReceiveMS;
	bool seeReceivedFlag;
	bool senseBodyReceivedFlag;
	bool quickDecideFlag;

	bool bodyCycleCommandSentFlag;
	bool headCycleCommandSentFlag;
	bool bodyCycleCommandLostFlag;
	bool headCycleCommandLostFlag;

	virtual void senseBody(const SExpression &exp);
	virtual void see(const SExpression &exp);
	virtual void hear(const SExpression &exp);
	virtual void fullState(const SExpression &exp);
	virtual void serverParam(const SExpression &exp);
	virtual void playerParam(const SExpression &exp);
	virtual void playerType(const SExpression &exp);
	virtual void init(const SExpression &exp);
	virtual void think(const SExpression &exp);
	virtual void changePlayerType(const SExpression &exp);

	virtual bool decide();
	virtual bool synchronize();

	virtual void sayDecide() = 0;
	virtual void headDecide() = 0;
	virtual void pointToDecide() = 0;
	virtual bool bodyDecide(int decideLevel) = 0;

	bool isLateForSending() const;
	void setSeeReceiveMSAfterSee();
	void setSeeReceiveMSAfterSenseBody();
	void setSeeReceiveMSAfterChangeView(const Command *command);
	void recoverSeeReceiveMSAfterChangeView(const Command *command);

	virtual void logEndInf();

	virtual void checkForLosts();
	virtual bool isBodyCycleCommandLost() const;
	virtual bool isHeadCycleCommandLost() const;

	// Getting functions
	int getSeeReceiveMS() const;
	bool isSeeReceived() const;
	bool isSenseBodyReceived() const;
	bool isQuickDecide() const;
	bool isBodyCycleCommandSent() const;
	bool isHeadCycleCommandSent() const;

	// Setting functions
	void setSeeReceiveMS(int seeReceiveMSArg);
	void setSeeReceived(bool seeReceivedFlagArg);
	void setSenseBodyReceived(bool senseBodyReceivedFlagArg);
	void setQuickDecide(bool quickDecideFlagArg);
	void setBodyCycleCommandSent(bool bodyCycleCommandSentFlagArg);
	void setHeadCycleCommandSent(bool headCycleCommandSentFlagArg);
	void setBodyCycleCommandLost(bool bodyCycleCommandLostFlagArg);
	void setHeadCycleCommandLost(bool headCycleCommandLostFlagArg);

public:

	SyncedAgent(int argc, char **argv);
	virtual ~SyncedAgent();

	ViewModeWidth getSyncedWide() const;
	ViewModeWidth getSyncedNormal() const;
	ViewModeWidth getSyncedNarrow() const;

	// Getters
	int getLastSeeReceiveMS() const;

};

#endif // __SYNCED_AGENT_H
