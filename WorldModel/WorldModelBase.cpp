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
 *  	and is modified by: Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <cassert>
#include <iostream>

#include <Logger.h>
#include <Basics.h>
#include <Degree.h>
#include <WorldModelBase.h>

using namespace std;
using namespace Basics;
using namespace Degree;

WorldModelBase::WorldModelBase()
{
	lastGoalieSeeCounter = 0;
	unsigned i;

	if (config["Agent"]["Server"]["Offline"].asBool())
		timer = new VirtualTimer();
	else
		timer = new OnlineTimer();

	seeCycle = 0;
	curCycle = 0;
	curTime = 0;
	fieldSide = SI_LEFT;
	playMode = PM_BEFORE_KICK_OFF;
	virtualPlayMode = VPM_NONE;
	playModeRemainCycle = 0;

	setBodyReplaced(false);

	initViewPartDirs();
	resetSeeDeltaCycles();

	// Objects on the field.
	flagsNum = 0;
	goalsNum = 0;
	linesNum = 0;

	for (i = 0; i < FLAGS_NUM; i++)
		flags[i] = new FieldFlag();
	for (i = 0; i < GOALS_NUM; i++)
		goals[i] = new FieldGoal();
	for (i = 0; i < LINES_NUM; i++)
		lines[i] = new FieldLine();

	ball = new Ball;
	body = new Body;

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		fullPlayers[TID_TEAMMATE][i] = new Player(PLM_FULL);
		fullPlayers[TID_TEAMMATE][i]->setUniNum(i + 1);
	}
	for (i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		fullPlayers[TID_OPPONENT][i] = new Player(PLM_FULL);
		fullPlayers[TID_OPPONENT][i]->setUniNum(i + 1);
	}

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		halfPlayers[TID_TEAMMATE][i] = new Player(PLM_HALF);
	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		halfPlayers[TID_OPPONENT][i] = new Player(PLM_HALF);

	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		quarterPlayers[i] = new Player(PLM_QUARTER);

	seeSynch = false;
	opponentTeamName = "";
}

WorldModelBase::~WorldModelBase()
{
	delete timer;

	for (int i = 0; i < FLAGS_NUM; i++)
		delete flags[i];
	for (int i = 0; i < GOALS_NUM; i++)
		delete goals[i];
	for (int i = 0; i < LINES_NUM; i++)
		delete lines[i];

	delete ball;

	if (!isBodyReplaced())
		delete body; // else it will be deleted with other players.

	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		delete fullPlayers[TID_TEAMMATE][i];
	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		delete fullPlayers[TID_OPPONENT][i];

	for (int i = 0; i < HALF_PLAYERS_NUM; i++)
		delete halfPlayers[TID_TEAMMATE][i];
	for (int i = 0; i < HALF_PLAYERS_NUM; i++)
		delete halfPlayers[TID_OPPONENT][i];

	for (int i = 0; i < QUARTER_PLAYERS_NUM; i++)
		delete quarterPlayers[i];
}

void WorldModelBase::recopyPointers(const WorldModelBase &wmBase)
{
	delete timer;
	if (dynamic_cast<OnlineTimer *>(wmBase.timer))
		timer = new OnlineTimer((const OnlineTimer&)*(wmBase.timer));
	else
		timer = new VirtualTimer((const VirtualTimer&)*(wmBase.timer));

	for (int i = 0; i < FLAGS_NUM; i++)
		*flags[i] = *wmBase.flags[i];
	for (int i = 0; i < GOALS_NUM; i++)
		*goals[i] = *wmBase.goals[i];
	for (int i = 0; i < LINES_NUM; i++)
		*lines[i] = *wmBase.lines[i];

	if (isBodyReplaced())
	{ // Undoing replacement.
		fullPlayers[TID_TEAMMATE][body->getUniNum() - 1] = new Player(PLM_FULL);
	}

	*ball = *wmBase.ball;
	*body = *wmBase.body;

	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		*fullPlayers[TID_TEAMMATE][i] = *wmBase.fullPlayers[TID_TEAMMATE][i];
	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		*fullPlayers[TID_OPPONENT][i] = *wmBase.fullPlayers[TID_OPPONENT][i];

	for (int i = 0; i < HALF_PLAYERS_NUM; i++)
		*halfPlayers[TID_TEAMMATE][i] = *wmBase.halfPlayers[TID_TEAMMATE][i];
	for (int i = 0; i < HALF_PLAYERS_NUM; i++)
		*halfPlayers[TID_OPPONENT][i] = *wmBase.halfPlayers[TID_OPPONENT][i];

	for (int i = 0; i < QUARTER_PLAYERS_NUM; i++)
		*quarterPlayers[i] = *wmBase.quarterPlayers[i];

	if (wmBase.isBodyReplaced())
		replaceBodyInPlayersList(body->getUniNum());
}

float WorldModelBase::getSafetyDashPower(float dash, float stamina) const
{
	double required_stamina = ( dash > 0.0 ? dash : dash * -2.0 );
    const double safety_stamina = stamina - (serverParam["recover_dec_thr"].asFloat() * serverParam["stamina_max"].asFloat()) - 3.0;
    double available_stamina = std::max( 0.0, safety_stamina );
    double result_power = std::min( required_stamina, available_stamina );

    if ( dash < 0.0 )
        result_power *= -0.5;

    if ( std::fabs( result_power ) > std::fabs( dash ) )
        return dash;

    return result_power;
}

bool WorldModelBase::isSeeSynch() const
{
	return seeSynch;
}

void WorldModelBase::initServerParam(const SExpression &exp)
{
	int i;

	SExpAtomic *at = dynamic_cast<SExpAtomic *>(exp[0]);
	assert(at);
	assert(at->asString() == "server_param");

	for (i = 1; i < exp.size(); i++)
	{
		SExpression *se = dynamic_cast<SExpression *>(exp[i]);
		assert(se);
		SExpAtomic *at1 = dynamic_cast<SExpAtomic *>((*se)[0]);
		assert(at1);
		SExpAtomic *at2 = dynamic_cast<SExpAtomic *>((*se)[1]);
		assert(at2);
		serverParam.add(at1->asString(), at2->asString());
	}

	// now I can set default variables of objects.
	ball->setServerParamVars(serverParam);

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		fullPlayers[TID_TEAMMATE][i]->setServerParamVars(serverParam);
	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		fullPlayers[TID_OPPONENT][i]->setServerParamVars(serverParam);

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		halfPlayers[TID_TEAMMATE][i]->setServerParamVars(serverParam);
	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		halfPlayers[TID_OPPONENT][i]->setServerParamVars(serverParam);

	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		quarterPlayers[i]->setServerParamVars(serverParam);
}

void WorldModelBase::initPlayerParam(const SExpression &exp)
{
	int i;

	SExpAtomic *at = dynamic_cast<SExpAtomic *>(exp[0]);
	assert(at);
	assert(at->asString() == "player_param");

	for (i = 1; i < exp.size(); i++)
	{
		SExpression *se = dynamic_cast<SExpression *>(exp[i]);
		assert(se);
		SExpAtomic *at1 = dynamic_cast<SExpAtomic *>((*se)[0]);
		assert(at1);
		SExpAtomic *at2 = dynamic_cast<SExpAtomic *>((*se)[1]);
		assert(at2);
		playerParam.add(at1->asString(), at2->asString());
	}

	playerTypes = vector<Param>(playerParam["player_types"].asInt());
}

void WorldModelBase::initPlayerType(const SExpression &exp)
{
	int i;

	SExpAtomic *at = dynamic_cast<SExpAtomic *>(exp[0]);
	assert(at);
	assert(at->asString() == "player_type");

	const SExpression &seid = exp["id"];
	at = dynamic_cast<SExpAtomic *>(seid[1]);
	assert(at);
	int id = at->asInt();

	for (i = 2; i < exp.size(); i++)
	{
		SExpression *se = dynamic_cast<SExpression *>(exp[i]);
		assert(se);
		SExpAtomic *at1 = dynamic_cast<SExpAtomic *>((*se)[0]);
		assert(at1);
		SExpAtomic *at2 = dynamic_cast<SExpAtomic *>((*se)[1]);
		assert(at2);
		playerTypes[id].add(at1->asString(), at2->asString());
	}

	if (id == 0) // now I can set default players type.
	{
		for (i = 0; i < FULL_PLAYERS_NUM; i++)
			fullPlayers[TID_TEAMMATE][i]->setType(0, playerTypes[0]);
		for (i = 0; i < FULL_PLAYERS_NUM; i++)
			fullPlayers[TID_OPPONENT][i]->setType(0, playerTypes[0]);

		for (i = 0; i < HALF_PLAYERS_NUM; i++)
			halfPlayers[TID_TEAMMATE][i]->setType(0, playerTypes[0]);
		for (i = 0; i < HALF_PLAYERS_NUM; i++)
			halfPlayers[TID_OPPONENT][i]->setType(0, playerTypes[0]);

		for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
			quarterPlayers[i]->setType(0, playerTypes[0]);
	}
}

void WorldModelBase::see(const SExpression &exp)
{
	if (getBody().getVMQuality() == VMQ_HIGH)
	{
		resetObjects();
		parseSee(exp);
		updateObjects();

		logObjects();
	}
	else
		LOG << "LOW QUALITY: see will not be applied." << endl;

	updateSeeDeltaCyclesAfterSee();
}

void WorldModelBase::parseSee(const SExpression &exp)
{
	SExpAtomic *at;
	SExpression *se1,*se2;
	int i;

	at = dynamic_cast<SExpAtomic *>(exp[0]);
	assert(at);
	assert(at->asString() == "see");

	// See time
	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);
	seeCycle = at->asInt();
	seeTime = getCurTime();

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
		case 'f':
			parseFlag(*se1);
			break;

		case 'g':
			parseGoal(*se1);
			break;

		case 'l':
			parseLine(*se1);
			break;

		case 'b':
		case 'B':
			parseBall(*se1);
			break;

		case 'p':
		case 'P':
			parsePlayer(*se1);
			break;

		case 'F':
		case 'L':
		case 'G':
			break; // Going to trash.

		default:
			assert(0);
		}
	}
}

void WorldModelBase::parseFlag(const SExpression &exp)
{
	SExpression *se;
	int flagId;

	se = dynamic_cast<SExpression *>(exp[0]);
	assert(se);
	flagId = fieldData.getFlagId(*se);

	flags[flagsNum]->parse(flagId, exp, getCurTime());
	flagsNum++;
}

void WorldModelBase::parseGoal(const SExpression &exp)
{
	SExpression *se;
	int goalId;

	se = dynamic_cast<SExpression *>(exp[0]);
	assert(se);
	goalId = fieldData.getGoalId(*se);

	goals[goalsNum]->parse(goalId, exp, getCurTime());
	goalsNum++;
}

void WorldModelBase::parseLine(const SExpression &exp)
{
	SExpression *se;
	int lineId;

	se = dynamic_cast<SExpression *>(exp[0]);
	assert(se);
	lineId = fieldData.getLineId(*se);

	lines[linesNum]->parse(lineId, exp, getCurTime());
	linesNum++;
}

void WorldModelBase::parseBall(const SExpression &exp)
{
	ball->parse(exp, getCurTime());
}

void WorldModelBase::parsePlayer(const SExpression &exp)
{
	SExpression *se;
	SExpAtomic *at;
	string ourTeamName;
	TeamId teamId = TID_UNKNOWN;
	unsigned uniNum = 0;
	bool isGoalie = false;
	unsigned i;

	se = dynamic_cast<SExpression *>(exp[0]);
	assert(se);

	if (se->size() > 1)
	{
		at = dynamic_cast<SExpAtomic *>((*se)[1]);
		assert(at);

		ourTeamName = (string)"\"" +
			config["Agent"]["Public"]["TeamName"].asString() + "\"";

		if (at->toString() == ourTeamName)
			teamId = TID_TEAMMATE;
		else
		{
			if (opponentTeamName == "")
				opponentTeamName = at->toString();
			teamId = TID_OPPONENT;
		}
	}

	if (se->size() > 2)
	{
		at = dynamic_cast<SExpAtomic *>((*se)[2]);
		assert(at);
		uniNum = at->asInt();
	}

	if (se->size() > 3)
	{
		at = dynamic_cast<SExpAtomic *>((*se)[3]);
		assert(at);
		assert(at->toString() == "goalie");
		if (teamId == TID_OPPONENT)
			lastGoalieSeeCounter = 0;
		isGoalie = true;
	}

	if (teamId == TID_UNKNOWN) // Quarter
	{
		for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
			if (!quarterPlayers[i]->isValid() &&
				!quarterPlayers[i]->isReadyToUpdate())
				break;

		if (i < QUARTER_PLAYERS_NUM)
			quarterPlayers[i]->parse(teamId, uniNum, isGoalie, exp, getCurTime());
	}

	else if (uniNum == 0) // Half
	{
		for (i = 0; i < HALF_PLAYERS_NUM; i++)
			if (!halfPlayers[teamId][i]->isValid() &&
				!halfPlayers[teamId][i]->isReadyToUpdate())
				break;

		if (i < HALF_PLAYERS_NUM)
			halfPlayers[teamId][i]->parse(teamId, uniNum, isGoalie, exp, getCurTime());
	}

	else // Full
	{
		assert(uniNum <= FULL_PLAYERS_NUM);
		fullPlayers[teamId][uniNum - 1]->parse(teamId, uniNum, isGoalie, exp,
											   getCurTime());
	}

	// Mehrdad -- tackleRemainCycle
	if (teamId != TID_UNKNOWN and uniNum != 0)
	{
		if (fullPlayers[teamId][uniNum - 1]->isTackling())
		{
			int remainCycle = 10 - (getCurCycle() - fullPlayers[teamId][uniNum - 1]->getLastStandCycle());
			remainCycle = max(1, remainCycle);
			fullPlayers[teamId][uniNum - 1]->setTackleRemainCycle(remainCycle + 1);
			LOG << "player " << uniNum << " isTackling, tackleRemainCycle : " << remainCycle << endl;
		}
		else
		{
			fullPlayers[teamId][uniNum - 1]->setLastStandCycle(getCurCycle());
		}
	}
	// Mehrdad -- tackleRemainCycle

}

void WorldModelBase::resetObjects()
{
	unsigned i;

	flagsNum = 0;
	goalsNum = 0;
	linesNum = 0;

	// Reading to parse objects
	ball->setReadyToUpdate(false);

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		fullPlayers[TID_TEAMMATE][i]->setReadyToUpdate(false);
	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		fullPlayers[TID_OPPONENT][i]->setReadyToUpdate(false);

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		halfPlayers[TID_TEAMMATE][i]->setReadyToUpdate(false);
	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		halfPlayers[TID_OPPONENT][i]->setReadyToUpdate(false);

	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		quarterPlayers[i]->setReadyToUpdate(false);

	// Killing a few players
	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		quarterPlayers[i]->kill();

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
	{
		if (halfPlayers[TID_TEAMMATE][i]->isAlive() &&
			halfPlayers[TID_TEAMMATE][i]->canBodySee(body->getPos(),
													 body->getViewAngle(), body->getVisibleDistance()))
			halfPlayers[TID_TEAMMATE][i]->kill();

		if (halfPlayers[TID_OPPONENT][i]->isAlive() &&
			halfPlayers[TID_OPPONENT][i]->canBodySee(body->getPos(),
													 body->getViewAngle(), body->getVisibleDistance()))
			halfPlayers[TID_OPPONENT][i]->kill();
	}
}

void WorldModelBase::replaceBodyInPlayersList(unsigned bodyUniNum)
{
	delete fullPlayers[TID_TEAMMATE][bodyUniNum - 1];
	fullPlayers[TID_TEAMMATE][bodyUniNum - 1] = body;

	setBodyReplaced(true);
}

void WorldModelBase::updateObjects()
{
	unsigned i;

	for (i = 0; i < flagsNum; i++)
		flags[i]->update(fieldData);

	for (i = 0; i < goalsNum; i++)
		goals[i]->update(fieldData);

	for (i = 0; i < linesNum; i++)
		lines[i]->update(fieldData);


	LOG << "body coll = " << body->isCollisionBall() << endl;

	body->update(flags, goals, lines, flagsNum, goalsNum, linesNum);

	LOG << "body coll = " << body->isCollisionBall() << endl;

	if (ball->isReadyToUpdate())
		ball->update(*body, playMode != PM_FREE_KICK, curCycle);
	else if (ball->isValid())
		ball->virtualUpdate(*body);

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_TEAMMATE][i]->isReadyToUpdate())
			fullPlayers[TID_TEAMMATE][i]->update(*body, playMode);
		else if (i != body->getUniNum() - 1 && // this function do not virtual update body.
				 fullPlayers[TID_TEAMMATE][i]->isAlive())
			fullPlayers[TID_TEAMMATE][i]->virtualUpdate(*body);

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_OPPONENT][i]->isReadyToUpdate())
			fullPlayers[TID_OPPONENT][i]->update(*body, playMode);
		else if (fullPlayers[TID_OPPONENT][i]->isAlive())
			fullPlayers[TID_OPPONENT][i]->virtualUpdate(*body);

	// Mehrdad -- for tackleRemainCycle
	for (i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		int remain = fullPlayers[TID_OPPONENT][i]->getTackleRemainCycle();
		remain = max(0, remain - 1);
		fullPlayers[TID_OPPONENT][i]->setTackleRemainCycle(remain);
		LOG << "opponent " << i + 1 << " tackleRemainCycle : " << remain << endl;
	}

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		int remain = fullPlayers[TID_TEAMMATE][i]->getTackleRemainCycle();
		remain = max(0, remain - 1);
		fullPlayers[TID_TEAMMATE][i]->setTackleRemainCycle(remain);
		LOG << "teammate " << i + 1 << " tackleRemainCycle : " << remain << endl;
	}
	// Mehrdad -- for tackleRemainCycle

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (halfPlayers[TID_TEAMMATE][i]->isReadyToUpdate())
			halfPlayers[TID_TEAMMATE][i]->update(*body, playMode);
		else if (halfPlayers[TID_TEAMMATE][i]->isValid())
			halfPlayers[TID_TEAMMATE][i]->virtualUpdate(*body);

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (halfPlayers[TID_OPPONENT][i]->isReadyToUpdate())
			halfPlayers[TID_OPPONENT][i]->update(*body, playMode);
		else if (halfPlayers[TID_OPPONENT][i]->isValid())
			halfPlayers[TID_OPPONENT][i]->virtualUpdate(*body);

	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		if (quarterPlayers[i]->isReadyToUpdate())
			quarterPlayers[i]->update(*body, playMode);
		else if (quarterPlayers[i]->isValid())
			quarterPlayers[i]->virtualUpdate(*body);

	completeUpdateObjects();
	lastGoalieSeeCounter++;
	LOG << "lastGoalieSeeCounter = " << lastGoalieSeeCounter << " cycle " << curCycle << endl;
}

void WorldModelBase::simulateObjects(BallStatus ballStatus,
		const Body &lastBody, const Command *bodyCycleCommand)
{
	unsigned i;

	if (getPlayMode() == PM_FREE_KICK &&
		getVirtualPlayMode() ==	VPM_GOALIE_CATCH_BALL && body->isGoalie())
		ball->simulateByAction(lastBody, bodyCycleCommand, true);
	else
		ball->simulateByAction(lastBody, bodyCycleCommand);

	ball->simulateByDynamics(*body, 2, ballStatus, curTime);

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (i != body->getUniNum() - 1 && // this function do not simulate body.
			fullPlayers[TID_TEAMMATE][i]->isAlive())
			fullPlayers[TID_TEAMMATE][i]->simulateByDynamics(*body);
	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_OPPONENT][i]->isAlive())
			fullPlayers[TID_OPPONENT][i]->simulateByDynamics(*body);

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (halfPlayers[TID_TEAMMATE][i]->isValid())
			halfPlayers[TID_TEAMMATE][i]->simulateByDynamics(*body);
	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (halfPlayers[TID_OPPONENT][i]->isValid())
			halfPlayers[TID_OPPONENT][i]->simulateByDynamics(*body);

	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		if (quarterPlayers[i]->isValid())
			quarterPlayers[i]->simulateByDynamics(*body);
}

void WorldModelBase::completeUpdateObjects()
{
	int i, j;
	Player *playerPtr;

	// Removing same Halves
	for (i = 0; i < HALF_PLAYERS_NUM; i++)
	{
		// Teammates
		if (halfPlayers[TID_TEAMMATE][i]->isValid() &&
			halfPlayers[TID_TEAMMATE][i]->getSeeTime() != getCurTime())
		{

			playerPtr = NULL;
			for (j = 0; j < HALF_PLAYERS_NUM; j++)
				if (halfPlayers[TID_TEAMMATE][j]->isValid() &&
					halfPlayers[TID_TEAMMATE][j]->getSeeTime() == getCurTime() &&
					halfPlayers[TID_TEAMMATE][j]->getDistance(*halfPlayers[TID_TEAMMATE][i]) <=
					halfPlayers[TID_TEAMMATE][j]->getPosDeviation() + halfPlayers[TID_TEAMMATE][i]->getPosDeviation() + 1) // One meter for sure
				{
					if (!playerPtr)
						playerPtr = halfPlayers[TID_TEAMMATE][j];
					else
					{
						playerPtr = NULL;
						break;
					}
				}

			if (playerPtr)
			{
				for (j = 0; j < HALF_PLAYERS_NUM; j++)
					if (i != j && halfPlayers[TID_TEAMMATE][j]->isValid() &&
						halfPlayers[TID_TEAMMATE][j]->getSeeTime() != getCurTime() &&
						halfPlayers[TID_TEAMMATE][j]->getDistance(*playerPtr) <=
						halfPlayers[TID_TEAMMATE][j]->getPosDeviation() + playerPtr->getPosDeviation() + 1) // One meter for sure
				{
					j = -1;
					break;
				}

				if (j != -1)
					halfPlayers[TID_TEAMMATE][i]->kill();
			}
		}

		// Opponents
		if (halfPlayers[TID_OPPONENT][i]->isValid() &&
			halfPlayers[TID_OPPONENT][i]->getSeeTime() != getCurTime())
		{

			playerPtr = NULL;
			for (j = 0; j < HALF_PLAYERS_NUM; j++)
				if (halfPlayers[TID_OPPONENT][j]->isValid() &&
					halfPlayers[TID_OPPONENT][j]->getSeeTime() == getCurTime() &&
					halfPlayers[TID_OPPONENT][j]->getDistance(*halfPlayers[TID_OPPONENT][i]) <=
					halfPlayers[TID_OPPONENT][j]->getPosDeviation() + halfPlayers[TID_OPPONENT][i]->getPosDeviation() + 1) // One meter for sure
				{
					if (!playerPtr)
						playerPtr = halfPlayers[TID_OPPONENT][j];
					else
					{
						playerPtr = NULL;
						break;
					}
				}

			if (playerPtr)
			{
				for (j = 0; j < HALF_PLAYERS_NUM; j++)
					if (i != j && halfPlayers[TID_OPPONENT][j]->isValid() &&
						halfPlayers[TID_OPPONENT][j]->getSeeTime() != getCurTime() &&
						halfPlayers[TID_OPPONENT][j]->getDistance(*playerPtr) <=
						halfPlayers[TID_OPPONENT][j]->getPosDeviation() + playerPtr->getPosDeviation() + 1) // One meter for sure
				{
					j = -1;
					break;
				}

				if (j != -1)
					halfPlayers[TID_OPPONENT][i]->kill();
			}
		}

	}

	// Changing Quarters to Halves
	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		if (quarterPlayers[i]->isValid())
		{
			playerPtr = NULL;
			for (j = 0; j < HALF_PLAYERS_NUM; j++)
			{
				// Teammates
				if (halfPlayers[TID_TEAMMATE][j]->isValid() &&
					halfPlayers[TID_TEAMMATE][j]->getSeeTime() != getCurTime() &&
					halfPlayers[TID_TEAMMATE][j]->getDistance(*quarterPlayers[i]) <=
					halfPlayers[TID_TEAMMATE][j]->getPosDeviation() + quarterPlayers[i]->getPosDeviation() + 1) // One meter for sure
				{
					if (playerPtr == NULL)
						playerPtr = halfPlayers[TID_TEAMMATE][j];
					else
					{
						playerPtr = NULL;
						break;
					}
				}

				// Opponents
				if (halfPlayers[TID_OPPONENT][j]->isValid() &&
					halfPlayers[TID_OPPONENT][j]->getSeeTime() != getCurTime() &&
					halfPlayers[TID_OPPONENT][j]->getDistance(*quarterPlayers[i]) <=
					halfPlayers[TID_OPPONENT][j]->getPosDeviation() + quarterPlayers[i]->getPosDeviation() + 1) // One meter for sure
				{
					if (playerPtr == NULL)
						playerPtr = halfPlayers[TID_OPPONENT][j];
					else
					{
						playerPtr = NULL;
						break;
					}
				}
			}

			if (playerPtr != NULL)
			{
				for (j = 0; j < QUARTER_PLAYERS_NUM; j++)
					if (i != j && quarterPlayers[j]->isValid() &&
						quarterPlayers[j]->getDistance(*playerPtr) <=
						quarterPlayers[j]->getPosDeviation() + playerPtr->getPosDeviation() + 1) // One meter for sure
					{
						j = -1;
						break;
					}

				if (j != -1)
				{
					playerPtr->setByAnotherPlayer(*quarterPlayers[i], *body);
					quarterPlayers[i]->kill();
				}
			}
		}

	// Changing Quarters to Fulls
	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		if (quarterPlayers[i]->isValid())
		{
			playerPtr = NULL;
			for (j = 0; j < FULL_PLAYERS_NUM; j++)
			{
				// Teammates
				if (fullPlayers[TID_TEAMMATE][j]->isAlive() &&
					!fullPlayers[TID_TEAMMATE][j]->isBody() &&
					fullPlayers[TID_TEAMMATE][j]->getSeeTime() != getCurTime() &&
					fullPlayers[TID_TEAMMATE][j]->getDistance(*quarterPlayers[i]) <=
					fullPlayers[TID_TEAMMATE][j]->getPosDeviation() + quarterPlayers[i]->getPosDeviation() + 1) // One meter for sure
				{
					if (playerPtr == NULL)
						playerPtr = fullPlayers[TID_TEAMMATE][j];
					else
					{
						playerPtr = NULL;
						break;
					}
				}

				// Opponents
				if (fullPlayers[TID_OPPONENT][j]->isAlive() &&
					fullPlayers[TID_OPPONENT][j]->getSeeTime() != getCurTime() &&
					fullPlayers[TID_OPPONENT][j]->getDistance(*quarterPlayers[i]) <=
					fullPlayers[TID_OPPONENT][j]->getPosDeviation() + quarterPlayers[i]->getPosDeviation() + 1) // One meter for sure
				{
					if (playerPtr == NULL)
						playerPtr = fullPlayers[TID_OPPONENT][j];
					else
					{
						playerPtr = NULL;
						break;
					}
				}
			}

			if (playerPtr != NULL)
			{
				for (j = 0; j < QUARTER_PLAYERS_NUM; j++)
					if (i != j && quarterPlayers[j]->isValid() &&
						quarterPlayers[j]->getDistance(*playerPtr) <=
						quarterPlayers[j]->getPosDeviation() + playerPtr->getPosDeviation() + 1) // One meter for sure
					{
						j = -1;
						break;
					}

				if (j != -1)
				{
					playerPtr->setByAnotherPlayer(*quarterPlayers[i], *body);
					quarterPlayers[i]->kill();
				}
			}
		}

	// Changing Halves to Fulls
	for (i = 0; i < HALF_PLAYERS_NUM; i++)
	{
		// Teammates
		if (halfPlayers[TID_TEAMMATE][i]->isValid() && halfPlayers[TID_TEAMMATE][i]->getSimCounter() == 0)
		{
			playerPtr = NULL;
			for (j = 0; j < FULL_PLAYERS_NUM; j++)
				if (fullPlayers[TID_TEAMMATE][j]->isAlive() &&
					!fullPlayers[TID_TEAMMATE][j]->isBody() &&
					fullPlayers[TID_TEAMMATE][j]->getSeeTime() != getCurTime() &&
					fullPlayers[TID_TEAMMATE][j]->getDistance(*halfPlayers[TID_TEAMMATE][i]) <=
					fullPlayers[TID_TEAMMATE][j]->getPosDeviation() + halfPlayers[TID_TEAMMATE][i]->getPosDeviation() + 1) // One meter for sure
				{
					if (playerPtr == NULL)
						playerPtr = fullPlayers[TID_TEAMMATE][j];
					else
					{
						playerPtr = NULL;
						break;
					}
				}

			if (playerPtr != NULL)
			{
				for (j = 0; j < HALF_PLAYERS_NUM; j++)
					if (i != j && halfPlayers[TID_TEAMMATE][j]->isValid() &&
							halfPlayers[TID_TEAMMATE][j]->getDistance(*playerPtr) <=
							halfPlayers[TID_TEAMMATE][j]->getPosDeviation() + playerPtr->getPosDeviation() + 1) // One meter for sure
					{
						j = -1;
						break;
					}

				if (j != -1)
				{
					playerPtr->setByAnotherPlayer(*halfPlayers[TID_TEAMMATE][i], *body);
					halfPlayers[TID_TEAMMATE][i]->kill();
				}
			}
		}

		// Opponents
		if (halfPlayers[TID_OPPONENT][i]->isValid() && halfPlayers[TID_OPPONENT][i]->getSimCounter() == 0)
		{
			playerPtr = NULL;
			for (j = 0; j < FULL_PLAYERS_NUM; j++)
			{
				if (fullPlayers[TID_OPPONENT][j]->isAlive() &&
					fullPlayers[TID_OPPONENT][j]->getSeeTime() != getCurTime() &&
					fullPlayers[TID_OPPONENT][j]->getDistance(*halfPlayers[TID_OPPONENT][i]) <=
					fullPlayers[TID_OPPONENT][j]->getPosDeviation() + halfPlayers[TID_OPPONENT][i]->getPosDeviation() + 1) // One meter for sure
				{
					if (playerPtr == NULL)
						playerPtr = fullPlayers[TID_OPPONENT][j];
					else
					{
						playerPtr = NULL;
						break;
					}
				}
			}

			if (playerPtr != NULL)
			{
				for (j = 0; j < HALF_PLAYERS_NUM; j++)
					if (i != j && halfPlayers[TID_OPPONENT][j]->isValid() &&
						halfPlayers[TID_OPPONENT][j]->getDistance(*playerPtr) <=
						halfPlayers[TID_OPPONENT][j]->getPosDeviation() + playerPtr->getPosDeviation() + 1) // One meter for sure
					{
						j = -1;
						break;
					}

				if (j != -1)
				{
					playerPtr->setByAnotherPlayer(*halfPlayers[TID_OPPONENT][i], *body);
					halfPlayers[TID_OPPONENT][i]->kill();
				}
			}
		}

	}

	// Changes in sense area
	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		if (quarterPlayers[i]->isValid() && quarterPlayers[i]->getAbsVec().getMagnitude() < 3)
		{
			playerPtr = NULL;
			for (j = 0; j < FULL_PLAYERS_NUM; j++)
			{
				// Teammates
				if (fullPlayers[TID_TEAMMATE][j]->isAlive() &&
					!fullPlayers[TID_TEAMMATE][j]->isBody() &&
					fullPlayers[TID_TEAMMATE][j]->getSeeTime() != getCurTime() &&
					fullPlayers[TID_TEAMMATE][j]->getAbsVec().getMagnitude() <= 3.5 &&
					fullPlayers[TID_TEAMMATE][j]->getDistance(*quarterPlayers[i]) <=
					fullPlayers[TID_TEAMMATE][j]->getPosDeviation() + quarterPlayers[i]->getPosDeviation() + 0.5) // Half meter for sure
				{
					if (playerPtr == NULL ||
						(fullPlayers[TID_TEAMMATE][j]->getDistance(*quarterPlayers[i]) <
						 playerPtr->getDistance(*quarterPlayers[i])))
						playerPtr = fullPlayers[TID_TEAMMATE][j];
				}

				// Opponents
				if (fullPlayers[TID_OPPONENT][j]->isAlive() &&
					fullPlayers[TID_OPPONENT][j]->getSeeTime() != getCurTime() &&
					fullPlayers[TID_OPPONENT][j]->getAbsVec().getMagnitude() <= 3.5 &&
					fullPlayers[TID_OPPONENT][j]->getDistance(*quarterPlayers[i]) <=
					fullPlayers[TID_OPPONENT][j]->getPosDeviation() + quarterPlayers[i]->getPosDeviation() + 0.5) // Half meter for sure
				{
					if (playerPtr == NULL ||
						(fullPlayers[TID_OPPONENT][j]->getDistance(*quarterPlayers[i]) <
						 playerPtr->getDistance(*quarterPlayers[i])))
						playerPtr = fullPlayers[TID_OPPONENT][j];
				}
			}

			if (playerPtr != NULL)
			{
				playerPtr->setByAnotherPlayer(*quarterPlayers[i], *body);
				quarterPlayers[i]->kill();
			}
		}

	// Players change place
	for (i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		if (fullPlayers[TID_TEAMMATE][i]->isAlive() &&
			!fullPlayers[TID_TEAMMATE][i]->isBody() &&
			fullPlayers[TID_TEAMMATE][i]->getSeeTime() != getCurTime())
		{
			putOutOfVisibleDistance(*fullPlayers[TID_TEAMMATE][i]);

			if (abs(fullPlayers[TID_TEAMMATE][i]->getHeadVec().getDirection()) <
				body->getViewAngle() - CHANGE_PLACE_SURE_ANGLE &&
				fullPlayers[TID_TEAMMATE][i]->getAbsVec().getMagnitude() <
				UNUM_FAR_LENGTH - CHANGE_PLACE_SURE_DIST)
				putOutOfVisibleAngle(*fullPlayers[TID_TEAMMATE][i]);

			changePosDevBySee(*fullPlayers[TID_TEAMMATE][i]);
		}

		if (fullPlayers[TID_OPPONENT][i]->isAlive() &&
			fullPlayers[TID_OPPONENT][i]->getSeeTime() != getCurTime())
		{
			putOutOfVisibleDistance(*fullPlayers[TID_OPPONENT][i]);

			if (abs(fullPlayers[TID_OPPONENT][i]->getHeadVec().getDirection()) <
				body->getViewAngle() - CHANGE_PLACE_SURE_ANGLE &&
				fullPlayers[TID_OPPONENT][i]->getAbsVec().getMagnitude() <
				UNUM_FAR_LENGTH - CHANGE_PLACE_SURE_DIST)
				putOutOfVisibleAngle(*fullPlayers[TID_OPPONENT][i]);

			changePosDevBySee(*fullPlayers[TID_OPPONENT][i]);
		}
	}

	for (i = 0; i <= HALF_PLAYERS_NUM; i++)
	{
		if (halfPlayers[TID_TEAMMATE][i]->isValid() &&
			halfPlayers[TID_TEAMMATE][i]->getSeeTime() != getCurTime())
		{
			putOutOfVisibleDistance(*halfPlayers[TID_TEAMMATE][i]);

			if (abs(halfPlayers[TID_TEAMMATE][i]->getHeadVec().getDirection()) <
				body->getViewAngle() - CHANGE_PLACE_SURE_ANGLE &&
				halfPlayers[TID_TEAMMATE][i]->getAbsVec().getMagnitude() <
				TEAM_FAR_LENGTH - CHANGE_PLACE_SURE_DIST)
				putOutOfVisibleAngle(*halfPlayers[TID_TEAMMATE][i]);

			changePosDevBySee(*halfPlayers[TID_TEAMMATE][i]);
		}

		if (halfPlayers[TID_OPPONENT][i]->isValid() &&
			halfPlayers[TID_OPPONENT][i]->getSeeTime() != getCurTime())
		{
			putOutOfVisibleDistance(*halfPlayers[TID_OPPONENT][i]);

			if (abs(halfPlayers[TID_OPPONENT][i]->getHeadVec().getDirection()) <
				body->getViewAngle() - CHANGE_PLACE_SURE_ANGLE &&
				halfPlayers[TID_OPPONENT][i]->getAbsVec().getMagnitude() <
				TEAM_FAR_LENGTH - CHANGE_PLACE_SURE_DIST)
				putOutOfVisibleAngle(*halfPlayers[TID_OPPONENT][i]);

			changePosDevBySee(*halfPlayers[TID_OPPONENT][i]);
		}
	}

	// Ball change place
	if (ball->isValid() && ball->getSeeTime() != getCurTime())
	{
		putOutOfVisibleDistance(*ball);
		changePosDevBySee(*ball);
	}

}

void WorldModelBase::putOutOfVisibleAngle(Object &object)
{
	float rotateAngle;

//	LOG << "obj.angle: " << object.getHeadVec().getDirection() << endl;

	if (object.getHeadVec().getDirection() > 0)
		rotateAngle = body->getViewAngle() -
			object.getHeadVec().getDirection() +
			CHANGE_PLACE_SURE_ANGLE;
	else
		rotateAngle = -body->getViewAngle() -
			object.getHeadVec().getDirection() -
			CHANGE_PLACE_SURE_ANGLE;

	object.setAbsVec().rotate(rotateAngle);
	object.setBodyVec().rotate(rotateAngle);
	object.setHeadVec().rotate(rotateAngle);
	object.setPos() = body->getPos() + object.getAbsVec();

	if (dynamic_cast<Ball *>(&object))
		LOG << "Complete updates: ball putted out of visible angle." << endl;
	if (Player *player = dynamic_cast<Player *>(&object))
		LOG << "Complete updates: player " << player->getTeamId() << " "
			<< player->getUniNum() << " putted out of visible angle." << endl;
}

void WorldModelBase::putOutOfVisibleDistance(Object &object)
{
	if (object.getAbsVec().getMagnitude() < body->getVisibleDistance())
	{
		object.setAbsVec().setAsPolar(body->getVisibleDistance() +
									  PUT_OUT_VIS_SURE_DIST, object.getAbsVec().getDirection());
		object.setBodyVec().setAsPolar(body->getVisibleDistance() +
									   PUT_OUT_VIS_SURE_DIST, object.getBodyVec().getDirection());
		object.setHeadVec().setAsPolar(body->getVisibleDistance() +
									   PUT_OUT_VIS_SURE_DIST, object.getHeadVec().getDirection());

		object.setPos() = body->getPos() + object.getAbsVec();

		if (dynamic_cast<Ball *>(&object))
			LOG << "Complete updates: ball putted out of sense area." << endl;
		if (Player *player = dynamic_cast<Player *>(&object))
			LOG << "Complete updates: player " << player->getTeamId() << " "
				<< player->getUniNum() << " putted out of sense area." << endl;
	}
}

void WorldModelBase::changePosDevBySee(Object &object)
{
	const Player *player = dynamic_cast<const Player *>(&object);
	if (player && player->isGoalie() &&
		player->getTeamId() == TID_OPPONENT)
	{
		if (object.getPosDeviation() > object.getAbsVec().getMagnitude() -
				body->getVisibleDistance() - 1.5 + PUT_OUT_VIS_SURE_DIST)
			object.setPosDeviation(object.getAbsVec().getMagnitude() -
					body->getVisibleDistance() - 1.5 + PUT_OUT_VIS_SURE_DIST);
	}

	else if (object.getPosDeviation() > object.getAbsVec().getMagnitude() -
			body->getVisibleDistance() + PUT_OUT_VIS_SURE_DIST)
		object.setPosDeviation(object.getAbsVec().getMagnitude() -
				body->getVisibleDistance() + PUT_OUT_VIS_SURE_DIST);
}

void WorldModelBase::logObjects()
{
	unsigned i;

	LOG << "Body"
		<< " Pos" << body->getPos()
		<< " Vel" << body->getVel()
		<< " Ang" << Point(body->getBodyDir(), body->getHeadDir())
		<< " Poi" << body->getPointingDir() << endl;

	LOG << "Ball"
		<< " Pos" << ball->getPos()
		<< " Vel" << ball->getVel()
		<< " Ang" << Point(ball->getBodyVec().getDirection(),
						   ball->getHeadVec().getDirection())
		<< " Len" << ball->getAbsVec().getMagnitude()
		<< " PoD" << ball->getPosDeviation() << endl;

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_TEAMMATE][i]->isValid() &&
			!fullPlayers[TID_TEAMMATE][i]->isBody())
			LOG << "OurF" << i+1
				<< " Pos" << fullPlayers[TID_TEAMMATE][i]->getPos()
				<< " Vel" << fullPlayers[TID_TEAMMATE][i]->getVel()
				<< " Ang" << Point(fullPlayers[TID_TEAMMATE][i]->getBodyDir(),
								   fullPlayers[TID_TEAMMATE][i]->getHeadDir())
				<< " PoD" << fullPlayers[TID_TEAMMATE][i]->getPosDeviation()
				<< " Poi" << fullPlayers[TID_TEAMMATE][i]->getPointingDir()
				<< " Sta" << fullPlayers[TID_TEAMMATE][i]->getStamina() << endl;

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_OPPONENT][i]->isValid())
			LOG << "OppF" << i+1
				<< " Pos" << fullPlayers[TID_OPPONENT][i]->getPos()
				<< " Vel" << fullPlayers[TID_OPPONENT][i]->getVel()
				<< " Ang" << Point(fullPlayers[TID_OPPONENT][i]->getBodyDir(),
								   fullPlayers[TID_OPPONENT][i]->getHeadDir())
				<< " PoD" << fullPlayers[TID_OPPONENT][i]->getPosDeviation()
				<< " Poi" << fullPlayers[TID_OPPONENT][i]->getPointingDir()
				<< " Sta" << fullPlayers[TID_OPPONENT][i]->getStamina()
				<< " Goa" << (fullPlayers[TID_OPPONENT][i]->isGoalie() ? "True" : "False" )
				<< " Sim" << fullPlayers[TID_OPPONENT][i]->getSimCounter() << endl;

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (halfPlayers[TID_TEAMMATE][i]->isValid())
			LOG << "OurH" << i+1
				<< " Pos" << halfPlayers[TID_TEAMMATE][i]->getPos()
				<< " Vel" << halfPlayers[TID_TEAMMATE][i]->getVel()
				<< " Ang" << Point(halfPlayers[TID_TEAMMATE][i]->getBodyDir(),
								   halfPlayers[TID_TEAMMATE][i]->getHeadDir())
				<< " PoD" << halfPlayers[TID_TEAMMATE][i]->getPosDeviation()
				<< " Poi" << halfPlayers[TID_TEAMMATE][i]->getPointingDir()
				<< " Sta" << halfPlayers[TID_TEAMMATE][i]->getStamina() << endl;

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (halfPlayers[TID_OPPONENT][i]->isValid())
			LOG << "OppH" << i+1
				<< " Pos" << halfPlayers[TID_OPPONENT][i]->getPos()
				<< " Vel" << halfPlayers[TID_OPPONENT][i]->getVel()
				<< " Ang" << Point(halfPlayers[TID_OPPONENT][i]->getBodyDir(),
								   halfPlayers[TID_OPPONENT][i]->getHeadDir())
				<< " PoD" << halfPlayers[TID_OPPONENT][i]->getPosDeviation()
				<< " Poi" << halfPlayers[TID_OPPONENT][i]->getPointingDir()
				<< " Sta" << halfPlayers[TID_OPPONENT][i]->getStamina()
				<< " Sim" << halfPlayers[TID_OPPONENT][i]->getSimCounter() << endl;

	for (i = 0; i < QUARTER_PLAYERS_NUM; i++)
		if (quarterPlayers[i]->isValid())
			LOG << "Quar" << i+1
				<< " Pos" << quarterPlayers[i]->getPos()
				<< " Vel" << quarterPlayers[i]->getVel()
				<< " Ang" << Point(quarterPlayers[i]->getBodyDir(),
								   quarterPlayers[i]->getHeadDir())
				<< " PoD" << quarterPlayers[i]->getPosDeviation()
				<< " Poi" << quarterPlayers[i]->getPointingDir()
				<< " Sta" << quarterPlayers[i]->getStamina() << endl;

}

void WorldModelBase::initViewPartDirs()
{
	for (unsigned i = 0; i < VIEW_PARTS_NUM; i++)
		viewPartDirs[i] = i * (360 / VIEW_PARTS_NUM);
}

void WorldModelBase::resetSeeDeltaCycles()
{
	for (unsigned i = 0; i < VIEW_PARTS_NUM; i++)
		viewPartCycles[i] = MAX_SEE_DELTA_CYCLE;
}

void WorldModelBase::updateSeeDeltaCyclesAfterSee()
{
	for (unsigned i = 0; i < VIEW_PARTS_NUM; i++)
	{
		//LOG << "Updating -> " << i << " with (" << body->getHeadDir() << ", " << body->getViewAngle() << ")" << endl;
		//if (getSeeViewPartValue(i, body->getHeadDir(), body->getViewAngle()) >= 0.75)
		//LOG << "Angle = " << normalizeAngle(viewPartDirs[i]) << endl;
		//LOG << "UpdatingSeeDeltaCycle -> " << abs(getDeltaAngle(viewPartDirs[i], body->getHeadDir())) << " " << body->getViewAngle()
		//   << " " << body->getViewAngle() - VIEW_PART_SIZE / 2.f << endl;
		if (fabs(getDeltaAngle(viewPartDirs[i], body->getHeadDir())) < body->getViewAngle() - VIEW_PART_SIZE / 2.f)
			viewPartCycles[i] = 0;
	}
}

void WorldModelBase::updateSeeDeltaCyclesAfterSenseBody()
{
	for (unsigned i = 0; i < VIEW_PARTS_NUM; i++)
		if (viewPartCycles[i] < MAX_SEE_DELTA_CYCLE)
			viewPartCycles[i]++;
}

float WorldModelBase::getSeeDeltaCycle(float dir) const
{
	return viewPartCycles[(unsigned)floor(Degree::absoluteAngle(dir) * .2f)];
}

float WorldModelBase::getRangeSeeDeltaCycle(float dir, float viewSize) const
{
	float result = 0.f;
	unsigned count = 0;

	float direction = viewPartDirs[(unsigned)floor(Degree::absoluteAngle(dir) * .2f)];
	float halfDir = getDeltaAngle(direction, dir) + viewSize;
	while (1)
	{
		count++;
		if(halfDir >= 5.f)
		{
			//LOG << "See Delta Cycle (" << normalizeAngle(direction) << ") = " << getSeeDeltaCycle(absoluteAngle(direction)) << endl;
			result += getSeeDeltaCycle(absoluteAngle(direction));
		}
		else
		{
			//LOG << "See Delta Cycle (" << normalizeAngle(direction) << ") = " << getSeeDeltaCycle(absoluteAngle(direction)) << endl;
			result += getSeeDeltaCycle(normalizeAngle(direction)) * (halfDir / 5.f);
			break;
		}
		halfDir -= 5.f;
		direction += 5.f;
	}

	direction = viewPartDirs[(unsigned)floor(Degree::absoluteAngle(dir) * .2f)];
	halfDir = getDeltaAngle(direction, dir) - viewSize;
	while (1)
	{
		halfDir += 5.f;
		direction -= 5.f;
		count++;
		if(halfDir <= -5.f)
		{
			//LOG << "See Delta Cycle (" << normalizeAngle(direction) << ") = " << getSeeDeltaCycle(absoluteAngle(direction)) << endl;
			result += getSeeDeltaCycle(absoluteAngle(direction));
		}
		else
		{
			//LOG << "See Delta Cycle (" << normalizeAngle(direction) << ") = " << getSeeDeltaCycle(absoluteAngle(direction)) << endl;
			result += getSeeDeltaCycle(absoluteAngle(direction)) * (halfDir / 5.f);
			break;
		}
	}

	/*for (float direction = dir - viewSize;direction <= dir + viewSize;direction += 5.f)
	{
		count++;
		LOG << "See Delta Cycle (" << absoluteAngle(direction) << ") = " << getSeeDeltaCycle(absoluteAngle(direction)) << endl;
		result += getSeeDeltaCycle(absoluteAngle(direction));
	}*/
	//LOG << "RESULT = " << result << " COUNT = " << count << " RETURN = " << result / count << endl;
	return result / count;
}

/*float WorldModelBase::getSeeViewPartValue(unsigned viewPartNum, float headDir, float viewAngle) const
{
	float deltaAngle;

	deltaAngle = fabs(getDeltaAngle(viewPartDirs[viewPartNum], headDir));

	LOG << "Cur Angle = " << viewPartDirs[viewPartNum] << " Delta Angle = " << deltaAngle << " EXTRA = " << EXTRA_VIEW_SIZE + 5 << endl;

//	Edited By Pooria
//	deltaAngle += VIEW_PART_SIZE / 2.0 - viewAngle;
//	if (deltaAngle <= EXTRA_VIEW_SIZE + 5) // 5 Degrees for sure
//		return reRate(deltaAngle, 0, EXTRA_VIEW_SIZE + 5, 0.75, 1, RRM_REVERSE);

	if (deltaAngle <= viewAngle / 2.f)
		return reRate(deltaAngle, 0, EXTRA_VIEW_SIZE + 5, 0.75, 1, RRM_REVERSE);

	return reRate(deltaAngle, EXTRA_VIEW_SIZE + 5, 45, 0, 0.75, RRM_REVERSE);
}*/

void WorldModelBase::updatePlayModeRemainCycleAfterSenseBody()
{
	if (playModeRemainCycle > 0)
		playModeRemainCycle--;
}

void WorldModelBase::parseChangePlayerType(const SExpression &exp)
{
	SExpAtomic *at;
	unsigned playerNum;
	unsigned playerType;

	if (exp.size() == 3) // teammate change player type
	{
		at = dynamic_cast<SExpAtomic *>(exp[1]);
		assert(at);
		playerNum = at->asInt();
		at = dynamic_cast<SExpAtomic *>(exp[2]);
		assert(at);
		playerType = at->asInt();

		assert(playerNum > 0 && playerNum <= FULL_PLAYERS_NUM);
		assert((int)playerType < playerParam["player_types"].asInt());
		fullPlayers[TID_TEAMMATE][playerNum - 1]->setType(playerType,
														  playerTypes[playerType]);
		fullPlayers[TID_TEAMMATE][playerNum - 1]->setCard(CM_NONE);
	}
	else // opponent change player type
	{
//		assert(exp.size() == 3);
	}
}

void WorldModelBase::parseOk(const SExpression &exp)
{
	if (exp.size() == 2)
	{
		SExpAtomic *at;
		at = dynamic_cast<SExpAtomic *>(exp[1]);
		assert(at);
		if (at->asString() == "synch_see")
		{
			LOG << "SeeSynch system started" << endl;
			seeSynch = true;
			body->setSynch();
		}
	}
	else
		LOG << "Unkown ok header" << endl;
}

// Getting functions
bool WorldModelBase::isBodyReplaced() const
{
	return bodyReplacedFlag;
}

unsigned WorldModelBase::getFlagsNum() const
{
	return flagsNum;
}

unsigned WorldModelBase::getGoalsNum() const
{
	return goalsNum;
}

unsigned WorldModelBase::getLinesNum() const
{
	return linesNum;
}

unsigned WorldModelBase::getSeeCycle() const
{
	return seeCycle;
}

unsigned WorldModelBase::getCurCycle() const
{
	return curCycle;
}

unsigned WorldModelBase::getSeeTime() const
{
	return seeTime;
}

unsigned WorldModelBase::getCurTime() const
{
	return curTime;
}
unsigned WorldModelBase::getRefereeHearTime() const
{
	return refereeHearTime;
}

Side WorldModelBase::getFieldSide() const
{
	return fieldSide;
}

PlayMode WorldModelBase::getPlayMode() const
{
	return playMode;
}

VirtualPlayMode WorldModelBase::getVirtualPlayMode() const
{
	return virtualPlayMode;
}

float WorldModelBase::getViewPartDir(unsigned num) const
{
	return viewPartDirs[num];
}

unsigned WorldModelBase::getViewPartCycle(unsigned num) const
{
	return viewPartCycles[num];
}

const Timer &WorldModelBase::getTimer() const
{
	return (const Timer &)*timer;
}

unsigned WorldModelBase::getPlayModeRemainCycle() const
{
	return playModeRemainCycle;
}

const Param &WorldModelBase::getServerParam() const
{
	return (const Param &)serverParam;
}

const Param &WorldModelBase::getPlayerParam() const
{
	return (const Param &)playerParam;
}

const Param &WorldModelBase::getPlayerType(unsigned num) const
{
	return (const Param &)playerTypes[num];
}

const Ball &WorldModelBase::getBall() const
{
	return (const Ball &)*ball;
}

const Body &WorldModelBase::getBody() const
{
	return (const Body &)*body;
}

const Player &WorldModelBase::getFullPlayer(TeamId teamId, unsigned num) const
{
	return (const Player &)*fullPlayers[teamId][num];
}

const Player &WorldModelBase::getHalfPlayer(TeamId teamId, unsigned num) const
{
	return (const Player &)*halfPlayers[teamId][num];
}

const Player &WorldModelBase::getQuarterPlayer(unsigned num) const
{
	return (const Player &)*quarterPlayers[num];
}

unsigned WorldModelBase::getPlayOnPastCycles() const
{
	return curTime - playOnStartCycle;
}

unsigned WorldModelBase::getLastGoalieSeeCounter() const
{
	return lastGoalieSeeCounter;
}

const string WorldModelBase::getOpponentTeamName() const
{
	return opponentTeamName;
}

// Setting functions
void WorldModelBase::setBodyReplaced(bool bodyReplacedFlagArg)
{
	bodyReplacedFlag = bodyReplacedFlagArg;
}

void WorldModelBase::setFlagsNum(unsigned flagsNumArg)
{
	flagsNum = flagsNumArg;
}

void WorldModelBase::setGoalsNum(unsigned goalsNumArg)
{
	goalsNum = goalsNumArg;
}

void WorldModelBase::setLinesNum(unsigned linesNumArg)
{
	linesNum = linesNumArg;
}

void WorldModelBase::setSeeCycle(unsigned seeCycleArg)
{
	seeCycle = seeCycleArg;
}

void WorldModelBase::setCurCycle(unsigned curCycleArg)
{
	curCycle = curCycleArg;
}

void WorldModelBase::setCurTime(unsigned curTimeArg)
{
	curTime = curTimeArg;
}

void WorldModelBase::setFieldSide(Side fieldSideArg)
{
	fieldSide = fieldSideArg;

	if (fieldSide == SI_LEFT)
		fieldData.setRightSideRate(1);
	else
		fieldData.setRightSideRate(-1);

	if((getPlayMode() == PM_PENALTY_SETUP || getPlayMode() == PM_PENALTY_SETUP_OPP || getPlayMode() == PM_PENALTY_READY || getPlayMode() == PM_PENALTY_READY_OPP) && fieldSide == SI_LEFT)
		fieldData.setRightSideRate(1);
	if((getPlayMode() == PM_PENALTY_SETUP || getPlayMode() == PM_PENALTY_SETUP_OPP || getPlayMode() == PM_PENALTY_READY || getPlayMode() == PM_PENALTY_READY_OPP) && fieldSide == SI_RIGHT)
		fieldData.setRightSideRate(-1);
}

FieldData &WorldModelBase::setFieldData()
{
	return fieldData;
}

void WorldModelBase::setVirtualPlayMode(VirtualPlayMode virtualPlayModeArg)
{
	virtualPlayMode = virtualPlayModeArg;
}

Timer &WorldModelBase::setTimer()
{
	return (Timer &)*timer;
}

Ball &WorldModelBase::setBall()
{
	return (Ball &)*ball;
}

Body &WorldModelBase::setBody()
{
	return *body;
}

Player &WorldModelBase::setFullPlayer(TeamId teamId, unsigned num)
{
	return (Player &)*fullPlayers[teamId][num];
}

Player &WorldModelBase::setHalfPlayer(TeamId teamId, unsigned num)
{
	return (Player &)*halfPlayers[teamId][num];
}

Player &WorldModelBase::setQuarterPlayer(unsigned num)
{
	return (Player &)*quarterPlayers[num];
}

