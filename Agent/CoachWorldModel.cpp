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

#include <cmath>
#include <sstream>
#include <cassert>
#include <iostream>

#include <Config.h>
#include <Logger.h>
#include <Basics.h>
#include <Degree.h>
#include <Defines.h>
#include <CoachWorldModel.h>
#include <SayEncoding.h>

using namespace std;
using namespace Basics;
using namespace Degree;

// class CoachWorldModel

CoachWorldModel::CoachWorldModel() : leftTeam("NULL"), rightTeam("NULL")
{
}

CoachWorldModel::CoachWorldModel(const CoachWorldModel &worldModelCopy):
	/*WorldModel(worldModelCopy), AHMAD2006*/leftTeam("NULL"), rightTeam("NULL")
{
	LOG << "CoachWorldModel copied." << endl;
}

CoachWorldModel::~CoachWorldModel()
{
}

void CoachWorldModel::parseSeeGlobal(const SExpression &exp)
{
	SExpAtomic *at;
	SExpression *se1,*se2;
	int i;

	at = dynamic_cast<SExpAtomic *>(exp[0]);
	assert(at);
	assert(at->asString() == "see_global");

	// SeeGlobal time
	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);

	for (i = 2; i < exp.size(); i++)
	{
		se1 = dynamic_cast<SExpression *>(exp[i]);
		assert(se1);
		se2 = dynamic_cast<SExpression *>((*se1)[0]);
		assert(se2);
		at = dynamic_cast<SExpAtomic *>((*se2)[0]);
		assert(at);

		switch (at->asString()[0])
		{
		case 'g':
			parseGoal(*se1);
			break;

		case 'b':
			parseBall(*se1);
			break;

		case 'p':
			parsePlayer(*se1);
			break;

		default:
			assert(0);
		}
	}

}

void CoachWorldModel::parseGoal(const SExpression &exp)
{
	SExpression *se;
	int goalId;

	se = dynamic_cast<SExpression *>(exp[0]);
	assert(se);
	goalId = fieldData.getGoalId(*se);

	goals[goalsNum]->parseFullState(goalId, exp, getCurTime());
	goalsNum++;
}

void CoachWorldModel::parseBall(const SExpression &exp)
{
	ball->parseFullState(exp, getCurTime());
}

void CoachWorldModel::parsePlayer(const SExpression &exp)
{
	SExpression *se;
	SExpAtomic *at;
	string ourTeamName;
	TeamId teamId = TID_UNKNOWN;
	unsigned uniNum = 0;
	bool goalieFlag = false;

	se = dynamic_cast<SExpression *>(exp[0]);
	assert(se);

	at = dynamic_cast<SExpAtomic *>((*se)[1]);
	assert(at);

	ourTeamName = (string)"\"" +
			config["Agent"]["Public"]["TeamName"].asString() + "\"";

	if (at->toString() == ourTeamName)
		teamId = TID_TEAMMATE;
	else
		teamId = TID_OPPONENT;

	at = dynamic_cast<SExpAtomic *>((*se)[2]);
	assert(at);
	uniNum = at->asInt();

	if (se->size() >= 4)
	{
		at = dynamic_cast<SExpAtomic *>((*se)[3]);
		assert(at);
		if (at->toString()[0] == 'g')
			goalieFlag = true;
	}

	assert(uniNum <= FULL_PLAYERS_NUM);
	fullPlayers[teamId][uniNum - 1]->parseFullState(teamId, uniNum, goalieFlag, exp,
			getCurTime());
}

void CoachWorldModel::parseHear(const SExpression &exp)
{
	unsigned hearCycle;
	TeamId senderTeam;
	string ourTeamName;
	unsigned senderNum;
	SExpression *se;
	SExpAtomic *at;

	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);
	hearCycle = at->asInt();

	at = dynamic_cast<SExpAtomic *>(exp[2]);
	if (at && at->toString() == "referee")
	{
		parseRefereeHear(exp);
		refereeHearTime = curTime;
	}
	else // a player has said that.
	{
		se = dynamic_cast<SExpression *>(exp[2]);
		assert(se);

		ourTeamName = (string)"\"" +
			config["Agent"]["Public"]["TeamName"].asString() + "\"";

		at = dynamic_cast<SExpAtomic *>((*se)[1]);
		if (at->toString() == ourTeamName)
			senderTeam = TID_TEAMMATE;
		else
			senderTeam = TID_OPPONENT;

		at = dynamic_cast<SExpAtomic *>((*se)[2]);
		senderNum = at->asInt();
	}

}

void CoachWorldModel::logObjects()
{
	unsigned i;

	LOG << "Ball"
		<< " Pos" << ball->getPos()
		<< " Vel" << ball->getVel() << endl;

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_TEAMMATE][i]->isValid())
		{
			if (fullPlayers[TID_TEAMMATE][i]->isGoalie())
				LOG << "OurF" << i + 1
					<< " Pos" << fullPlayers[TID_TEAMMATE][i]->getPos()
					<< " Vel" << fullPlayers[TID_TEAMMATE][i]->getVel()
					<< " Bod" << fullPlayers[TID_TEAMMATE][i]->getBodyDir()
					<< " Hed" << fullPlayers[TID_TEAMMATE][i]->getHeadDir()
					<< " Nec" << fullPlayers[TID_TEAMMATE][i]->getPointingDir()
					<< " Tac" << fullPlayers[TID_TEAMMATE][i]->isTackling() << " G" << endl;
			else
				LOG << "OurF" << i + 1
					<< " Pos" << fullPlayers[TID_TEAMMATE][i]->getPos()
					<< " Vel" << fullPlayers[TID_TEAMMATE][i]->getVel()
					<< " Bod" << fullPlayers[TID_TEAMMATE][i]->getBodyDir()
					<< " Hed" << fullPlayers[TID_TEAMMATE][i]->getHeadDir()
					<< " Nec" << fullPlayers[TID_TEAMMATE][i]->getPointingDir()
					<< " Tac" << fullPlayers[TID_TEAMMATE][i]->isTackling() << endl;
		}

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_OPPONENT][i]->isValid())
		{
				if (fullPlayers[TID_OPPONENT][i]->isGoalie())
					LOG << "OppF" << i + 1
						<< " Pos" << fullPlayers[TID_OPPONENT][i]->getPos()
						<< " Vel" << fullPlayers[TID_OPPONENT][i]->getVel()
						<< " Bod" << fullPlayers[TID_OPPONENT][i]->getBodyDir()
						<< " Hed" << fullPlayers[TID_OPPONENT][i]->getHeadDir()
						<< " Nec" << fullPlayers[TID_OPPONENT][i]->getPointingDir()
						<< " Tac" << fullPlayers[TID_OPPONENT][i]->isTackling() << " G" << endl;
				else
					LOG << "OppF" << i + 1
						<< " Pos" << fullPlayers[TID_OPPONENT][i]->getPos()
						<< " Vel" << fullPlayers[TID_OPPONENT][i]->getVel()
						<< " Bod" << fullPlayers[TID_OPPONENT][i]->getBodyDir()
						<< " Hed" << fullPlayers[TID_OPPONENT][i]->getHeadDir()
						<< " Nec" << fullPlayers[TID_OPPONENT][i]->getPointingDir()
						<< " Tac" << fullPlayers[TID_OPPONENT][i]->isTackling() << endl;
		}
}

// CoachWorldModelHistory
CoachWorldModelHistory::CoachWorldModelHistory()
{
	curTime = -1;
	startTime = -1;
	size = 0;
	assert(COACH_WORLD_MODEL_HISTORY_SIZE >= 2);
}

CoachWorldModelHistory::~CoachWorldModelHistory()
{
}
/*
const CoachWorldModel **CoachWorldModelHistory::gethistory() const
{
	return history;
}
*/
int CoachWorldModelHistory::getSize() const
{
	return size;
}

int CoachWorldModelHistory::getCurTime() const
{
	return curTime;
}

int CoachWorldModelHistory::getStartTime() const
{
	return startTime;
}

CoachWorldModel **CoachWorldModelHistory::sethistory()
{
	return history;
}

void CoachWorldModelHistory::setSize(int sizeArg)
{
	size = sizeArg;
}

void CoachWorldModelHistory::setCurTime(int curTimeArg)
{
	curTime = curTimeArg;
}

void CoachWorldModelHistory::setStartTime(int startTimeArg)
{
	startTime = startTimeArg;
}

void CoachWorldModelHistory::saveWorldModel(CoachWorldModel *worldModel,
		unsigned time)
{
/*	int i;
	CoachWorldModel *newWorldModel;
	LOG << "Saving WorldModel time= " << time << endl;
	assert (size <= COACH_WORLD_MODEL_HISTORY_SIZE);
	if (size == COACH_WORLD_MODEL_HISTORY_SIZE)
	{
		LOG << "Shifting history" << endl;

		newWorldModel = history[0];

		for (i = 1; i < COACH_WORLD_MODEL_HISTORY_SIZE; i++)
		{
			history[i - 1] = history[i];
		}

		newWorldModel->copyFrom(*worldModel);
		history[COACH_WORLD_MODEL_HISTORY_SIZE - 1] = newWorldModel;

		startTime = time - COACH_WORLD_MODEL_HISTORY_SIZE + 1;
		curTime = time;
	}
	else
	{
		LOG << "insert to history" << endl;
		history[size] = new CoachWorldModel(*worldModel);

		if (startTime == -1)
			startTime = time;
		curTime = time;
		size++;
	}

	LOG << "Now history size is " << size << endl;
	LOG << "curTime " << curTime << endl;
	LOG << "startTime " << startTime << endl;
					AHMAD2006			*/
}

const CoachWorldModel *CoachWorldModelHistory::operator[](int time) const
{
	if (time >= 0)
	{
		assert(time <= curTime);
		assert(time >= startTime);
		return history[time - startTime];
	}
	else
	{
		assert(-time <= size - 1);
		return history[(size - 1) + time];
	}
}

void CoachWorldModel::setLeftTeam (const string & name) {
	leftTeam = name;
};

void CoachWorldModel::setRightTeam (const string & name) {
	rightTeam = name;
};

const string & CoachWorldModel::getLeftTeam () const { return leftTeam;};
const string & CoachWorldModel::getRightTeam () const { return rightTeam;};
