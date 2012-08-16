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
 *  This file is created by: MohammadReza Montazeri Shatoori
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <FastIC.h>

#include <Basics.h>
#include <Defines.h>
#include <Command.h>
#include <WorldModel.h>
#include <Degree.h>
#include <Rectangle.h>
#include <Logger.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#define MAXDASH 100

//#define FASTLOG

using namespace std;
using namespace Degree;

bool FastIC::isSet = false;
FastType FastIC::types[TYPESCOUNT];

int TurnModel::getCyclesToTurn( float velocity , float angleChange )
{
	int velocityBlock = min ( (int)( velocity / ( maxVelocity / (float)TURN_VELCOUNT ) ) , TURN_VELCOUNT - 1);
	int angleBlock = min ( (int)( angleChange / ( maxAngleChange / (float)TURN_ANGLECOUNT ) ) , TURN_ANGLECOUNT - 1);
	return cyclesToTurn[velocityBlock][angleBlock];
}


FastPlayer::FastPlayer() :
	pos(),
	vel(),
	delay(0),
	extraDashCycles(0),
	dashMulti(1.0),
	controlMulti(0.0),
	exhaustionTime(0),
	calculated(true),
	hasDir(false),
	playerPtr(NULL)
{
}

FastPlayer::FastPlayer( const WorldModel * worldModel, int _delay, float _controlBuff , float _dashMulti ,const Player * _playerPtr
	    , int _extraDashCycles, bool _plusVel) :
		pos(0.0,0.0),
		vel(0.0,0.0),
		delay(_delay),
		extraDashCycles(_extraDashCycles),
		dashMulti(_dashMulti),
		controlMulti(_controlBuff),
		calculated(false),
		playerPtr(_playerPtr),
		plusVel(_plusVel)
{
	exhaustionTime = 1000;
	if (_playerPtr)
	{
		exhaustionTime = getExhaustionTime(worldModel);
		if (not(_playerPtr->getBodyDirCounter() < 2 or (worldModel->getBody().getUniNum() == _playerPtr->getUniNum() and _playerPtr->getTeamId() == TID_TEAMMATE)))
		{
			if (_playerPtr->getDistance(worldModel->getBall()) > 5.f)
				delay += 1;
			else if (_playerPtr->getDistance(worldModel->getBall()) > 2.5f)
				delay += 0;
		}
		pos = _playerPtr->getPos();
		vel = _playerPtr->getVel();
	}
}

int FastPlayer::getExhaustionTime(const WorldModel * worldModel)
{
	float stamina = playerPtr->getStamina();
	if (stamina < 100 or playerPtr->getTeamId() == TID_OPPONENT)
		stamina = 8000;
	for (int result = 0;result < 150;result++)
	{
		if (stamina < 2500)
			return result;
		stamina -= 100 - worldModel->getPlayerType(playerPtr->getType())["stamina_inc_max"].asFloat();
	}
	return 150;
}

FastIC::FastIC() : maxAll(10), maxTmm(3), maxOpp(3), maxCyclesAfter(20), checkQuarters(false), calculateSelf(false)
		, unknownExtraKickable(1.f), maxCycles(100), outFlag(false), outCycle(0)
{
}

FastIC::FastIC(const WorldModel * worldModel, bool quarter) : worldModel(worldModel), maxAll(10), maxTmm(3), maxOpp(3), checkQuarters(quarter)
		, calculateSelf(false), unknownExtraKickable(1.f), maxCycles(100), outFlag(false), outCycle(0), maxCyclesAfter(100)
{
    ball = FastBall();
	if (not isSet)
	{
		Body body[18];
		for ( unsigned i = 0; i < TYPESCOUNT; i++ )
		{
			body[i] = worldModel->getBody();
			body[i].setServerParamVars(worldModel->getServerParam());
			body[i].setType(i, worldModel->getPlayerType(i));
			body[i].setEffort(body[i].getEffortMax());
			body[i].setPos() = Point(0, 0);
			body[i].setVel().setAsCartesian(0, 0);
			body[i].setBodyDir(0);
			types[i].Distance[0] = 0;
			types[i].KickableArea = worldModel->getBall().getSize() + body[i].getKickableMargin() + body[i].getSize() - .055;
			types[i].CatchableArea = sqrt(pow(worldModel->getServerParam()["catchable_area_w"].asFloat() * 0.5, 2.0) +
				pow(worldModel->getServerParam()["catchable_area_l"].asFloat(), 2.0));
			types[i].Decay = body[i].getDecay();
			for ( unsigned cycle = 0; cycle < 150; cycle++ )
			{
				types[i].Distance[cycle] = body[i].getPos().getX();
				float DashPower = MAXDASH;
				DashPower = worldModel->getSafetyDashPower(DashPower, body[i].getStamina());
				DashCommand command(DashPower);
				body[i].simulateByAction(&command);
				body[i].simulateByDynamics();
			}
			float rateStep = body[i].getSpeedMax() / (float)TURN_VELCOUNT;
			types[i].Turn.maxVelocity = body[i].getSpeedMax();
			for ( unsigned int rateCount = 0; rateCount < TURN_VELCOUNT; rateCount++ )
			{
				float thisRate = rateCount * rateStep;
				float angleStep = 180.f / (float)TURN_ANGLECOUNT;
				for ( unsigned int angleCount = 0; angleCount < TURN_ANGLECOUNT; angleCount++ )
				{
					float threshold = 5.f;
					float thisAngle = angleCount * angleStep;
					int turnCycles = 0;
					float calculatingRate = thisRate;
					while (true)
					{
						float turnAngle = thisAngle;
						if ( std::fabs(turnAngle) < threshold )
							break;
						turnAngle = turnAngle * (1 + body[i].getInertiaMoment() * calculatingRate );
						if ( std::fabs(turnAngle) > body[i].getMaxMoment() )
							turnAngle = Degree::normalizeAngle( (turnAngle / std::fabs(turnAngle) ) * body[i].getMaxMoment() );
						turnAngle = turnAngle / (1 + body[i].getInertiaMoment() * calculatingRate );
						calculatingRate *= body[i].getDecay();
						thisAngle -= turnAngle;
						turnCycles++;
					}
					types[i].Turn.cyclesToTurn[rateCount][angleCount] = turnCycles;
				}
			}
		}
		isSet = true;
	}
}

FastIC::~FastIC()
{
	deletePointers();
}

void FastIC::addPlayer(const Player * _playerPtr, int _delay, float _controlBuff ,float _dashMulti, int extraDashCycle, bool _plusVel)
{
	fastPlayers.push_back(FastPlayer(worldModel, _delay, _controlBuff , _dashMulti , _playerPtr, extraDashCycle, _plusVel));
}

vector<FastPlayer> & FastIC::players()
{
	return fastPlayers;
}

void FastIC::refresh()
{
	outFlag = false;
	outCycle = 0;
	for (vector<FastPlayer>::iterator it = fastPlayers.begin();it != fastPlayers.end();it++)
	{
		if (it->playerPtr)
		{
			it->pos = it->playerPtr->getPos();
			it->vel = it->playerPtr->getVel();
			it->calculated = false;
			if (it->plusVel)
			    it->pos += it->playerPtr->getVel() / it->playerPtr->getDecay();
		}
	}
}

void FastIC::updateBody(unsigned updateCycle)
{
	for (vector<FastPlayer>::iterator it = fastPlayers.begin();it != fastPlayers.end();it++)
	{
		if (it->playerPtr and it->playerPtr->isBody())
		{
			for (int i = 0;i < updateCycle;i++)
			{
				it->pos += it->vel;
				it->vel.x *= it->playerPtr->getDecay();
				it->vel.y *= it->playerPtr->getDecay();
			}
			it->delay += updateCycle;
			return;
		}
	}
}

const std::vector<FastestPlayer> & FastIC::getFastestPlayers() const
{
	return fastestPlayers;
}

bool compareByReachCycle(FastestPlayer i, FastestPlayer j)
{
	return (i.reachCycle < j.reachCycle);
}

void FastIC::calculate()
{
	int cycle = 0;

	bool shouldContinue = true;
	bool quarterCheck = checkQuarters;
	int lastReach = -1;

	int calAll = 0, calTmm = 0, calOpp = 0, firstCycle = -1000;

	Vector tmpBallPos = ball.pos;
	Vector tmpBallVel = ball.vel;
	fastestPlayers.clear();

	for (vector<FastPlayer>::iterator it = fastPlayers.begin();it != fastPlayers.end();it++)
	{
	    it->hasDir = false;
	    if ((it->playerPtr->getBodyDirCounter() < 2) or it->playerPtr->isBody())
		it->hasDir = true;
	}

	while (shouldContinue)
	{
		shouldContinue = false;
		for (vector<FastPlayer>::iterator it = fastPlayers.begin();it != fastPlayers.end();it++)
		{
			if (it->calculated or (it->playerPtr->getModel() == PLM_QUARTER and !quarterCheck)/* or it->playerPtr->getTackleRemainCycle() > cycle*/)
				continue;
			int playerType = it->playerPtr->getType();
			shouldContinue = true;
			int turnCycle = 0;
			if (it->hasDir)
			{
				Vector itPosToBall;
				itPosToBall.setByPoints(tmpBallPos, it->pos);

				float targetDir = (tmpBallPos - it->pos).getDirection();
				float dirChange = min(fabs(normalizeAngle(it->playerPtr->getBodyDir() - targetDir)), fabs(normalizeAngle(180 - (it->playerPtr->getBodyDir() - targetDir))));
				float minDir = 0.0;
				if (it->controlMulti * types[playerType].KickableArea <= (tmpBallPos - it->pos).getMagnitude())
					minDir = arcSin(it->controlMulti * types[playerType].KickableArea / (tmpBallPos - it->pos).getMagnitude());
				if (dirChange <= minDir)
					dirChange = 0.0;
				turnCycle = types[playerType].Turn.getCyclesToTurn(Vector(it->vel).getMagnitude(), dirChange);
			}
			float dashDistance = types[playerType].Distance[max(0, min(cycle - turnCycle - it->delay , it->exhaustionTime))] + (max(0, cycle - it->exhaustionTime) * EXHAUSTION_RATE);
			dashDistance += EXTRA_DASH_RATE * it->extraDashCycles;
			dashDistance *= it->dashMulti;
			float controlBuff = it->controlMulti * (types[playerType].KickableArea);
			float distToBall = it->pos.getDistance(tmpBallPos);
			if ( (Rectangle(36.0, 20.0, 52.5, -20.0).isInRectangle(tmpBallPos) and it->playerPtr->isGoalie() and (it->playerPtr->getTeamId() != TID_TEAMMATE)) )
				controlBuff = it->controlMulti * types[playerType].CatchableArea - 0.055f; // REPLACE WITH HETERO CATCHABLE IN SERVER 14
			if (it->playerPtr->getTeamId() == TID_OPPONENT and it->playerPtr->getType())
				controlBuff *= unknownExtraKickable;
			if (dashDistance + controlBuff > distToBall)
			{
				it->calculated = true;
				if (quarterCheck)
					if (lastReach == -1)
						lastReach = cycle;
					else
						if (cycle != lastReach)
							quarterCheck = false;
				if (firstCycle == 1000)
				    maxCyclesAfter = cycle;
				fastestPlayers.push_back(FastestPlayer(it->playerPtr->getUniNum(), it->playerPtr->getTeamId() == TID_TEAMMATE, cycle, tmpBallPos, it->playerPtr));
				if (it->playerPtr->getUniNum() == worldModel->getBody().getUniNum() and it->playerPtr->getTeamId() == TID_TEAMMATE)
					calculateSelf = false;
				calAll++;
				if (it->playerPtr->getTeamId() == TID_TEAMMATE)
					calTmm++;
				else if (it->playerPtr->getTeamId() == TID_OPPONENT)
					calOpp++;
				tmpBallVel = Point(0.0, 0.0);
			}
			it->pos += it->vel;
			it->vel.x *= types[playerType].Decay;
			it->vel.y *= types[playerType].Decay;
		}
		if (cycle >= ball.delay)
		{
			tmpBallPos += tmpBallVel;
			tmpBallVel *= ball.decay;
		}
		if ((not calculateSelf) and ((calAll >= maxAll) or (calTmm >= maxTmm) or (calOpp >= maxOpp) or (maxCyclesAfter >= cycle - firstCycle)))
		{
			shouldContinue = false;
			break;
		}
		if (cycle >= maxCycles)
		{
			shouldContinue = false;
		}
		if (not Rectangle(-53.0, 35.0, 53.0, -35.0).isInRectangle(tmpBallPos))
		{
			outFlag = true;
			outCycle = cycle;
			shouldContinue = false;
		}
		cycle++;
	}

}

void FastIC::setByWorldModel()
{
	deletePointers();
	fastPlayers.clear();
	fastestPlayers.clear();

	ball.pos = worldModel->getBall().getPos();
	ball.vel = worldModel->getBall().getVel();
	ball.delay = 0;
	ball.decay = worldModel->getBall().getDecay();

	for (int i = 0;i < 11;i++)
	{
		const Player * opp = &worldModel->getFullPlayer(TID_OPPONENT, i);

		if ((not opp) or (not opp->isValid()))
			continue;

		int turnDelay = 0;
		float kickableArea = 1.f;
		if (opp->getDistance(worldModel->getBall()) < 1.5) // May be in kickable
		    kickableArea = 1.1f;

		fastPlayers.push_back(FastPlayer(worldModel, turnDelay, kickableArea, 1.0, opp));
	}

	for (int i = 0;i < 20;i++)
	{
		const Player * opp = &worldModel->getHalfPlayer(TID_OPPONENT, i);

		if ((not opp) or (not opp->isValid()))
			continue;

		float kickableArea = 1.f;
		if (opp->getDistance(worldModel->getBall()) < 1.5) // May be in kickable
		    kickableArea = 1.1f;

		int turnDelay = 1;
		Vector vel;
		vel.setAsCartesian(0.0, 0.0);

		fastPlayers.push_back(FastPlayer(worldModel, turnDelay, kickableArea, 1.0, opp));
	}

	for (int i = 0;i < 11;i++)
	{
		const Player * tmm = &worldModel->getFullPlayer(TID_TEAMMATE, i);

		if ((not tmm) or (not tmm->isValid()))
			continue;

		if (tmm->getUniNum() == worldModel->getBody().getUniNum())
		{
			//LOG << "Khodamam!" << endl;
			continue;
		}

		int turnDelay = 0;

		fastPlayers.push_back(FastPlayer(worldModel, turnDelay, 1.0, 1.0, tmm));
	}

	for (int i = 0;i < 20;i++)
	{
		const Player * tmm = &worldModel->getHalfPlayer(TID_TEAMMATE, i);

		if ((not tmm) or (not tmm->isValid()))
			continue;

		int turnDelay = 1;
		if (tmm->getUniNum() == worldModel->getBody().getUniNum())
		{
			continue;
		}
		fastPlayers.push_back(FastPlayer(worldModel, turnDelay, 1.0, 1.0, tmm));
	}

	if (checkQuarters)
	{
		for (int i = 0;i < 30;i++)
		{
			const Player * quar = &worldModel->getQuarterPlayer(i);

			if ((not quar) or (not quar->isValid()))
				continue;

			int turnDelay = 1;
			fastPlayers.push_back(FastPlayer(worldModel, turnDelay, 1.0, 1.0, quar));
		}
	}
	int turnDelay = 0;
	fastPlayers.push_back(FastPlayer(worldModel, turnDelay, 1.0, 1.0, &worldModel->getBody()));
}

void FastIC::setBall(Point ballPos, Point ballVel, int ballDelay, float ballDecay)
{
	/*LOG << "ds" << endl;
	LOG << " " << ballPos << endl;
	LOG << "K: " << &ball << endl;
	LOG << " 2ea " << ball.pos << endl; */
	ball.pos = ballPos;
	/*LOG << "dfgdfg " << endl;
	LOG << " " << ballPos << endl;
	LOG << " 2ea " << ball.pos << endl; */
	ball.vel = ballVel;
	ball.delay = ballDelay;
	if (ballDecay == -9999)
		ballDecay = worldModel->getBall().getDecay();
	ball.decay = ballDecay;
}

void FastIC::setBall(Ball _ball, unsigned delay)
{
	ball.pos = _ball.getPos();
	ball.vel = _ball.getVel();
	ball.decay = _ball.getDecay();
	ball.delay = delay;
}

bool FastIC::isFastestQuarter() const
{
	int first = 150;
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (it->playerPtr->getModel() == PLM_QUARTER and it->reachCycle <= first)
			return true;
		else if (it->reachCycle <= first)
			first = it->reachCycle;
		else
			return false;
	return false;
}

bool FastIC::isSelfFastestPlayer() const
{
	int first = 150;
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (it->isTeammate and (unsigned)it->unum == worldModel->getBody().getUniNum() and it->reachCycle <= first)
			return true;
		else if (it->reachCycle <= first)
			first = it->reachCycle;
		else
			return false;
	return false;
}

bool FastIC::isSelfFastestTeammate() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
	{
		if (it->isTeammate)
		{
			if ((unsigned) it->unum == worldModel->getBody().getUniNum())
				return true;
			else
				return false;
		}
	}
	return false;
}

bool FastIC::isOurTeamBallPossessor() const
{
	int min = 1000;
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (it->isTeammate)
			return it->reachCycle <= min;
		else
			if (min == 1000)
				min = it->reachCycle;
			else
				return false;
	return false;
}

int FastIC::getFastestTeammateReachCycle(bool withMe) const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (it->isTeammate and (withMe or (unsigned)it->unum != worldModel->getBody().getUniNum()))
		{
			return it->reachCycle;
		}
	return 9999;
}

Point FastIC::getFastestTeammateReachPoint(bool withMe) const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (it->isTeammate and (withMe or (unsigned)it->unum != worldModel->getBody().getUniNum()))
		{
			return it->receivePoint;
		}
	return Point(1000, 1000);
}

int FastIC::getFastestOpponentReachCycle() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (it->playerPtr->getTeamId() == TID_OPPONENT)
			return it->reachCycle;
	return 9999;
}

Point FastIC::getFastestOpponentReachPoint() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (not it->isTeammate)
		{
			return it->receivePoint;
		}
	return Point(1000, 1000);
}

int FastIC::getFastestPlayerReachCycle() const
{
	vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
	if (it != fastestPlayers.end())
		return it->reachCycle;
	return 9999;
}

int FastIC::getSelfReachCycle() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
	{
		if (it->isTeammate and (unsigned) it->unum == worldModel->getBody().getUniNum())
			return it->reachCycle;
	}
	return 9999;
}

const Player * FastIC::getFastestTeammate(bool withMe) const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (it->isTeammate and (withMe or it->unum != worldModel->getBody().getUniNum()))
			return it->playerPtr;
	return NULL;
}

const Player * FastIC::getFastestOpponent() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
		if (it->playerPtr->getTeamId() == TID_OPPONENT)
			return it->playerPtr;
	return NULL;
}

const Player * FastIC::getFastestPlayer() const
{
	vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
	if (it != fastestPlayers.end())
		return it->playerPtr;
	return NULL;
}

void FastIC::setMaxCount(unsigned max)
{
	maxAll = max;
}

void FastIC::setMaxTeammateCount(unsigned max)
{
	maxTmm = max;
}

void FastIC::setMaxOpponentCount(unsigned max)
{
	maxOpp = max;
}

void FastIC::setUnknownExtraKickable(float exKick)
{
	unknownExtraKickable = exKick;
}

void FastIC::setMaxCycleAfterFirstFastestPlayer(unsigned max)
{
	maxCyclesAfter = max;
}

void FastIC::setMaxCycles(unsigned max)
{
	maxCycles = max;
}

void FastIC::deletePointers()
{
	/*for (vector<FastPlayer>::iterator it = fastPlayers.begin();it != fastPlayers.end();it++)
		delete it->playerPtr;*/
}

void FastIC::logFastests() const
{
    LOG << "Fastest Players : " << endl;
    for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();it != fastestPlayers.end();it++)
    {
		LOG << "Player (" << it->isTeammate << "," << it->unum << ") ReachCycle = " << it->reachCycle << " " << it->playerPtr << "." << endl;
	    //<< " or Player (" << it->playerPtr->getTeamId() << "," << it->playerPtr->getUniNum() << ")." << endl;
    }
}

void FastIC::setCalculateSelf(bool cal)
{
    calculateSelf = cal;
}

bool FastIC::isOut()
{
	return outFlag;
}

unsigned FastIC::getOutCycle()
{
	return outCycle;
}

/*void FastIC::calTime(string name)
{
	gettimeofday(&timer, NULL);
	times[ct] = timer.tv_usec;
	names[ct] = name;
	ct++;
}

void FastIC::saveTimes()
{
	LOG << "FastIC Time : " << endl;
	for (int i = 0;i < ct;i++)
		LOG << "\t" << names[i] << ": " << times[i] - now << endl;
	ct = 0;
}
*/
