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
 *  	and is modified by: Pooria Kaviani
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
#include <BasicAgent.h>
#include <SExpression.h>
#include <SignalHandler.h>
#include <OnlineConnection.h>
#include <VirtualConnection.h>

using namespace std;

BasicAgent::BasicAgent(int argc, char **argv)
{
	setConfigDefaults();
	config["Agent"].setByFile("./Configs/Agent.conf");
	if (!config["Agent"].setByArguments(argc, argv))
		exit(1);

	worldModel = new WorldModel();

	sayCycleCommand = NULL;
	bodyCycleCommand = NULL;
	headCycleCommand = NULL;
	pointToCycleCommand = NULL;
	attentionCycleCommand = NULL;
	changeViewCycleCommand = NULL;

	lastBodyCycleCommand = new EmptyCommand();

	setBodyCycleCommandDecidePermitted(true);
	setBodyCycleCommandSendPermitted(true);
	setHeadCycleCommandDecidePermitted(true);
	setHeadCycleCommandSendPermitted(true);
}

BasicAgent::~BasicAgent()
{
	delete worldModel;
}

void BasicAgent::run()
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

void BasicAgent::senseBody(const SExpression &exp)
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
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->setCurTime(worldModel->getCurTime() + 1);

	worldModel->setBody().parseSenseBody(exp, worldModel->getFieldSide());
	checkForLosts();

	if (!isBodyCycleCommandLost())
		worldModel->setBody().simulateByAction(bodyCycleCommand, true);

	worldModel->setBody().simulateByDynamics(true);
	worldModel->setBody().completeParseSenseBody();

	const Body lastBody = worldModel->getBody(); // for ball simulate

	if (!isBodyCycleCommandLost())
		worldModel->simulateObjects(BS_NONE, lastBody, bodyCycleCommand);
	else
		worldModel->simulateObjects(BS_NONE, lastBody, NULL);

	worldModel->setCurCycle(worldModel->getBody().getSenseBodyCycle());
	worldModel->updateSeeDeltaCyclesAfterSenseBody();
	worldModel->updatePlayModeRemainCycleAfterSenseBody();

	worldModel->updateAfterSenseBody();

	worldModel->logObjects();
}

void BasicAgent::see(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->see(exp);
}

void BasicAgent::hear(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->parseHear(exp);
}

void BasicAgent::fullState(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;
}

void BasicAgent::serverParam(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->initServerParam(exp);
}

void BasicAgent::playerParam(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->initPlayerParam(exp);
}

void BasicAgent::playerType(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->initPlayerType(exp);
}

void BasicAgent::init(const SExpression &exp)
{
	string logFileName, offLogFileName;
	SExpAtomic *at;
	int bodyUniNum;

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

	at = dynamic_cast<SExpAtomic *>(exp[2]);
	assert(at);
	assert(at->asInt() <= FULL_PLAYERS_NUM);
	bodyUniNum = at->asInt();

	worldModel->setBody().setUniNum(bodyUniNum);
	worldModel->setBody().setGoalie(
			config["Agent"]["Public"]["IsGoalie"].asBool());
	worldModel->replaceBodyInPlayersList(bodyUniNum);

	at = dynamic_cast<SExpAtomic *>(exp[3]);
	assert(at);
	assert(at->toString() == "before_kick_off");

	// Initing log file
	if ((config["Agent"]["AgentLog"]
			   ["OutputLogAddress"].asString().end() - 1)[0] != '/')
		logFileName = "/";
	else
		logFileName = "";
	logFileName += config["Agent"]["Public"]["TeamName"].asString();

	if (bodyUniNum < 10)
		logFileName += (char)('0' + bodyUniNum);
	else
		logFileName += (char)('A' + bodyUniNum - 10);

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

	string drawLogFileName = logFileName + ".draw";
	if (config["Agent"]["AgentLog"]["DrawLogToFile"].asBool())
		logger.add("DrawLog", new LogFile(
				config["Agent"]["AgentLog"]
					  ["DrawLogAddress"].asString() + drawLogFileName,
				&worldModel->getTimer()));
	else
		logger.add("DrawLog", new LogNull());


	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	cout << "Player " << bodyUniNum << " connected." << endl;
	if (logger["MainLog"].isOutValid())
		cout << "    Log: " << config["Agent"]["AgentLog"]
			["OutputLogAddress"].asString() + logFileName << endl;
	if (logger["OfflineLog"].isOutValid())
		cout << "    OffLog: " << config["Agent"]["AgentLog"]
			["OutputLogAddress"].asString() + offLogFileName << endl;

	Command *clangVer = new CLangVersionCommand(7, 9);
	LOG << "SendStr: " << clangVer->toString() << endl;
	connection->send(clangVer);
	delete clangVer;
}

void BasicAgent::think(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;
}

void BasicAgent::changePlayerType(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->parseChangePlayerType(exp);
}

void BasicAgent::ok(const SExpression &exp)
{
	LOG << "RecStr: " << exp.toString() << endl;
	OFFLOG << "RecStr: " << exp.toString() << endl;

	worldModel->parseOk(exp);
}

bool BasicAgent::sigAlrmHandler()
{
	return synchronize();
}

bool BasicAgent::sigIntHandler()
{
	LOG << "Interrupt Handler." << endl;
	cout << "Interrupt Handler." << endl;
	return false;
}

bool BasicAgent::sigIOHandler()
{
	bool iReceivedSomething = false;

	string message;
	while (connection->receive(message) == 1)
	{
		iReceivedSomething = true;

		curReceiveMS = worldModel->getTimer().now();

		unsigned i = 0;
		SExpression exp(message, i);
//		assert(i + 1 == message.length());
		string header = ((SExpAtomic *)exp[0])->asString();

		if (header == "sense_body")
			senseBody(exp);
		else if (header == "see")
			see(exp);
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

	if (iReceivedSomething)
		decide();

	return true;
}

bool BasicAgent::sigKillHandler()
{
	LOG << "Kill Handler." << endl;
	cout << "Kill Handler." << endl;
	return false;
}

bool BasicAgent::sigQuitHandler()
{
	LOG << "Quit Handler." << endl;
	cout << "Quit Handler." << endl;
	return false;
}

bool BasicAgent::sigSegVHandler()
{
	LOG << "Segmentation Violation Handler." << endl;
	cout << "Segmentation Violation Handler." << endl;

	exit(1);
	return false;
}

bool BasicAgent::sigTermHandler()
{
	LOG << "Termination Handler." << endl;
	cout << "Termination Handler." << endl;
	return false;
}

bool BasicAgent::sigAbrtHandler()
{
	LOG << "Abort Handler." << endl;
	cout << "Abort Handler." << endl;
	return false;
}

SignalsMask BasicAgent::signalsMask()
{
	SignalsMask result = SIG_INT | SIG_KILL | SIG_QUIT | SIG_SEGV | SIG_TERM;
	// if (online)
		result |= SIG_ALRM | SIG_IO;
	return result;
}

void BasicAgent::sigAlrmTiming(long &begin, long &interval)
{
	begin = interval = 10;
}

void BasicAgent::setConfigDefaults()
{
	config.add("Agent");

	config["Agent"].addGroup("Server");
	config["Agent"]["Server"].add("HostName", "localhost");
	config["Agent"]["Server"].add("PlayerPort", "6000");
	config["Agent"]["Server"].add("Version", "9.4.5");
	config["Agent"]["Server"].add("Offline", "False");

	config["Agent"].addGroup("Public");
	config["Agent"]["Public"].add("TeamName", "Ahmad");
	config["Agent"]["Public"].add("IsGoalie", "False");
	config["Agent"]["Public"].add("UniformNum", "0");
	config["Agent"]["Public"].add("LatestTrick", "False");

	config["Agent"].addGroup("AgentLog");
	config["Agent"]["AgentLog"].add("LogToFile", "False");
	config["Agent"]["AgentLog"].add("OffLogToFile", "False");
	config["Agent"]["AgentLog"].add("InputLogAddress", ".");
	config["Agent"]["AgentLog"].add("OutputLogAddress", ".");

}

void BasicAgent::offlinePlayerManager()
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

	if (config["Agent"]["Public"]["UniformNum"].asInt() <= 0)
	{
		cout << "Offline Player Manager : you must specify player number." << endl
			 << "Type \"--number pnum\" after command." << endl;
		return;
	}

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
				setBodyCycleCommandDecidePermitted(true);
				setBodyCycleCommandSendPermitted(true);
				setHeadCycleCommandDecidePermitted(true);
				setHeadCycleCommandSendPermitted(true);

				breakWhile = true;
				break;
			}
			else if (logHistories[logHistoryNum].title == "EndInf")
			{
				if (logHistories[logHistoryNum].message[0] - '0')
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
					setHeadCycleCommandSendPermitted(false);

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

bool BasicAgent::initOfflinePlayerInputLog(ifstream &inputLog)
{
	ostringstream ss;

	if (config["Agent"]["Public"]["UniformNum"].asInt() < 10)
		ss << config["Agent"]["AgentLog"]["InputLogAddress"].asString()
			<< "/" << config["Agent"]["Public"]["TeamName"].asString()
			<< config["Agent"]["Public"]["UniformNum"].asInt()
			<< ".log.off.in";
	else
		ss << config["Agent"]["AgentLog"]["InputLogAddress"].asString()
			<< "/" << config["Agent"]["Public"]["TeamName"].asString()
			<< (char)(config["Agent"]["Public"]["UniformNum"].asInt() - 10 + 'A')
			<< ".log.off.in";

	inputLog.open(ss.str().c_str());
	return inputLog;
}

int BasicAgent::parseOfflinePlayerLogLine(ifstream &inputLog,
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
		if (colon < line.size())
			message = line.substr(colon + 2);
		else
			message = "";
		return milliSecond;
	}
	return -1;
}

void BasicAgent::initOnlineConnection()
{
	connection = new OnlineConnection(
		config["Agent"]["Server"]["HostName"].asString(),
		config["Agent"]["Server"]["PlayerPort"].asInt());

	Command *init = new InitCommand(config["Agent"]["Public"]["TeamName"].asString(),
		config["Agent"]["Server"]["Version"].asString(),
		config["Agent"]["Public"]["IsGoalie"].asBool());

	connection->send(init);
	delete init;
}

void BasicAgent::closeOnlineConnection()
{
	Command *bye = new ByeCommand();
	connection->send(bye);
	delete bye;

	delete connection;
	connection = NULL;
}

void BasicAgent::initVirtualConnection()
{
	connection = new VirtualConnection();
}

void BasicAgent::closeVirtualConnection()
{
	delete connection;
	connection = NULL;
}

void BasicAgent::setBodyLowNarrow()
{ // It is a dirty TOF (by Ahmad 03 Khordad 1385)
	worldModel->setBody().setVMQuality(VMQ_LOW);
	worldModel->setBody().setVMWidth(VMW_NARROW);
}


// Getting functions
bool BasicAgent::isBodyCycleCommandDecidePermitted() const
{
	return bodyCycleCommandDecidePermittedFlag;
}

bool BasicAgent::isBodyCycleCommandSendPermitted() const
{
	return bodyCycleCommandSendPermittedFlag;
}

bool BasicAgent::isHeadCycleCommandDecidePermitted() const
{
	return headCycleCommandDecidePermittedFlag;
}

bool BasicAgent::isHeadCycleCommandSendPermitted() const
{
	return headCycleCommandSendPermittedFlag;
}

const WorldModel &BasicAgent::getWorldModel() const
{
	return *worldModel;
}

int BasicAgent::getCurReceiveMS() const
{
	return curReceiveMS;
}

// Setting functions
void BasicAgent::setBodyCycleCommandDecidePermitted(bool bodyCycleCommandDecidePermittedFlagArg)
{
	bodyCycleCommandDecidePermittedFlag = bodyCycleCommandDecidePermittedFlagArg;
}

void BasicAgent::setBodyCycleCommandSendPermitted(bool bodyCycleCommandSendPermittedFlagArg)
{
	bodyCycleCommandSendPermittedFlag = bodyCycleCommandSendPermittedFlagArg;
}

void BasicAgent::setHeadCycleCommandDecidePermitted(bool headCycleCommandDecidePermittedFlagArg)
{
	headCycleCommandDecidePermittedFlag = headCycleCommandDecidePermittedFlagArg;
}

void BasicAgent::setHeadCycleCommandSendPermitted(bool headCycleCommandSendPermittedFlagArg)
{
	headCycleCommandSendPermittedFlag = headCycleCommandSendPermittedFlagArg;
}
