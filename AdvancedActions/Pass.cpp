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
 *  This file is created by: Mohammad Zareie
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#include <Pass.h>
#include <Intercept.h>
#include <Logger.h>
#include <WorldModel.h>
#include <BasicKick.h>
#include <BasicDash.h>
#include <MultiStepKick.h>
#include <Degree.h>
#include <Basics.h>
#include <ChainAction.h>
#include <Share.h>
#include <FastIC.h>
#include <BasicTurn.h>

using namespace std;
using namespace Degree;
using namespace Basics;

#define LOGP LOG
#define WORST_VALUE -1000

Pass::Pass(const WorldModel *worldModel) :
		wm(worldModel), target(Point(0, 0))
{
	validPlayers = "23456789AB";
	if (wm->getPlayMode() != PM_KICK_IN)
		validRect = Rectangle((double) -52.5, (double) -34, (double) wm->getOppOffsideLine(),
				(double) +34);
	else
		validRect = Rectangle((double) -52.5, (double) -34, (double) 52.5, (double) +34);
	invalidRect = Rectangle((double) -100, (double) -100, (double) -99, (double) -99);
	isValueCalculated = false;
	bestEntry.value = WORST_VALUE;
	bestSafeEntry.value = WORST_VALUE;
	bestEntry.target = NULL;
	bestSafeEntry.target = NULL;
	safeDeltaCycle = 2;
	safeDistance = 2;
	maxSteps = 3;
	safeSearch = false;
}

Pass::~Pass()
{
}

void Pass::setValidPlayers(string _validPlayers)
{
	validPlayers = _validPlayers;
}

void Pass::addValidPlayer(int num)
{ // FROM 1 TO 11 :[1,11]
	if (isInPlayers(num, validPlayers))
		return;
	validPlayers.append("X");
	validPlayers[validPlayers.size() - 1] = uniNumToChar(num);
}

Rectangle& Pass::getValidRect()
{
	return validRect;
}

Rectangle& Pass::getinValidRect()
{
	return invalidRect;
}

void Pass::setMaxSteps(unsigned step)
{
	maxSteps = step;
}

void Pass::setSafety(bool safety)
{
	safeSearch = safety;
}

bool compareByX(Point p1, Point p2)
{
	return p1.x < p2.x;
}
bool compareByY(Point p1, Point p2)
{
	return p1.y < p2.y;
}

void Pass::createEntries()
{
	LOGP << "Creating Pass Entries" << endl;
	bool breakerUse = true;
	if (wm->getBall().getPos().getX() < -20)
		breakerUse = false;
	PassEntry tmp;
	float oppOffsideLine = wm->getOppOffsideLine();
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		LOGP << "checking player number " << i + 1 << endl;
		if (!isInPlayers(i + 1, validPlayers))
		{
			LOGP << "player is not in valid players" << endl;
			continue;
		}

		if (wm->getBody().getUniNum() == i + 1)
		{
			LOGP << "cheking my self" << endl;
			continue;
		}

		if (!wm->getFullPlayer(TID_TEAMMATE, i).isValid())
		{
			LOGP << "player simcounter seems not fine using isValid()" << endl;
			LOGP << "Player Simcounter : " << wm->getFullPlayer(TID_TEAMMATE, i).getSimCounter()
					<< endl;
			continue;
		}

		if (!validRect.isInRectangle(wm->getFullPlayer(TID_TEAMMATE, i).getPos()))
		{
			LOGP << "player is not in validRect" << endl;
			continue;
		}

		if (invalidRect.isInRectangle(wm->getFullPlayer(TID_TEAMMATE, i).getPos()))
		{
			LOGP << "player is in  invalidRect" << endl;
			continue;
		}

		tmp.type = DIRECT;
		tmp.releaseCycle = wm->getCurCycle();
		tmp.noKickCycle = wm->getCurCycle() + 2;
		tmp.target = &wm->getFullPlayer(TID_TEAMMATE, i);
		tmp.targetUniNum = i + 1;
		tmp.targetModel = tmp.target->getModel();
		tmp.targetPoint = tmp.target->getPos();

		if (wm->getBody().getPos().getDistance(tmp.targetPoint) < 5
				and wm->getPlayMode() == PM_PLAY_ON)
		{
			LOGP << "Kheily Nazdike Behesh Pass Nemidam" << endl;
			continue;
		}
		tmp.direction = (tmp.targetPoint - wm->getBody().getPos()).getDirection();
//		if (wm->getSeeDeltaCycle(tmp.direction) > 3)
//			continue;
		addSpeedEntry(tmp);
		if (breakerUse)
		{ //BREAKER
			tmp.type = BREAKER;
			float b[] =
			{ -2.5, 5, 10, 15, 20 };
			for (unsigned j = 0; j < 5; j++)
			{
				tmp.targetPoint = tmp.target->getPos();
				tmp.targetPoint.x += b[j];
				if (tmp.targetPoint.x > 50)
					break;
				if (tmp.targetPoint.x < oppOffsideLine - 10)
					continue;
				tmp.direction = (tmp.targetPoint - wm->getBody().getPos()).getDirection();
//				if (wm->getSeeDeltaCycle(tmp.direction) > 3)
//					continue;
				addSpeedEntry(tmp);
			}
		}
		{ //INDIRECT
			tmp.type = INDIRECT;
			tmp.direction += 30;
			tmp.targetPoint = Point(1000, 1000);
//			addSpeedEntry(tmp);
			tmp.direction -= 60;
			tmp.targetPoint = Point(1000, 1000);
//			addSpeedEntry(tmp);
		}
	}
	for (unsigned i = 0; i < HALF_PLAYERS_NUM; i++)
	{
		LOGP << "checking half player number " << i + 1 << endl;

		if (!&wm->getHalfPlayer(TID_TEAMMATE, i))
			continue;
		if (!wm->getHalfPlayer(TID_TEAMMATE, i).isValid())
		{
			LOGP << "player simcounter seems not fine using isValid()" << endl;
			continue;
		}

		if (!validRect.isInRectangle(wm->getHalfPlayer(TID_TEAMMATE, i).getPos()))
		{
			LOGP << "player is not in validRect" << endl;
			continue;
		}

		if (invalidRect.isInRectangle(wm->getHalfPlayer(TID_TEAMMATE, i).getPos()))
		{
			LOGP << "player is in  invalidRect" << endl;
			continue;
		}

		tmp.type = DIRECT;
		tmp.releaseCycle = wm->getCurCycle();
		tmp.noKickCycle = wm->getCurCycle() + 2;
		tmp.target = &wm->getHalfPlayer(TID_TEAMMATE, i);
		tmp.targetUniNum = i + 1;
		tmp.targetModel = tmp.target->getModel();
		tmp.targetPoint = tmp.target->getPos();
		tmp.direction = (tmp.targetPoint - wm->getBody().getPos()).getDirection();
//		if (wm->getSeeDeltaCycle(tmp.direction) > 3)
//			continue;
		addSpeedEntry(tmp);

		if (breakerUse)
		{ //BREAKER
			tmp.type = BREAKER;
			float step = 5;
			float max = 15;
			float min = -5;
			for (; min <= max; min += step)
			{
				if (min == 0)
					continue;
				tmp.targetPoint = tmp.target->getPos();
				tmp.targetPoint.x += min;
				if (tmp.targetPoint.x > 50)
					break;
				if (tmp.targetPoint.x < oppOffsideLine - 10)
					continue;
				tmp.direction = (tmp.targetPoint - wm->getBody().getPos()).getDirection();
				//addSpeedEntry(tmp);
			}
		}
		{ //INDIRECT
			tmp.type = INDIRECT;
			tmp.direction += 30;
			tmp.targetPoint = Point(1000, 1000);
//			addSpeedEntry(tmp);
			tmp.direction -= 60;
			tmp.targetPoint = Point(1000, 1000);
//			addSpeedEntry(tmp);
		}
	}

//	if (wm->getBall().getPos().getX() > 0) //sharte ride
	{ //Course
//		string validPlayers = "9AB";
//		if (!Basics::isInPlayers(wm->getBody().getUniNum(), validPlayers))
//			return;
		Point ballPos = wm->getBall().getPos();
		float offsideX = wm->getOppOffsideLine();
		if (ballPos.x >= offsideX or offsideX - ballPos.x > 10)
			return;
		LOGP << "Checking Course" << endl;
		tmp.type = COURSE;
		vector<Point> players;
//		bool isAdded[5]={0,0,0,0,0};
		for (unsigned i = 1; i < 6; i++)
		{
			if (wm->getFullPlayer(TID_OPPONENT, i).isValid())
			{
				players.push_back(wm->getFullPlayer(TID_OPPONENT, i).getPos());
				LOGP << "Adding Player Number " << i + 1 << endl;
//				if(i<5)
//					isAdded[i]=1;
			}
		}
		sort(players.begin(), players.end(), compareByX);
		reverse(players.begin(), players.end());
		vector<Point> offsidePlayers;
		for (unsigned i = 0; i < players.size() and i < 5; i++)
		{
			offsidePlayers.push_back(players[i]);
		}
		sort(offsidePlayers.begin(), offsidePlayers.end(), compareByY);
		for (unsigned i = 1; i < offsidePlayers.size(); i++)
		{
			Point middle[3];
			middle[0] = (offsidePlayers[i] * 3 + offsidePlayers[i - 1] * 1) / 4;
			middle[1] = (offsidePlayers[i] * 2 + offsidePlayers[i - 1] * 2) / 4;
			middle[2] = (offsidePlayers[i] * 1 + offsidePlayers[i - 1] * 3) / 4;
			for (unsigned j = 0; j < 3; j++)
			{
				LOGP << "Adding Course The Point Is : " << middle << endl;
				tmp.type = COURSE;
				tmp.target = NULL;
				tmp.releaseCycle = wm->getCurCycle();
				tmp.noKickCycle = wm->getCurCycle() + 2;
				tmp.targetPoint = middle[j];
				tmp.direction = (middle[j] - wm->getBody().getPos()).getDirection();
				addSpeedEntry(tmp);
			}
		}
	}
}

void Pass::addSpeedEntry(PassEntry& tmp)
{
	int oppReachCycle = wm->getGlobalFastIC().getFastestOpponentReachCycle();
	float maxVel = 1000;
	//		tmp.vel = MultiStepKick::getMaxDirectionVelocity(wm,
	//				wm->getBody().getPos(), wm->getBody().getVel(),
	//				wm->getBody().getBodyDir(), wm->getBall().getPos(),
	//				wm->getBall().getVel(), 1,
	//				wm->getFullPlayer(TID_TEAMMATE, i).getPos(), chert);
	Vector meToPoint;
//	if (tmp.type == DIRECT) //|| tmp.type == BREAKER)
//	{
//		float arriveVel = 2; //maximum speed of arrival
//		meToPoint.setByPoints(wm->getBall().getPos(), tmp.targetPoint);
//		float maxVel2 = wm->getBall().getDecay()
//				* (arriveVel - (meToPoint.getMagnitude() * (1 - (1 / wm->getBall().getDecay()))));
//		//Calculated maxVel2 by the formula in basicKick :: KickToPointArrive Vel
//		maxVel = min(maxVel, maxVel2);
//		LOGP << "Max Direction Vel : " << maxVel << endl;
//		LOGP << "Age ax in Vel mohkam tar bezanam mikhore too sooratesh : " << maxVel2 << endl;
//	}
	LOGP << "Opp Reach Cycle : " << oppReachCycle << endl;
	int steps = min_max(+1, oppReachCycle - 1, maxSteps);
	LOGP << "Calculating MSK with max step of : " << steps << " And The Target Point : "
			<< tmp.targetPoint << endl;
	MultiStepKick multi(wm, wm->getBody(), wm->getBall(), tmp.targetPoint, maxVel, steps);
	multi.calculate();
	LOGP << " MaxVel is : " << maxVel << endl;
	maxVel = min(maxVel, multi.getMaxAvailableSpeed());
	LOGP << " MaxVel is : " << maxVel << endl;
	if (tmp.type == DIRECT)
	{
		for (float j = maxVel; j >= 0.8; j -= 0.3)
		{
			multi.setDesiredSpeed(j);
			tmp.step = multi.getFinalStep();
			tmp.vel = j;
			tmp.releaseBall = multi.getFinalBallWithoutDynamics();
//		tmp.releaseBall.setVel().setAsPolar(tmp.releaseBall.getVel().getMagnitude() * 0.9,
//				tmp.releaseBall.getVel().getDirection());
			tmp.actions.clear();
			for (unsigned i = 0; i < tmp.step; i++)
				tmp.actions.push_back(multi.getChain(i));
			entries.push_back(tmp);
		}
	}
	else
	{
		for (float j = maxVel; j >= 1.5; j -= 0.2)
		{
			multi.setDesiredSpeed(j);
			tmp.step = multi.getFinalStep();
			tmp.vel = j;
			tmp.releaseBall = multi.getFinalBall();
			//		tmp.releaseBall.setVel().setAsPolar(tmp.releaseBall.getVel().getMagnitude() * 0.9,
			//				tmp.releaseBall.getVel().getDirection());
			tmp.actions.clear();
			for (unsigned i = 0; i < tmp.step; i++)
				tmp.actions.push_back(multi.getChain(i));
			entries.push_back(tmp);
		}
	}
}

void Pass::evaluate(PassEntry &tmp, FastIC &temp)
{
	temp.setBall(tmp.releaseBall, /*tmp.step - 1*/0); //tmp.step);
	temp.refresh();
//	temp.logFasts(LOGP);
	temp.calculate();
	//temp.logFastests();
	tmp.tmmReachCycle = temp.getFastestTeammateReachCycle(0);
	tmp.reachDist = temp.getFastestTeammateReachDist(0);
	tmp.oppReachCycle = temp.getFastestOpponentReachCycle();
	//		tmp.oppReachCycle -= (wm->getSeeDeltaCycle(tmp.direction)
	//				+ wm->getSeeDeltaCycle(tmp.direction + 5) + wm->getSeeDeltaCycle(tmp.direction - 5))
	//				/ 6; //for more safety
	tmp.reachPoint = temp.getFastestPlayerReachPoint();
	//		tmp.value = wm->getPointValue(tmp.reachPoint);
	tmp.reachPointValue = wm->getPointValue(tmp.reachPoint);
	if (tmp.type == COURSE)
		tmp.target = temp.getFastestTeammate(0); //I didn't know he is the target until now
//	LOGP << endl << "Evaling this entry :" << tmp;
	if (!temp.getFastestTeammate(0))
	{
		LOGP << endl << "Evaling this entry :" << tmp;
		LOGP << "fastest teammate nadarim" << endl;
		if (temp.getFastestOpponent())
		{
			LOGP << "fastest OPP uninum : " << temp.getFastestOpponent()->getUniNum() << endl;
			LOGP << "fastest OPP Pos : " << temp.getFastestOpponent()->getPos() << endl;
		}
		tmp.value = WORST_VALUE;

		return;
	}
	LOGP << "Fastest tmm uninum : " << temp.getFastestTeammate(0)->getUniNum() << endl;
	LOGP << "Fastest tmm pos : " << temp.getFastestTeammate(0)->getPos() << endl;
	if (temp.getFastestTeammate(0) != tmp.target)
	{
		LOGP << endl << "Evaling this entry :" << tmp;
//		tmp.target = temp.getFastestTeammate(0);
//		tmp.targetModel = tmp.target->getModel();
//		tmp.targetPoint = tmp.target->getPos();
//		tmp.targetUniNum = tmp.target->getUniNum();
		LOGP << "be yeki dige mirese (" << temp.getFastestTeammate(0)->getUniNum() << ")" << endl;
		tmp.value = WORST_VALUE;
		return;
//		LOGP << "farz mikonim be oon pas dadim :D" << endl;
	}
	if (tmp.oppReachCycle <= tmp.tmmReachCycle)
	{
		LOGP << endl << "Evaling this entry :" << tmp;
		if (temp.getFastestOpponent()->getUniNum())
			LOGP << "harif zoodtar migire (" << temp.getFastestOpponent()->getUniNum() << ")"
					<< endl;
		else
			LOGP << " harif zoodyar migire taze half ham hast posesh ham : "
					<< temp.getFastestOpponent()->getPos() << endl;
		tmp.value = WORST_VALUE;
		return;
	}
//	tmp.oppCircleCount = wm->getOppCountInDistanceFromPoint(tmp.reachPoint, safetyCycleRadius);
	tmp.directionSeeDeltaCycle = wm->getSeeDeltaCycle(tmp.direction);
	tmp.isCrowded = isCrowded(tmp, temp.getPlayersDistances());
	tmp.oppCircleCount = getOppCircleCount(temp.getPlayersDistances());
	tmp.invalidOppValue = getInvalidOppValue(tmp);
	tmp.isSafe = isSafe(tmp);
	tmp.isInOppDangerArea = wm->isPointInOppDangerArea(tmp.reachPoint);
	tmp.isInOurDangerArea = wm->isPointInOurDangerArea(tmp.reachPoint);
	tmp.reachBallVel = temp.getFirstReachVel();
	tmp.isMarked = wm->getOppCountInDistanceFromPoint(tmp.target->getPos(), 3);
	tmp.isAfterOffside = false;
	if (tmp.reachPoint.x > wm->getOppOffsideLine(15))
		tmp.isAfterOffside = true;
	tmp.minOppDist = temp.getMinOppDist();
	tmp.minOppDistCycle = temp.getMinOppDistCycle();
	tmp.calculateValue();
	LOGP << endl << "Evaling this entry :" << tmp;
}

bool Pass::isStillGood(PassEntry &entry)
{
	FastIC temp(wm);
	temp.setByWorldModel(0, 0, 1, 6, 1, 0); //, 20, 1, 1);
	temp.addVirtualGoalie();
	temp.addPlayer(entry.target);
	temp.setMaxOpponentCount(2);
	temp.setMaxCycleAfterFirstFastestPlayer(5);
	temp.setMaxCycles(40);
	evaluate(entry, temp);
	if (entry.value == WORST_VALUE)
		return false;
	return true;

}
void Pass::evaluateEntries()
{
	LOGP << "Evaluating Entries" << endl;
	LOGP << "Entries Size : " << entries.size() << endl;
	FastIC temp(wm);
	FastIC temp2(wm);
	if (safeSearch)
		temp.setByWorldModel(0, 0, 1, 6, 0, 0, 0, 1.3);
	else
		temp.setByWorldModel(0, 0, 1, 6, 1, 0);
	if (safeSearch)
		temp2.setByWorldModel(0, 0, 1, 6, 0, 0, 0, 1.3);
	else
		temp2.setByWorldModel(0, 0, 1, 6, 1, 0);
	if (safeSearch)
	{
		for (unsigned i = 0; i < temp.players().size(); i++)
			if (temp.players()[i].playerPtr->getTeamId() != TID_TEAMMATE)
				temp.players()[i].dashMulti = 1.2;
		for (unsigned i = 0; i < temp2.players().size(); i++)
			if (temp2.players()[i].playerPtr->getTeamId() != TID_TEAMMATE)
				temp2.players()[i].dashMulti = 1.2;
	}

//	temp2.setByWorldModel(0, 1, 1, 6, 1, 0); //, 20, 1, 1);//for course because we don't know who is target we add all tmms
	temp.addVirtualGoalie();
	temp2.addVirtualGoalie();
	temp.setMaxOpponentCount(2);
	temp2.setMaxOpponentCount(2);
	temp.setMaxCycleAfterFirstFastestPlayer(5);
	temp2.setMaxCycleAfterFirstFastestPlayer(5);
	temp.setMaxCycles(40);
	temp2.setMaxCycles(40);
	temp.setCalculateDistances(true);
	temp2.setCalculateDistances(true);
//	temp.logFasts(LOGP);
	for (unsigned i = 0; i < entries.size(); i++)
	{
		if (entries[i].type != COURSE)
		{
			temp.addPlayer(entries[i].target);
			evaluate(entries[i], temp);
			temp.removePlayer(entries[i].target);
		}
		else
			evaluate(entries[i], temp2);
	}
}

void Pass::findBestEntry()
{
	for (unsigned i = 0; i < entries.size(); i++)
	{
		if (bestEntry < entries[i])
			bestEntry = entries[i];
		if (entries[i].isSafe)
		{
			if (bestSafeEntry < entries[i])
				bestSafeEntry = entries[i];
		}
	}
	//LOGP <<endl<<endl<< "Best Entry :" << bestEntry << endl<<endl;
}
bool Pass::isSafe(const PassEntry& entry) const
{
	if (entry.oppReachCycle - entry.tmmReachCycle < (int) safeDeltaCycle)
	{
		LOGP << "Not Safe Because Of SafeDeltaCycle" << endl;
		return false;
	}
	if (entry.isCrowded)
		return false;
//	if (entry.isMarked)
//		return false;
	if (entry.reachDist < 0.1) //momkene be toop narese vas hamin miam ba kickable*0.9 ham emtehan mikonim
	{
		FastIC temp(wm);
		temp.setMaxCount(1);
		temp.setMaxCycles(40);
		temp.addPlayer(entry.target, 1, 0.9);
		temp.setBall(entry.releaseBall, entry.step);
		temp.refresh();
		temp.calculate();
		if (temp.getFastestTeammateReachCycle() <= entry.oppReachCycle)
			return false;
	}
	return true;
}

bool Pass::isCrowded(PassEntry &tmp, vector<pair<float, const Player*> > v) const
{
	sort(v.begin(), v.end());
	unsigned count = 0;

	for (unsigned i = 0; i < v.size(); i++)
	{
		if (v[i].second->getTeamId() == TID_TEAMMATE)
			continue;
		if (v[i].first < safeDistance + tmp.target->getSimCounter())
			return true;
		else if (v[i].first < safeDistance + 2 + tmp.target->getSimCounter())
			count++;
		else
			//i sorted them so we won't find any smaller
			break;
	}
	if (count > 1)
		return true;
	return false;
}

unsigned Pass::getOppCircleCount(vector<pair<float, const Player*> > v) const
{
	sort(v.begin(), v.end());
	unsigned count = 0;
	const float safetyCycleRadius = 3;
	for (unsigned i = 0; i < v.size(); i++)
	{
		if (v[i].second->getTeamId() == TID_TEAMMATE)
			continue;
		if (v[i].first < safetyCycleRadius)
			count++;
	}
	return count;
}

float Pass::getInvalidOppValue(const PassEntry& entry) const
{
	float minValue = 10000;
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		if (!wm->getFullPlayer(TID_OPPONENT, i).isValid())
		{
			float value = wm->getFullPlayer(TID_OPPONENT, i).getPos().getDistance(entry.reachPoint)
					/ wm->getFullPlayer(TID_OPPONENT, i).getSimCounter();
			if (wm->getFullPlayer(TID_OPPONENT, i).getSimCounter() > 1000) //100% invalid simcounter
				continue;
			minValue = min(minValue, value);
		}
	}
	return minValue;
}

bool Pass::isPassAvailable()
{
	if (!isValueCalculated)
		getValue();
	if (bestEntry.value == WORST_VALUE)
		return false;
	return true;
}

bool Pass::isSafePassAvailable()
{
	if (!isValueCalculated)
		getValue();
	if (bestSafeEntry.value == WORST_VALUE)
		return false;
	return true;
}

float Pass::getValue()
{
	if (!isValueCalculated)
	{
		LOGP << "Calculating value" << endl;
		LOGP << "createEntries() started" << endl;
		createEntries();
		LOGP << "createEntries() finished" << endl;
		LOGP << "evaluateEntries() started" << endl;
		evaluateEntries();
		LOGP << "evaluateEntries() finished" << endl;
		LOGP << "findBestEntry() started" << endl;
		findBestEntry();
		LOGP << "findBestEntry() finished" << endl;
	}
	isValueCalculated = true;
	LOGP << " returning " << bestEntry.value << " as final pass value" << endl;
	LOGP << "Best Pass Entry : " << endl << bestEntry << endl;
	return bestEntry.value;
}

float Pass::getSafeValue()
{
	if (!isValueCalculated)
		getValue();
	return bestSafeEntry.value;
}

PassEntry Pass::getBestEntry() const
{
	return bestEntry;
}

PassEntry Pass::getBestSafeEntry() const
{
	return bestSafeEntry;
}

void Pass::decide(Form &form)
{
	/*if (fabs(bestEntry.direction) > 90 and wm->getGlobalFastIC().getFastestOpponentReachCycle() > 3
	 and wm->getPlayMode() == PM_PLAY_ON)
	 {
	 LOGP << "Goore Babash wide negah mikonam" << endl;
	 bestEntry.noKickCycle = 0;
	 bestEntry.noMoveCycle = 0;
	 command = NULL;
	 Ball vBall = wm->getBall();
	 vBall.simulateByDynamics(wm->getBody());
	 Body vBody = wm->getBody();
	 vBody.simulateByDynamics();
	 if (vBody.getPos().getDistance(vBall.getPos()) < wm->getKickableArea() - 0.05)
	 {
	 LOGP << "Niazi be kick nist toop belfetre too kickable mimoone" << endl;
	 Share::chain.clear();
	 command = TurnToAngle(0, wm->getBody()).getCommand();
	 form.headForm.tnMode=TNM_LOOK_TO_DIR_WIDE;
	 form.headForm.direction=0;
	 return;
	 }
	 MultiStepKick::HoldBall hold(wm);
	 /*if(hold.decide(3))
	 command = hold.getCommand();
	 else *//*
	 unsigned cycle = 3;
	 if (hold.decide(--cycle))
	 command = hold.getCommand();
	 else if (hold.decide(--cycle))
	 command = hold.getCommand();
	 else if (hold.decide(--cycle))
	 command = hold.getCommand();
	 else
	 {
	 LOGP << "Heif nemitoonam toopo to kickable am negah daram :(" << endl;
	 }
	 if (command != NULL)
	 {
	 Share::chain.clear();
	 ChainAction ca;
	 ca.type = CA_TURNDIR;
	 ca.dir = 0;
	 ca.isWide = true;
	 ca.cycle = wm->getCurCycle() + 1;
	 Share::chain.push_back(ca);
	 if (!TurnToAngle(ca.dir, vBody).isOneTurnPossible() && cycle > 1)
	 { //age cycle<=1 nemitoonam 2 cycle poshte sare ham turn konam toop az dast mire
	 ca.cycle = wm->getCurCycle() + 2;
	 Share::chain.push_back(ca);
	 }
	 }
	 }
	 else */
//	if (wm->getPlayMode() == PM_PLAY_ON
//			&& ((wm->getPointValue(wm->getBall().getPos()) > wm->getPointValue(bestEntry.reachPoint)
//					&& wm->getGlobalFastIC().getFastestOpponentReachCycle() > 4
//			/*&& bestEntry.oppReachCycle - bestEntry.tmmReachCycle > 2
//			 && wm->getPlayMode() == PM_PLAY_ON*/) || bestEntry.value == WORST_VALUE))
	if (bestEntry.value == WORST_VALUE)
	{
		LOG << "I'm forced to Pass but i can't so holding ball" << endl;
		HoldBall hold(wm);
		hold.decide();
		command = hold.getCommand();
//		form.headForm.direction = bestEntry.direction;
//		form.headForm.tnMode = TNM_LOOK_TO_DIR_NORMAL;
		form.headForm.direction = (Point(52, 0) - wm->getBody().getPos()).getDirection();
		form.headForm.tnMode = TNM_LOOK_TO_DIR_NARROW;
		LOGP << "holding the ball" << endl;
		return;
	}

	float dir = (bestEntry.reachPoint - wm->getBody().getPos()).getDirection(); //better than bestEntry.direction i guess
	Share::chain.clear();
	if (wm->getPlayMode() == PM_PLAY_ON
			and (fabs(Degree::getDeltaAngle(wm->getBody().getBodyDir(), dir)) > 30
					or wm->getSeeDeltaCycle(dir) > 3)
			and wm->getGlobalFastIC().getFastestOpponentReachCycle() > safeDeltaCycle
			and !wm->getOppCountInDistanceFromPoint(wm->getBall().getPos(), 3)
			and !wm->isPointInOppDangerArea(wm->getBall().getPos()))
	{
		form.headForm.tnMode = TNM_LOOK_TO_DIR_NARROW;
		if (wm->getSeeDeltaCycle(dir) > 3)
			form.headForm.direction = dir;
		else
			form.headForm.direction = (Point(52.5, 0) - wm->getBody().getPos()).getDirection();
		bestEntry.noKickCycle = 0;
		bestEntry.noMoveCycle = 0;
		LOG << "Forsat Daram bargardam shayad ye chizi behtar peida kardam" << endl;
		command = NULL;
		Ball vBall = wm->getBall();
		vBall.simulateByDynamics(wm->getBody());
		Body vBody = wm->getBody();
		vBody.simulateByDynamics();
		if (vBody.getPos().getDistance(vBall.getPos()) < wm->getKickableArea() - 0.05)
		{
			LOGP << "Niazi be kick nist toop belfetre too kickable mimoone" << endl;
			Share::chain.clear();
			command = TurnToAngle(dir, wm->getBody()).getCommand();
			return;
		}
		HoldBall hold(wm);
		/*if(hold.decide(3))
		 command = hold.getCommand();
		 else */
		unsigned cycle = 3;
		if (hold.decide(--cycle))
			command = hold.getCommand();
		else if (hold.decide(--cycle))
			command = hold.getCommand();
		else if (hold.decide(--cycle))
			command = hold.getCommand();
		else
		{
			LOGP << "Heif nemitoonam toopo to kickable am negah daram :(" << endl;
		}
		if (command != NULL)
		{
			Share::chain.clear();
			ChainAction ca, ca2;
			ca.type = CA_TURNDIR;
			ca2.type = CA_VIEW;
			ca.dir = dir;
			ca2.tnm = TNM_LOOK_TO_DIR_NARROW;
			ca2.dir = dir;
			ca.cycle = wm->getCurCycle() + 1;
			ca2.cycle = wm->getCurCycle() + 1;
			Share::chain.push_back(ca);
			Share::chain.push_back(ca2);
			return;
		}
	}
	else
	{

		ChainAction ca = bestEntry.actions[0];
		ca.wm = wm;
		ca.calCommand();
		command = ca.getCommand();
		Share::chain.clear();
		for (int i = 1; i < bestEntry.step; i++)
		{
			bestEntry.actions[i].cycle = wm->getCurCycle() + i;
			bestEntry.actions[i].type = CA_MULTISTEP_PASS;
			bestEntry.actions[i].passEntry = bestEntry;
			Share::chain.push_back(bestEntry.actions[i]);
		}
		if (bestEntry.step > 1)
		{
			ChainAction say;
			say.type = CA_SAYPASS;
			say.passEntry = bestEntry;
			say.cycle = wm->getCurCycle() + bestEntry.step - 1;
			Share::chain.push_back(say);
		}
//		command = KickToDirectionVelocity(bestEntry.direction, bestEntry.vel, wm->getBall(),
//				wm->getBody()).getCommand();
		LOGP << "I'm going to pass to :" << bestEntry.target->getUniNum() << endl;
		LOGP << "Player Type is : " << bestEntry.target->getModel() << endl;
		LOGP << "Say Information : " << bestEntry << endl;
		form.sayForm.vars.passCycle = (bestEntry.step - 1) % form.sayForm.vars.passCycle.base;
		if (bestEntry.target->getModel() == PLM_FULL)
			form.sayForm.vars.passReceiver = bestEntry.target->getUniNum();
		else
			form.sayForm.vars.passReceiver = wm->getBody().getUniNum(); //majbooram
		form.sayForm.vars.passBallPos = bestEntry.releaseBall.getPos();
		form.sayForm.vars.passDir = absoluteAngle(bestEntry.releaseBall.getVel().getDirection());
		form.sayForm.vars.passVel = bestEntry.releaseBall.getVel().getMagnitude();
		form.sayForm.addSay(ST_PASS);
		LOGP << "size : " << form.sayForm.says.size() << endl;
	}
}

void Pass::safeDecide(Form &form)
{
	swap(bestEntry, bestSafeEntry);
	decide(form);
	swap(bestEntry, bestSafeEntry);
}

Command* Pass::getCommand()
{
	LOGP << "pass dadam" << endl;
	return command;
}

#undef LOGP
