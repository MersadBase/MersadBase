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

#ifndef __BASIC_AGENT_H
#define __BASIC_AGENT_H

#include <iostream>

#include <SignalsMask.h>
#include <Connection.h>
#include <WorldModel.h>
#include <SignalHandled.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

#define MAX_LOG_HOSTORY 50

class Command;
class SExpression;

struct LogHistory
{
	long milliSecond;
	std::string title;
	std::string message;
};

class BasicAgent: public SignalHandled
{
protected:
	Connection *connection;
	WorldModel *worldModel;

	Command *sayCycleCommand;
	Command *bodyCycleCommand;
	Command *headCycleCommand;
	Command *pointToCycleCommand;
	Command *attentionCycleCommand;
	Command *changeViewCycleCommand;

	Command *lastBodyCycleCommand; // is used for Lost commands.

	int curReceiveMS;

	bool bodyCycleCommandDecidePermittedFlag;
	bool bodyCycleCommandSendPermittedFlag;
	bool headCycleCommandDecidePermittedFlag;
	bool headCycleCommandSendPermittedFlag;

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

	virtual void checkForLosts() = 0;
	virtual bool isBodyCycleCommandLost() const = 0;
	virtual bool isHeadCycleCommandLost() const = 0;

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
	virtual void ok(const SExpression &exp);

	virtual bool decide() = 0;
	virtual bool synchronize() = 0;

public:
	BasicAgent(int argc, char **argv);
	virtual ~BasicAgent();

	virtual void run();

	virtual bool sigAlrmHandler();
	virtual bool sigIntHandler();
	virtual bool sigIOHandler();
	virtual bool sigKillHandler();
	virtual bool sigQuitHandler();
	virtual bool sigSegVHandler();
	virtual bool sigTermHandler();
	virtual bool sigAbrtHandler();

	virtual SignalsMask signalsMask();
	virtual void sigAlrmTiming(long &begin, long &interval);

	void setBodyLowNarrow(); // It is a dirty TOF (by Ahmad 03 Khordad 1385)

	// Getting functions
	bool isBodyCycleCommandDecidePermitted() const;
	bool isBodyCycleCommandSendPermitted() const;
	bool isHeadCycleCommandDecidePermitted() const;
	bool isHeadCycleCommandSendPermitted() const;
	const WorldModel &getWorldModel() const;
	int getCurReceiveMS() const;

	// Setting functions
	void setBodyCycleCommandDecidePermitted(bool bodyCycleCommandDecidePermittedFlagArg);
	void setBodyCycleCommandSendPermitted(bool bodyCycleCommandSendPermittedFlagArg);
	void setHeadCycleCommandDecidePermitted(bool headCycleCommandDecidePermittedFlagArg);
	void setHeadCycleCommandSendPermitted(bool headCycleCommandSendPermittedFlagArg);

};

#endif // __BASIC_AGENT_H
