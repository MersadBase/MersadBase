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

#ifndef __SYNCED_COACH_H
#define __SYNCED_COACH_H

#include <BasicCoach.h>

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

class SyncedCoach: public BasicCoach
{
protected:
	bool seeGlobalReceivedFlag;

	bool freeFormCycleCommandSentFlag;
	bool changePlayerCycleCommandSentFlag;

	virtual void seeGlobal(const SExpression &exp);
	virtual void hear(const SExpression &exp);
	virtual void fullState(const SExpression &exp);
	virtual void serverParam(const SExpression &exp);
	virtual void playerParam(const SExpression &exp);
	virtual void playerType(const SExpression &exp);
	virtual void init(const SExpression &exp);
	virtual void think(const SExpression &exp);
	virtual void changePlayerType(const SExpression &exp);

	virtual bool process() = 0;
	virtual bool decide();
	virtual bool synchronize();

	virtual void logEndInf();

	// Getting functions
	bool isSeeGlobalReceived() const;
	bool isFreeFormCycleCommandSent() const;
	bool isChangePlayerCycleCommandSent() const;

	// Setting functions
	void setSeeGlobalReceived(bool seeGlobalReceivedFlagArg);
	void setFreeFormCycleCommandSent(bool freeFormCycleCommandSentFlagArg);
	void setChangePlayerCycleCommandSent(bool changePlayerCycleCommandSentFlagArg);

public:
	SyncedCoach(int argc, char **argv);
	virtual ~SyncedCoach();
};

#endif // __SYNCED_COACH_H
