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
#include <Share.h>
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

bool compareByUni(const FastPlayer& p1, const FastPlayer& p2)
{
	if (p1.playerPtr->getTeamId() != p2.playerPtr->getTeamId())
		return p1.playerPtr->getTeamId() > p2.playerPtr->getTeamId();
	if (p1.playerPtr->getModel() != p2.playerPtr->getModel())
		return p1.playerPtr->getModel() > p2.playerPtr->getModel();
	return p1.playerPtr->getUniNum() < p2.playerPtr->getUniNum();
}

int TurnModel::getCyclesToTurn(float velocity, float angleChange)
{
	int velocityBlock = min((int) (velocity / (maxVelocity / (float) TURN_VELCOUNT)),
			TURN_VELCOUNT - 1);
	int angleBlock = min((int) (angleChange / (maxAngleChange / (float) TURN_ANGLECOUNT)),
			TURN_ANGLECOUNT - 1);
	return cyclesToTurn[velocityBlock][angleBlock];
}

FastPlayer::FastPlayer() :
		pos(), vel(), delay(0), extraDashCycles(0), dashMulti(1.0), controlMulti(0.0), exhaustionTime(
				0), calculated(true), hasDir(false), playerPtr(NULL), lastDist(100000)
{
}

FastPlayer::FastPlayer(const WorldModel * worldModel, int _delay, float _controlBuff,
		float _dashMulti, const Player * _playerPtr, int _extraDashCycles, bool _plusVel) :
		pos(0.0, 0.0), vel(0.0, 0.0), delay(_delay), extraDashCycles(_extraDashCycles), dashMulti(
				_dashMulti), controlMulti(_controlBuff), calculated(false), playerPtr(_playerPtr), plusVel(
				_plusVel), lastDist(100000)
{
	exhaustionTime = 1000;
	if (_playerPtr)
	{
		exhaustionTime = getExhaustionTime(worldModel);
		if (not (_playerPtr->getBodyDirCounter() < 3
				or (worldModel->getBody().getUniNum() == _playerPtr->getUniNum()
						and _playerPtr->getTeamId() == TID_TEAMMATE)))
		{
//			if (_playerPtr->getDistance(worldModel->getBall()) > 5.f)
//				delay += 1;
//			else if (_playerPtr->getDistance(worldModel->getBall()) > 2.5f)
//				delay += 0;
		}
		pos = _playerPtr->getPos();
		vel = _playerPtr->getVel();
	}
}

int FastPlayer::getExhaustionTime(const WorldModel * worldModel)
{
	float stamina = playerPtr->getStamina();
	if (stamina < 100 or playerPtr->getTeamId() == TID_OPPONENT)
		stamina = worldModel->getServerParam()["stamina_max"].asFloat();
	for (int result = 0; result < 150; result++)
	{
		if (stamina < 2500)
			return result;
		stamina -= 100
				- worldModel->getPlayerType(playerPtr->getType())["stamina_inc_max"].asFloat();
	}
	return 150;
}

FastIC::FastIC() :
		maxAll(10), maxTmm(3), maxOpp(3), maxCyclesAfter(20), maxCycles(100), checkQuarters(false), calculateSelf(
				false), unknownExtraKickable(1.f), outFlag(false), outCycle(0), calculateDistances(
				1), fixCycle(-1)
{
}

FastIC::FastIC(const WorldModel * worldModel, bool quarter) :
		worldModel(worldModel), maxAll(10), maxTmm(3), maxOpp(3), maxCyclesAfter(100), maxCycles(
				100), checkQuarters(quarter), calculateSelf(false), unknownExtraKickable(1.f), outFlag(
				false), outCycle(0), calculateDistances(1), fixCycle(-1)
{
	ball = FastBall();
	if (not isSet)
	{
		Body body[18];
		for (unsigned i = 0; i < TYPESCOUNT; i++)
		{
			body[i] = worldModel->getBody();
			body[i].setServerParamVars(worldModel->getServerParam());
			body[i].setType(i, worldModel->getPlayerType(i));
			body[i].setEffort(body[i].getEffortMax());
			body[i].setPos() = Point(0, 0);
			body[i].setVel().setAsCartesian(0, 0);
			body[i].setBodyDir(0);
			types[i].Distance[0] = 0;
			types[i].KickableArea = worldModel->getBall().getSize() + body[i].getKickableMargin()
					+ body[i].getSize() - .055;
			types[i].CatchableArea = sqrt(
					pow(worldModel->getServerParam()["catchable_area_w"].asFloat() * 0.5, 2.0)
							+ pow(worldModel->getServerParam()["catchable_area_l"].asFloat(), 2.0));
			types[i].Decay = body[i].getDecay();
			for (unsigned cycle = 0; cycle < 150; cycle++)
			{
				types[i].Distance[cycle] = body[i].getPos().getX();
				float DashPower = MAXDASH;
				DashPower = worldModel->getSafetyDashPower(DashPower, body[i].getStamina());
				DashCommand command(DashPower);
				body[i].simulateByAction(&command);
				body[i].simulateByDynamics();
			}
			float rateStep = body[i].getSpeedMax() / (float) TURN_VELCOUNT;
			types[i].Turn.maxVelocity = body[i].getSpeedMax();
			for (unsigned int rateCount = 0; rateCount < TURN_VELCOUNT; rateCount++)
			{
				float thisRate = rateCount * rateStep;
				float angleStep = 180.f / (float) TURN_ANGLECOUNT;
				for (unsigned int angleCount = 0; angleCount < TURN_ANGLECOUNT; angleCount++)
				{
					float threshold = 5.f;
					float thisAngle = angleCount * angleStep;
					int turnCycles = 0;
					float calculatingRate = thisRate;
					while (true)
					{
						float turnAngle = thisAngle;
						if (std::fabs(turnAngle) < threshold)
							break;
						turnAngle = turnAngle * (1 + body[i].getInertiaMoment() * calculatingRate);
						if (std::fabs(turnAngle) > body[i].getMaxMoment())
							turnAngle = Degree::normalizeAngle(
									(turnAngle / std::fabs(turnAngle)) * body[i].getMaxMoment());
						turnAngle = turnAngle / (1 + body[i].getInertiaMoment() * calculatingRate);
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

void FastIC::addPlayer(const Player * _playerPtr, int _delay, float _controlBuff, float _dashMulti,
		int extraDashCycle, bool _plusVel)
{
	fastPlayers.push_back(
			FastPlayer(worldModel, _delay, _controlBuff, _dashMulti, _playerPtr, extraDashCycle,
					_plusVel));
	sort(fastPlayers.begin(), fastPlayers.end(), compareByUni);
}

void FastIC::removePlayer(const Player* ptr)
{
	for (vector<FastPlayer>::iterator it = fastPlayers.begin(); it != fastPlayers.end(); it++)
	{
		if (it->playerPtr == ptr)
		{
			fastPlayers.erase(it);
			return;
		}
	}
}

vector<FastPlayer> & FastIC::players()
{
	return fastPlayers;
}

vector<pair<float, const Player*> > FastIC::getPlayersDistances() const
{
	return distances;
}

void FastIC::refresh()
{
	outFlag = false;
	goalFlag = false;
	outCycle = 0;
	sort(fastPlayers.begin(), fastPlayers.end(), compareByUni);
	for (vector<FastPlayer>::iterator it = fastPlayers.begin(); it != fastPlayers.end(); it++)
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
	distances.clear();
}

void FastIC::updateBody(unsigned updateCycle)
{
	for (vector<FastPlayer>::iterator it = fastPlayers.begin(); it != fastPlayers.end(); it++)
	{
		if (it->playerPtr and it->playerPtr->isBody())
		{
			for (unsigned i = 0; i < updateCycle; i++)
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

void FastIC::calculate(bool forShoot)
{
	int cycle = 0;

	bool shouldContinue = true;
	bool quarterCheck = checkQuarters;
	int lastReach = -1;

	minOppDist = 1000.f;
	minOppDistCycle = 0;

	int calAll = 0, calTmm = 0, calOpp = 0, firstCycle = -1000;

	Vector tmpBallPos = ball.pos;
	Vector tmpBallVel = ball.vel;
	Vector lastBallPos = ball.pos;
	fastestPlayers.clear();

	for (vector<FastPlayer>::iterator it = fastPlayers.begin(); it != fastPlayers.end(); it++)
	{
		it->hasDir = false;
		if ((it->playerPtr->getBodyDirCounter() < 2) or it->playerPtr->isBody())
			it->hasDir = true;
	}

	while (shouldContinue)
	{
		shouldContinue = false;
		for (vector<FastPlayer>::iterator it = fastPlayers.begin(); it != fastPlayers.end(); it++)
		{
			if (it->calculated
					or (it->playerPtr->getModel() == PLM_QUARTER and !quarterCheck)/* or it->playerPtr->getTackleRemainCycle() > cycle*/)
				continue;
			int playerType = it->playerPtr->getType();
			shouldContinue = true;
			int turnCycle = 0;
			if (it->hasDir)
			{
				Vector itPosToBall;
				itPosToBall.setByPoints(tmpBallPos, it->pos);

				float targetDir = (tmpBallPos - it->pos).getDirection();
				float dirChange = min(fabs(normalizeAngle(it->playerPtr->getBodyDir() - targetDir)),
						fabs(normalizeAngle(180 - (it->playerPtr->getBodyDir() - targetDir))));
				float minDir = 0.0;
				if (it->controlMulti * types[playerType].KickableArea
						<= (tmpBallPos - it->pos).getMagnitude())
					minDir = arcSin(
							it->controlMulti * types[playerType].KickableArea
									/ (tmpBallPos - it->pos).getMagnitude());
				if (dirChange <= minDir)
					dirChange = 0.0;
				turnCycle = types[playerType].Turn.getCyclesToTurn(Vector(it->vel).getMagnitude(),
						dirChange);
			}
			float dashDistance = types[playerType].Distance[max(
					0,
					min(cycle - turnCycle - it->delay /*- it->playerPtr->getTackleRemainCycle()*/,
							it->exhaustionTime))]
					+ (max(0, cycle - it->exhaustionTime) * EXHAUSTION_RATE);
			dashDistance += EXTRA_DASH_RATE * it->extraDashCycles;
			dashDistance *= it->dashMulti;
			float controlBuff = it->controlMulti * (types[playerType].KickableArea);
			float distToBall = it->pos.getDistance(tmpBallPos);
			if ((Rectangle(36.0, 20.0, 52.5, -20.0).isInRectangle(tmpBallPos)
					and it->playerPtr->isGoalie() and (it->playerPtr->getTeamId() != TID_TEAMMATE)))
				controlBuff = it->controlMulti * types[playerType].CatchableArea - 0.055f; // REPLACE WITH HETERO CATCHABLE IN SERVER 14
			if (it->playerPtr->getTeamId() == TID_OPPONENT and it->playerPtr->getType())
				controlBuff *= unknownExtraKickable;
			it->lastDist = max(distToBall - dashDistance, 0.f);
			if (!fastestPlayers.size() and !outFlag and  it->playerPtr->getTeamId() != TID_TEAMMATE
					and minOppDist > distToBall - (dashDistance + controlBuff))
			{
				minOppDist = distToBall - (dashDistance + controlBuff);
				minOppDistCycle = cycle;
			}

			if (dashDistance + controlBuff > distToBall
			/*and it->playerPtr->getTackleRemainCycle() >= cycle*/)
			{
				it->calculated = true;
				if (quarterCheck)
				{
					if (lastReach == -1)
						lastReach = cycle;
					else
					{
						if (cycle != lastReach)
							quarterCheck = false;
					}
				}
				if (firstCycle == 1000)
					maxCyclesAfter = cycle;
				float reachDist = dashDistance + controlBuff - distToBall;
				fastestPlayers.push_back(
						FastestPlayer(it->playerPtr->getUniNum(),
								it->playerPtr->getTeamId() == TID_TEAMMATE, cycle, tmpBallPos,
								it->playerPtr, reachDist));
				if (it->playerPtr->getUniNum() == worldModel->getBody().getUniNum()
						and it->playerPtr->getTeamId() == TID_TEAMMATE)
					calculateSelf = false;
				calAll++;
				if (it->playerPtr->getTeamId() == TID_TEAMMATE)
					calTmm++;
				else if (it->playerPtr->getTeamId() == TID_OPPONENT)
					calOpp++;
				if (fastestPlayers.size() == 1)
					firstReachVel = tmpBallVel;
				tmpBallVel = Point(0.0, 0.0);
			}
			it->pos += it->vel;
			it->vel.x *= types[playerType].Decay;
			it->vel.y *= types[playerType].Decay;
		}
		if (calculateDistances and ((fastestPlayers.size() and fixCycle == -1) or cycle == fixCycle)
				and !distances.size())
		{
			for (vector<FastPlayer>::iterator it = fastPlayers.begin(); it != fastPlayers.end();
					it++)
				distances.push_back(make_pair(it->lastDist, it->playerPtr));
		}
		if (cycle >= ball.delay)
		{
			lastBallPos = tmpBallPos;
			tmpBallPos += tmpBallVel;
			tmpBallVel *= ball.decay;
		}
		if ((not calculateSelf)
				and ((calAll >= maxAll) or (calTmm >= maxTmm) or (calOpp >= maxOpp)
						or (maxCyclesAfter >= cycle - firstCycle)))
		{
			shouldContinue = false;
//			break;
		}
		if (cycle >= maxCycles)
		{
			shouldContinue = false;
		}
		if (not worldModel->isPointInField(tmpBallPos, 0.f))
		{
			if (!outFlag)
			{
				outFlag = true;
				outPoint = tmpBallPos;
				outCycle = cycle;
//				Segment s(lastBallPos, tmpBallPos);
//				Segment goal(Point(52.5, -7), Point(52.5, 7));
//				if (s.isintersect(goal))
//					goalFlag = true;
				Line l(lastBallPos, tmpBallPos);
				if (fabs(l.getY(52.5)) < 7)
					goalFlag = true;
				shouldContinue = false;
				if (forShoot)
				{
					maxCycles = cycle + 10;
					shouldContinue = true;
				}
				tmpBallVel = Point(0.0, 0.0);
			}
		}
		cycle++;
	}

}

void FastIC::setByWorldModel(bool withMe, bool withTmm, bool withOpp, unsigned validCycle,
		unsigned fullOppDelay, unsigned fullTmmDelay, unsigned meDelay, float oppKickable)
{
	deletePointers();
	fastPlayers.clear();
	fastestPlayers.clear();

	ball.pos = worldModel->getBall().getPos();
	ball.vel = worldModel->getBall().getVel();
	ball.delay = 0;
	ball.decay = worldModel->getBall().getDecay();

	if (withOpp)
	{
		for (int i = 0; i < 11; i++)
		{
			const Player * opp = &worldModel->getFullPlayer(TID_OPPONENT, i);

			if ((not opp) or (not opp->isValid(validCycle)))
				continue;

			int turnDelay = fullOppDelay;
			float kickableArea = oppKickable;
			if (opp->getDistance(worldModel->getBall()) < 1.5) // May be in kickable
				kickableArea += .1f;
			if (opp->isGoalie()) //tof
			{
				kickableArea = oppKickable + .05f;
			}

			fastPlayers.push_back(FastPlayer(worldModel, turnDelay, kickableArea, 1.0, opp));
		}

		for (int i = 0; i < 20; i++)
		{
			const Player * opp = &worldModel->getHalfPlayer(TID_OPPONENT, i);

			if ((not opp) or (not opp->isValid(validCycle)))
				continue;

			float kickableArea = oppKickable;
			if (opp->getDistance(worldModel->getBall()) < 1.5) // May be in kickable
				kickableArea += .1f;

			int turnDelay = 1;
			Vector vel;
			vel.setAsCartesian(0.0, 0.0);

			fastPlayers.push_back(FastPlayer(worldModel, turnDelay, kickableArea, 1.0, opp));
		}
	}
	if (withTmm)
	{
		for (int i = 0; i < 11; i++)
		{
			const Player * tmm = &worldModel->getFullPlayer(TID_TEAMMATE, i);

			if ((not tmm) or (not tmm->isValid(validCycle)))
				continue;

			if (tmm->getUniNum() == worldModel->getBody().getUniNum())
			{
				//LOG << "Khodamam!" << endl;
				continue;
			}

			int turnDelay = fullTmmDelay;

			fastPlayers.push_back(FastPlayer(worldModel, turnDelay, 1.0, 1.0, tmm));

		}
		for (int i = 0; i < 20; i++)
		{
			const Player * tmm = &worldModel->getHalfPlayer(TID_TEAMMATE, i);

			if ((not tmm) or (not tmm->isValid(validCycle)))
				continue;

			int turnDelay = 1;
			if (tmm->getUniNum() == worldModel->getBody().getUniNum())
			{
				continue;
			}
			fastPlayers.push_back(FastPlayer(worldModel, turnDelay, 1.0, 1.0, tmm));
		}
	}
	if (checkQuarters)
	{
		for (int i = 0; i < 30; i++)
		{
			const Player * quar = &worldModel->getQuarterPlayer(i);

			if ((not quar) or (not quar->isValid(validCycle)))
				continue;

			int turnDelay = 1;
			fastPlayers.push_back(FastPlayer(worldModel, turnDelay, 1.0, 1.0, quar));
		}
	}
	int turnDelay = meDelay;
	if (withMe)
		fastPlayers.push_back(FastPlayer(worldModel, turnDelay, 1.0, 1.0, &worldModel->getBody()));
	sort(fastPlayers.begin(), fastPlayers.end(), compareByUni);
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

void FastIC::setBall(const Ball _ball, unsigned delay)
{
	ball.pos = _ball.getPos();
	ball.vel = _ball.getVel();
	ball.decay = _ball.getDecay();
	ball.delay = delay;
}

bool FastIC::isFastestQuarter() const
{
	int first = 150;
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
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
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
		if (it->isTeammate and (unsigned) it->unum == worldModel->getBody().getUniNum()
				and it->reachCycle <= first)
			return true;
		else if (it->reachCycle <= first)
			first = it->reachCycle;
		else
			return false;
	return false;
}

bool FastIC::isSelfFastestTeammate() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
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
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
		if (it->isTeammate)
			return it->reachCycle <= min;
		else if (min == 1000)
			min = it->reachCycle;
		else
			return false;
	return false;
}

int FastIC::getFastestTeammateReachCycle(bool withMe) const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
		if (it->isTeammate and (withMe or (unsigned) it->unum != worldModel->getBody().getUniNum()))
		{
			return it->reachCycle;
		}
	return 9999;
}

Point FastIC::getFastestTeammateReachPoint(bool withMe) const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
		if (it->isTeammate and (withMe or (unsigned) it->unum != worldModel->getBody().getUniNum()))
		{
			return it->receivePoint;
		}
	return Point(1000, 1000);
}

float FastIC::getFastestTeammateReachDist(bool withMe) const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
		if (it->isTeammate and (withMe or (unsigned) it->unum != worldModel->getBody().getUniNum()))
		{
			return it->reachDist;
		}
	return 9999;
}

int FastIC::getFastestOpponentReachCycle() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
		if (it->playerPtr->getTeamId() == TID_OPPONENT)
			return it->reachCycle;
	return 9999;
}

Point FastIC::getFastestOpponentReachPoint() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
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

Point FastIC::getFastestPlayerReachPoint() const
{
	vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
	if (it != fastestPlayers.end())
		return it->receivePoint;
	return Point(1000, 1000);
}

int FastIC::getSelfReachCycle() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
	{
		if (it->isTeammate and (unsigned) it->unum == worldModel->getBody().getUniNum())
			return it->reachCycle;
	}
	return 9999;
}

const Player * FastIC::getFastestTeammate(bool withMe) const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
		if (it->isTeammate and (withMe or it->unum != int(worldModel->getBody().getUniNum())))
			return it->playerPtr;
	return NULL;
}

const Player * FastIC::getFastestOpponent() const
{
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
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

Vector FastIC::getFirstReachVel() const
{
	return firstReachVel;
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

void FastIC::setFixCycle(unsigned cycle)
{
	fixCycle = cycle;
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
	logFastests(LOG);
}

void FastIC::logFasts() const
{
	logFasts(LOG);
}

void FastIC::logFastests(LogStream &stream) const
{
	stream << "Fastest Players : " << endl;
	for (vector<FastestPlayer>::const_iterator it = fastestPlayers.begin();
			it != fastestPlayers.end(); it++)
	{
		stream << "Player (" << it->isTeammate << "," << it->unum << ") ReachCycle = "
				<< it->reachCycle << " " << it->playerPtr << "." << endl;
		//<< " or Player (" << it->playerPtr->getTeamId() << "," << it->playerPtr->getUniNum() << ")." << endl;
	}
}

void FastIC::logFasts(LogStream &stream) const
{
	stream << "Fast Players : " << endl;
	for (vector<FastPlayer>::const_iterator it = fastPlayers.begin(); it != fastPlayers.end(); it++)
	{
		stream << "Player (" << it->playerPtr->getTeamId() << "," << it->playerPtr->getUniNum()
				<< ") Pos = " << it->playerPtr->getPos() << "." << endl;
		//<< " or Player (" << it->playerPtr->getTeamId() << "," << it->playerPtr->getUniNum() << ")." << endl;
	}
}

void FastIC::setCalculateSelf(bool cal)
{
	calculateSelf = cal;
}

void FastIC::setCalculateDistances(bool cal)
{
	calculateDistances = cal;
}

void FastIC::addVirtualGoalie()
{
//	// **********WARNING********* DO NOT USE FOR SHOOT BECAUSE IF GOALIE IS SEGMENTED YOU ARE SCREWED
//	for (unsigned i = 0; i < fastPlayers.size(); i++)
//	{
//		if (fastPlayers[i].playerPtr->getTeamId() != TID_TEAMMATE
//				and fastPlayers[i].playerPtr->isGoalie())
//		{
//			LOG << "No Need To Add Virtual Goalie,It's Added" << endl;
//			return;
//		}
//	}
//	const Player* oppGoalie = &worldModel->getFullPlayer(TID_OPPONENT, 0);
//
////	if (worldModel->getFullPlayer(TID_OPPONENT, 0).getSimCounter() < 8)
////	{
////		LOG << "Adding Old Goalie" << endl;
////		LOG << "Sim Counter : " << worldModel->getFullPlayer(TID_OPPONENT, 0).getSimCounter()
////				<< endl;
////		oppGoalie = &worldModel->getFullPlayer(TID_OPPONENT, 0);
////	}
//	//goalie not found
//
//	//finding the goalie from quarter
////	for (unsigned i = 0; i < QUARTER_PLAYERS_NUM and !oppGoalie; i++)
////	{
////		if (worldModel->getQuarterPlayer(i).getPos().getX() > 40
////				and worldModel->getQuarterPlayer(i).getSimCounter() < 4)
////		{
////			LOG << "Adding From QuarterPlayers" << endl;
////			oppGoalie = &worldModel->getQuarterPlayer(i);
////			break;
////		}
////	}
////	if (!oppGoalie)
//	{
//		LOG << "Adding Completly Virtual Goalie" << endl;
//		Player* opp = &Share::virtualOppGoalie;
//		opp->setPos() = worldModel->getFullPlayer(TID_OPPONENT, 0).getPos();
//		if (opp->getPos().getX() < 10) //goalie shoote
//			opp->setPos().setAsCartesian(50, 0);
//		opp->setTeamId(TID_OPPONENT);
//		opp->setStamina(8000);
//		opp->setBodyDirCounter(100);
//		opp->setDecay(0.4); //should be replaced by server param
//		opp->setGoalie(1);
//		opp->setVel().setAsCartesian(0, 0);
//		opp->setType(0);
//		if (worldModel->getFullPlayer(TID_OPPONENT, 0).getSimCounter() > 50)
//			opp->setSimCounter(0);
//		else
//			opp->setSimCounter(worldModel->getFullPlayer(TID_OPPONENT, 0).getSimCounter());
//		oppGoalie = opp;
//		addPlayer(oppGoalie, 0, 1, 1, oppGoalie->getSimCounter() * 10);
//	}
//	LOG << "Goalie Pos : " << oppGoalie->getPos() << endl;
//	LOG << "Goalie Type : " << oppGoalie->getType() << endl;
////	fastPlayers.push_back(FastPlayer(worldModel, 1, 1.1, 1.0, oppGoalie));
}

bool FastIC::isOut()
{
	return outFlag;
}

bool FastIC::isGoal()
{
	return goalFlag;
}

unsigned FastIC::getOutCycle()
{
	return outCycle;
}

Point FastIC::getOutPoint()
{
	return outPoint;
}

float FastIC::getMinOppDist()
{
	return minOppDist;
}

unsigned FastIC::getMinOppDistCycle()
{
	return minOppDistCycle;
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
