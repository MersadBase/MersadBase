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

#ifndef __BASIC_COACH_H
#define __BASIC_COACH_H

#include <iostream>

#include <SignalsMask.h>
#include <Connection.h>
#include <CoachWorldModel.h>
#include <SignalHandled.h>
#include <BasicAgent.h>

#include <TeamGraphic.h>
#include <set>

#ifndef NULL
#	define NULL 0
#endif // NULL


#define MAX_LOG_HOSTORY 50

class Command;
class SExpression;

class BasicCoach: public SignalHandled
{
protected:
	Connection *connection;
	CoachWorldModel *worldModel;
	CoachWorldModelHistory worldModelHistory;

	Command *freeFormCycleCommand;
	std::string changePlayerCycleCommand;

	int curReceiveMS;
	int sigAlarmCounter;
/*
	bool freeFormCycleCommandDecidePermittedFlag;
	bool CycleCommandSendPermittedFlag;
	bool headCycleCommandDecidePermittedFlag;
	bool headCycleCommandSendPermittedFlag;
*/
	void setConfigDefaults();
	void initOnlineConnection();
	void closeOnlineConnection();
	void initVirtualConnection();
	void closeVirtualConnection();

	void offlinePlayerManager();
	bool initOfflinePlayerInputLog(std::ifstream &inputLog);
	int parseOfflinePlayerLogLine(std::ifstream &inputLog,
			std::string &title, std::string &message);

	virtual void logEndInf() = 0;

	virtual void ok(const SExpression &exp);
	virtual void okTeamNames(const SExpression &exp);
	virtual void seeGlobal(const SExpression &exp);
	virtual void fullState(const SExpression &exp);
	virtual void hear(const SExpression &exp);
	virtual void serverParam(const SExpression &exp);
	virtual void playerParam(const SExpression &exp);
	virtual void playerType(const SExpression &exp);
	virtual void init(const SExpression &exp);
	virtual void think(const SExpression &exp);
	virtual void changePlayerType(const SExpression &exp);

	virtual bool decide() = 0;
	virtual bool synchronize() = 0;

	// *************
	bool useTeamGraphic;
	std::set< TeamGraphic::Index > M_team_graphic_ok_set;

	void analyzeOKTeamGraphic( const char * msg );

	const std::set< TeamGraphic::Index > & teamGraphicOKSet() const
	{
		return M_team_graphic_ok_set;
	}
	// *************

private:
	void updateTeamNames();

public:
	BasicCoach(int argc, char **argv);
	virtual ~BasicCoach();

	virtual void run();

	virtual bool sigAlrmHandler();
	virtual bool sigIntHandler();
	virtual bool sigIOHandler();
	virtual bool sigKillHandler();
	virtual bool sigQuitHandler();
	virtual bool sigSegVHandler();
	virtual bool sigTermHandler();

	virtual SignalsMask signalsMask();
	virtual void sigAlrmTiming(long &begin, long &interval);

	// Getting functions
/*
	bool isBodyCycleCommandDecidePermitted() const;
	bool isBodyCycleCommandSendPermitted() const;
	bool isHeadCycleCommandDecidePermitted() const;
	bool isHeadCycleCommandSendPermitted() const;
*/
	const WorldModel &getWorldModel() const;

	// Setting functions
/*
	void setBodyCycleCommandDecidePermitted(bool bodyCycleCommandDecidePermittedFlagArg);
	void setBodyCycleCommandSendPermitted(bool bodyCycleCommandSendPermittedFlagArg);
	void setHeadCycleCommandDecidePermitted(bool headCycleCommandDecidePermittedFlagArg);
	void setHeadCycleCommandSendPermitted(bool headCycleCommandSendPermittedFlagArg);
*/

};

#endif // __BASIC_COACH_H
