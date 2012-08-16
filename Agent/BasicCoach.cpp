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
 *  	and is modified by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include <Config.h>
#include <Logger.h>
#include <Command.h>
#include <BasicCoach.h>
#include <SExpression.h>
#include <SignalHandler.h>
#include <OnlineConnection.h>
#include <VirtualConnection.h>

#include <cstring>

using namespace std;

BasicCoach::BasicCoach(int argc, char **argv)
{
	setConfigDefaults();
	config["Agent"].setByFile("./Configs/Coach.conf");
	if (!config["Agent"].setByArguments(argc, argv))
		exit(1);

	worldModel = new CoachWorldModel();
	freeFormCycleCommand = NULL;
	changePlayerCycleCommand = "";
	sigAlarmCounter = 0;
}

BasicCoach::~BasicCoach()
{
	delete worldModel;
}

void BasicCoach::run()
{
	if (config["Agent"]["Server"]["Offline"].asBool())
	{
		initVirtualConnection();
		offlinePlayerManager();
		closeVirtualConnection();
	}
	else
	{
		SignalHandler::initSignalHandlers(this);
		initOnlineConnection();
		SignalHandler::run();
		SignalHandler::finalSignalHandlers();
		closeOnlineConnection();
	}
}

void BasicCoach::ok(const SExpression &exp)
{
	LOG << "OK header received from server" << endl;
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;
	SExpAtomic *at = NULL;
	if (exp.size() > 1)
	{
		at = dynamic_cast<SExpAtomic *>(exp[1]);
		assert(at);
		string type = at->asString();
		if (type == "team_names")
			okTeamNames(exp);
		else if ( ! std::strncmp( exp.toString().c_str(), "(ok team_graphic ",
			std::strlen( "(ok team_graphic " ) ) )
		{
		        analyzeOKTeamGraphic( exp.toString().c_str() );
		}
		else
			LOG << "Skipping _ok_ message with unknown header" << endl;
	}
};

void BasicCoach::okTeamNames (const SExpression &exp)
{
	SExpression *expr =NULL;
	SExpAtomic  *at   =NULL;
	if (exp.size() > 2)
	{
		LOG << "Left Team exists" << endl;
		expr = dynamic_cast<SExpression * >(exp[2]); assert(expr);
		at   = dynamic_cast<SExpAtomic *  >((*expr)[0]); assert(at);
		assert(at->asString() == "team");
		at   = dynamic_cast<SExpAtomic *  >((*expr)[1]); assert(at);
		assert(at->asString() == "l");
		at   = dynamic_cast<SExpAtomic *  >((*expr)[2]); assert(at);
		LOG << "Setting left team name " << at->asString() << endl;
		worldModel->setLeftTeam(at->asString());
	}

	if (exp.size() > 3)
	{
		LOG << "Right team exists" << endl;
		expr = dynamic_cast<SExpression * >(exp[3]); assert(expr);
		at   = dynamic_cast<SExpAtomic *  >((*expr)[0]); assert(at);
		assert(at->asString() == "team");
		at   = dynamic_cast<SExpAtomic *  >((*expr)[1]); assert(at);
		assert(at->asString() == "r");
		at   = dynamic_cast<SExpAtomic *  >((*expr)[2]); assert(at);
		LOG << "Setting right team name " << at->asString() << endl;
		worldModel->setRightTeam(at->asString());
	}
};

void BasicCoach::seeGlobal(const SExpression &exp)
{
	unsigned curCycle;
	SExpAtomic *at;

	logEndInf();

	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);
	curCycle = at->asInt();

	worldModel->setTimer().resetCycle(curCycle);

	LOG.newCycle();
	DRAW.newCycle();
	if (!(curCycle % 20))
	{
		// every twenty cycles we check again for new teams
		updateTeamNames();
	}

	LOG << "RecStr: " << exp.toString() << endl;
//	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->setCurTime(worldModel->getCurTime() + 1);
	worldModel->resetObjects();
	worldModel->parseSeeGlobal(exp);

//	checkForLosts();

	worldModel->setCurCycle(curCycle);
//	worldModel->updateSeeDeltaCyclesAfterSenseBody();
	worldModel->updatePlayModeRemainCycleAfterSenseBody();

	worldModel->logObjects();
	worldModelHistory.saveWorldModel(worldModel, worldModel->getCurTime());
//	//TODO
}

void BasicCoach::hear(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->parseHear(exp);
}

void BasicCoach::fullState(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;
}

void BasicCoach::serverParam(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->initServerParam(exp);
}

void BasicCoach::playerParam(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->initPlayerParam(exp);
}

void BasicCoach::playerType(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->initPlayerType(exp);

	LOG << "Sending Eye On" << endl;
	Command *eye = new EyeCommand(true);
	connection->send(eye);
	delete eye;

}

void BasicCoach::init(const SExpression &exp)
{
	string logFileName, offLogFileName;
	SExpAtomic *at;

	at = dynamic_cast<SExpAtomic *>(exp[0]);
	assert(at);
	assert(at->toString() == "init");

	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);
	if (at->toString()[0] == 'l')
		worldModel->setFieldSide(SI_LEFT);
	else
	{
		assert(at->toString()[0] == 'r');
		worldModel->setFieldSide(SI_RIGHT);
	}

	// Initing log file
	if ((config["Agent"]["AgentLog"]
			   ["OutputLogAddress"].asString().end() - 1)[0] != '/')
		logFileName = "/";
	else
		logFileName = "";
	logFileName += config["Agent"]["Public"]["TeamName"].asString();

	logFileName += (char)('C');

	logFileName += ".log";
	if (config["Agent"]["AgentLog"]["LogToFile"].asBool())
		logger.add("MainLog", new LogFile(
				config["Agent"]["AgentLog"]
					  ["OutputLogAddress"].asString() + logFileName,
				&worldModel->getTimer()));
	else
		logger.add("MainLog", new LogNull());

	offLogFileName = logFileName + ".off";
	if (config["Agent"]["AgentLog"]["OffLogToFile"].asBool())
		logger.add("OfflineLog", new LogFile(
				config["Agent"]["AgentLog"]
					  ["OutputLogAddress"].asString() + offLogFileName,
				&worldModel->getTimer()));
	else
		logger.add("OfflineLog", new LogNull());

	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	cout << "Coach is connected." << endl;
	if (logger["MainLog"].isOutValid())
		cout << "    Log: " << config["Agent"]["AgentLog"]
			["OutputLogAddress"].asString() + logFileName << endl;
	if (logger["OfflineLog"].isOutValid())
		cout << "    OffLog: " << config["Agent"]["AgentLog"]
			["OutputLogAddress"].asString() + offLogFileName << endl;

	if (config["Agent"]["Public"]["UseTeamGraphic"].asBool())
		useTeamGraphic = true;
	else
		useTeamGraphic = false;
}

void BasicCoach::updateTeamNames() {
	if ((worldModel->getLeftTeam() =="NULL" )|| (worldModel->getRightTeam() =="NULL" )) {
		LOG << "Null team names exist, checking.." <<endl;
		Command * command = new TeamNamesCommand();
		connection->send(command);
		delete command;
	} else {
		LOG << "Team names are OK, not checking.. (" << worldModel->getLeftTeam() <<
			") (" << worldModel->getRightTeam() << ")" <<endl;
	}
};



void BasicCoach::think(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;
}

void BasicCoach::changePlayerType(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->parseChangePlayerType(exp);
}

bool BasicCoach::sigAlrmHandler()
{
/*	sigAlarmCounter++;

	if (sigAlarmCounter == 200)
	{
		Command *eye = new EyeCommand(true);
		connection->send(eye);
		delete eye;
	} else if (sigAlarmCounter == 250) {
	}
*/
	return synchronize();
}

bool BasicCoach::sigIntHandler()
{
	LOG << "Interrupt Handler." << endl;
	cout << "Interrupt Handler." << endl;
	return false;
}

bool BasicCoach::sigIOHandler()
{
	string message;
	while (connection->receive(message) == 1)
	{
		curReceiveMS = worldModel->getTimer().now();
		//std::cout << "SCRREC : " << message << std::endl;

		unsigned i = 0;
		SExpression exp(message, i);
//		assert(i + 1 == message.length());
		string header = ((SExpAtomic *)exp[0])->asString();

		if (header == "see_global")
			seeGlobal(exp);
		else if (header == "hear")
			hear(exp);
		else if (header == "fullstate")
			fullState(exp);
		else if (header == "server_param")
			serverParam(exp);
		else if (header == "player_param")
			playerParam(exp);
		else if (header == "player_type")
			playerType(exp);
		else if (header == "init")
			init(exp);
		else if (header == "think")
			think(exp);
		else if (header == "change_player_type")
			changePlayerType(exp);
		else if (header == "ok")
			ok(exp);
		else
			LOG << "Skipping message with unknown header: "
				<< exp.toString() << endl;
	}

	decide();

	return true;
}



bool BasicCoach::sigKillHandler()
{
	LOG << "Kill Handler." << endl;
	cout << "Kill Handler." << endl;
	return false;
}

bool BasicCoach::sigQuitHandler()
{
	LOG << "Quit Handler." << endl;
	cout << "Quit Handler." << endl;
	return false;
}
\

bool BasicCoach::sigSegVHandler()
{
	LOG << "Segmentation Violation Handler." << endl;
	cout << "Segmentation Violation Handler." << endl;

	exit(1);
	return false;
}

bool BasicCoach::sigTermHandler()
{
	LOG << "Termination Handler." << endl;
	cout << "Termination Handler." << endl;
	return false;
}

SignalsMask BasicCoach::signalsMask()
{
	SignalsMask result = SIG_INT | SIG_KILL | SIG_QUIT | SIG_SEGV | SIG_TERM;
	// if (online)
		result |= SIG_ALRM | SIG_IO;
	return result;
}

void BasicCoach::sigAlrmTiming(long &begin, long &interval)
{
	begin = interval = 10;
}

void BasicCoach::setConfigDefaults()
{
	config.add("Agent");

	config["Agent"].addGroup("Server");
	config["Agent"]["Server"].add("HostName","localhost");
	config["Agent"]["Server"].add("CoachPort","6000");
	config["Agent"]["Server"].add("Version","9.4.5");
	config["Agent"]["Server"].add("Offline","False");

	config["Agent"].addGroup("Public");
	config["Agent"]["Public"].add("TeamName","Mersad");
	config["Agent"]["Public"].add("IsGoalie","False");
	config["Agent"]["Public"].add("UniformNum","0");

	config["Agent"].addGroup("AgentLog");
	config["Agent"]["AgentLog"].add("LogToFile","False");
	config["Agent"]["AgentLog"].add("OffLogToFile","False");
	config["Agent"]["AgentLog"].add("InputLogAddress",".");
	config["Agent"]["AgentLog"].add("OutputLogAddress",".");

}


void BasicCoach::offlinePlayerManager()
{
	bool breakWhile = false;
	unsigned logHistoryNum = 0;
	ifstream inputLog;
	LogHistory logHistories[MAX_LOG_HOSTORY];

	VirtualConnection *virtualConnection =
			dynamic_cast<VirtualConnection *>(connection);
	assert(virtualConnection);

	VirtualTimer *virtualTimer =
			dynamic_cast<VirtualTimer *>(&worldModel->setTimer());

	if (!initOfflinePlayerInputLog(inputLog))
	{
		cout << "Offline Player Manager : can not open input log file for player "
			 << config["Agent"]["Public"]["UniformNum"].asInt() << endl;
		return;
	}

	long prevCycleSizeMS, cycleSizeMS = 0;
	while (!breakWhile)
	{
		// Finding "EndInf" title and making logHistory
		logHistoryNum = 0;
		prevCycleSizeMS = cycleSizeMS;

		while (1)
		{
			logHistories[logHistoryNum].milliSecond =
					parseOfflinePlayerLogLine(inputLog,
							logHistories[logHistoryNum].title,
							logHistories[logHistoryNum].message);

			if (logHistories[logHistoryNum].milliSecond == -1) // end of file
			{
/*				setBodyCycleCommandDecidePermitted(true);
				setBodyCycleCommandSendPermitted(true);
				setHeadCycleCommandDecidePermitted(true);
				setHeadCycleCommandSendPermitted(true);*/

				breakWhile = true;
				break;
			}
			else if (logHistories[logHistoryNum].title == "EndInf")
			{
/*				if (logHistories[logHistoryNum].message[0] - '0')
					setBodyCycleCommandDecidePermitted(true);
				else
					setBodyCycleCommandDecidePermitted(false);

				if (logHistories[logHistoryNum].message[1] - '0')
					setBodyCycleCommandSendPermitted(true);
				else
					setBodyCycleCommandSendPermitted(false);

				if (logHistories[logHistoryNum].message[2] - '0')
					setHeadCycleCommandDecidePermitted(true);
				else
					setHeadCycleCommandDecidePermitted(false);

				if (logHistories[logHistoryNum].message[3] - '0')
					setHeadCycleCommandSendPermitted(true);
				else
					setHeadCycleCommandSendPermitted(false);*/

				cycleSizeMS = logHistories[logHistoryNum].milliSecond;
				break;
			}
			else
				logHistoryNum++;
		}

		// Running LogHistory
		unsigned i;
		for (i = 0; i < logHistoryNum; i++)
		{
			if (logHistories[i].title == "RecStr")
			{
				virtualConnection->addMessage(logHistories[i].message);

				if (logHistories[i].message.substr(1, 10) == "sense_body")
					virtualTimer->setCurMS(prevCycleSizeMS);
				else
					virtualTimer->setCurMS(logHistories[i].milliSecond);

				for (i = i + 1; i < logHistoryNum; i++)
					if (logHistories[i].title == "RecStr" &&
						logHistories[i].message.substr(1, 4) == "hear" &&
						logHistories[i].message.find("referee") >
							logHistories[i].message.size())
							virtualConnection->addMessage(logHistories[i].message);
					else
					{
						i--; // This message is not Hear so I fix my change.
						break;
					}

				sigIOHandler();
			}
			else if (logHistories[i].title == "EmgSend")
			{
				virtualTimer->setCurMS(logHistories[i].milliSecond);
				sigAlrmHandler();
			}
		}
	}

	inputLog.close();

	LOG << "End of input offline log." << endl;
}

bool BasicCoach::initOfflinePlayerInputLog(ifstream &inputLog)
{
	ostringstream ss;

	ss << config["Agent"]["AgentLog"]["InputLogAddress"].asString()
		<< "/" << config["Agent"]["Public"]["TeamName"].asString()
		<< "C" << ".log.off.in";

	inputLog.open(ss.str().c_str());
	return inputLog;
}

int BasicCoach::parseOfflinePlayerLogLine(ifstream &inputLog,
		string &title, string &message)
{
	string line;
	string temp;
	int milliSecond;

	if (getline(inputLog, line))
	{
		unsigned colon = line.find(':');
		istringstream ss(line.substr(0, colon));
		ss >> milliSecond >> temp >> title;
		message = line.substr(colon + 2);
		return milliSecond;
	}
	return -1;
}

void BasicCoach::initOnlineConnection()
{
	Command * init = NULL;
	int port = -1;

	port = config["Agent"]["Server"]["CoachPort"].asInt();
 	init = new InitCommand(config["Agent"]["Public"]["TeamName"].asString(), config["Agent"]["Server"]["Version"].asString(), false);

	connection = new OnlineConnection(config["Agent"]["Server"]["HostName"].asString(), port);

	connection->send(init);
	delete init;
}

void BasicCoach::closeOnlineConnection()
{
	Command *bye = new ByeCommand();
	connection->send(bye);
	delete bye;

	delete connection;
	connection = NULL;
}

void BasicCoach::initVirtualConnection()
{
	connection = new VirtualConnection();
}

void BasicCoach::closeVirtualConnection()
{
	delete connection;
	connection = NULL;
}

/*
void BasicCoach::initVirtualConnection()
{
	connection = new VirtualConnection();
}

void BasicCoach::closeVirtualConnection()
{
	delete connection;
	connection = NULL;
}
*/

// Getting functions
/*
bool BasicCoach::isBodyCycleCommandDecidePermitted() const
{
	return bodyCycleCommandDecidePermittedFlag;
}

bool BasicCoach::isBodyCycleCommandSendPermitted() const
{
	return bodyCycleCommandSendPermittedFlag;
}

bool BasicCoach::isHeadCycleCommandDecidePermitted() const
{
	return headCycleCommandDecidePermittedFlag;
}

bool BasicCoach::isHeadCycleCommandSendPermitted() const
{
	return headCycleCommandSendPermittedFlag;
}
*/

const WorldModel &BasicCoach::getWorldModel() const
{
	return *worldModel;
}

// Setting functions
/*
void BasicCoach::setBodyCycleCommandDecidePermitted(bool bodyCycleCommandDecidePermittedFlagArg)
{
	bodyCycleCommandDecidePermittedFlag = bodyCycleCommandDecidePermittedFlagArg;
}

void BasicCoach::setBodyCycleCommandSendPermitted(bool bodyCycleCommandSendPermittedFlagArg)
{
	bodyCycleCommandSendPermittedFlag = bodyCycleCommandSendPermittedFlagArg;
}

void BasicCoach::setHeadCycleCommandDecidePermitted(bool headCycleCommandDecidePermittedFlagArg)
{
	headCycleCommandDecidePermittedFlag = headCycleCommandDecidePermittedFlagArg;
}

void BasicCoach::setHeadCycleCommandSendPermitted(bool headCycleCommandSendPermittedFlagArg)
{
	headCycleCommandSendPermittedFlag = headCycleCommandSendPermittedFlagArg;
}
*/

void BasicCoach::analyzeOKTeamGraphic( const char * msg )
{
    // "(ok team_graphic <x> <y>)"
    // "(ok team_graphic <x> <y>)"

    int x = -1, y = -1;

    if ( std::sscanf( msg,
                      "(ok team_graphic %d %d)",
                      &x, &y ) != 2
         || x < 0
         || y < 0 )
    {
        std::cout << " ESKILAS"
                  << " coach: "
                  << " recv illegal message. " << msg
                  << std::endl;
        return;
    }

    M_team_graphic_ok_set.insert( TeamGraphic::Index( x, y ) );
}
