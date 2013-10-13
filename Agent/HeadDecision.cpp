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
 *  	and is modified by: Pooria Kaviani, Mehrdad Bakhtiari, MohammadReza Montazeri Shatoori
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#include <cmath>
#include <cassert>

#include <Basics.h>
#include <Degree.h>
#include <Logger.h>
#include <Command.h>
#include <HeadDecision.h>
#include <AdvancedAgent.h>
#include <Share.h>

using namespace std;
using namespace Degree;
using namespace Basics;

#define LOGH LOG

// HeadDecision functions
HeadDecision::HeadDecision(AdvancedAgent *owner) :
		notConstOwner(owner), owner(owner)
{
	for (unsigned i = 0; i < VIEW_PARTS_NUM; i++)
		viewPartObjectsNums[i] = 0;

	turnNeckCommand = NULL;
	changeViewCommand = NULL;

	worldModel = &owner->getWorldModel();
}

HeadDecision::~HeadDecision()
{
}

Command *HeadDecision::getTurnNeckCommand()
{
	return turnNeckCommand;
}

Command *HeadDecision::getChangeViewCommand()
{
	return changeViewCommand;
}

void HeadDecision::decide(const Command *bodyCycleCommand, const HeadDecisionForm &headForm)
{
	// synchronizing

	// normal section
	float nextViewAngle = 0;
	ViewModeWidth nextWidth = VMW_NORMAL;
	TurnNeckMode tnMode = headForm.tnMode;

	LOGH << "HeadDecision::decide" << endl;
	LOGH << "\tTurnNeckMode: " << headForm.tnMode << endl;

	nextBody = worldModel->getBody();
	LOGH << "current cycle dir = " << nextBody.getBodyDir() << endl;
	nextBody.simulateByAction(bodyCycleCommand);
	nextBody.simulateByDynamics();
	LOGH << "next cycle dir = " << nextBody.getBodyDir() << endl;

	nextBall = worldModel->getBall();
	nextBall.simulateByAction(worldModel->getBody(), bodyCycleCommand);
	nextBall.simulateByDynamics(nextBody, 1, BS_NONE, worldModel->getCurTime());

	if (tnMode == TNM_AUTO_PLAY_ON)
	{
		if (worldModel->getBall().getAbsVec().getMagnitude() > 7.5)
			tnMode = TNM_LOOK_NORMALLY;
		else
			tnMode = TNM_LOOK_CAREFULLY;
	}

//	By Pooria
	/*	if (nextBall.getAbsVec().getMagnitude() < worldModel->getBody().getVisibleDistance())
	 {
	 if (tnMode == TNM_LOOK_CAREFULLY_TO_BALL or tnMode == TNM_LOOK_NORMALLY_TO_BALL)
	 tnMode = TNM_LOOK_CAREFULLY;
	 }
	 */
	assert(tnMode != TNM_NONE);
	TurnNeckMode lastMode = TNM_NONE;

	while (lastMode != tnMode)
	{
		lastMode = tnMode;

		switch (tnMode)
		{
		case TNM_LOOK_NORMALLY:
			nextWidth = owner->getSyncedNormal();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			setWeightsForLookNormally();
			break;

		case TNM_LOOK_CAREFULLY:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			setWeightsForLookCarefully();
			break;

		case TNM_LOOK_CAREFULLY_TO_BALL:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookCarefullyToBall(tnMode);
			//decideForLookToDir(tnMode, worldModel->getBall().getAbsVec().getDirection());
			break;

		case TNM_LOOK_NORMALLY_TO_BALL:
			nextWidth = owner->getSyncedNormal();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookNormallyToBall(tnMode);
			//decideForLookToDir(
			//	tnMode,
			//(worldModel->getBall().getPos() - worldModel->getBody().getPos()).getDirection());
			break;

		case TNM_LOOK_CAREFULLY_TO_OPP_GOALIE:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookCarefullyToOppGoalie(tnMode);
			break;

		case TNM_LOOK_NORMALLY_TO_OUR_GOALIE:
			nextWidth = owner->getSyncedNormal();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookNormallyToOurGoalie(tnMode);
			break;

		case TNM_LOOK_CAREFULLY_TO_OPP_PLAYER:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookCarefullyToOppPlayer(tnMode, headForm.attentionNum);
			break;

		case TNM_LOOK_CAREFULLY_TO_OPP_AND_BALL:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookCarefullyToOppAndBall(tnMode, headForm.attentionNum);
			break;

		case TNM_LOOK_OFFENSIVE_INTERCEPT:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForOffensiveIntercept(tnMode);
			break;

		case TNM_LOOK_OFFENSIVE:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForOffensiveLook(tnMode);
			//setWeightsForLookOffensive();
			break;

		case TNM_LOOK_TO_RECEIVER:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookToReceiver(tnMode, headForm.attentionNum);
			break;

		case TNM_SEARCH:
			nextWidth = owner->getSyncedNormal();
			turnNeckCommand = new TurnNeckCommand(0.f);
			break;

		case TNM_LOOK_TO_DIR_NARROW:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookToDir(headForm.direction);
			break;

		case TNM_LOOK_TO_TMM_AND_BALL:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookToTmmAndBall(tnMode, headForm.attentionNum);
			break;

		case TNM_LOOK_TO_DIR_NORMAL:
			nextWidth = owner->getSyncedNormal();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookToDir(headForm.direction);
			break;

		case TNM_LOOK_TO_DIR_WIDE:
			nextWidth = owner->getSyncedWide();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookToDir(headForm.direction);
			break;

		case TNM_LOOK_TO_BALL:
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookToBall(nextWidth);
			break;

		case TNM_LOOK_TO_POINT_NARROW:
			nextWidth = owner->getSyncedNarrow();
			nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
			decideForLookToDir((headForm.lookPoint - nextBody.getPos()).getDirection());
			break;

		default:
			assert(0);
		}
	}

	//in bara ine ke age mitoonim ba vmw behtar bebinim ba hamoon bebinim
	if ((worldModel->getSeeTime() == worldModel->getCurTime() - 2 and Share::lastVMW == VMW_WIDE)
			or (worldModel->getSeeTime() == worldModel->getCurTime() - 1
					and Share::lastVMW == VMW_NORMAL))
		nextWidth = Share::lastVMW;
	changeViewCommand = new ChangeViewCommand(VMQ_HIGH, nextWidth);
	Share::lastVMW = nextWidth;
	if (tnMode == TNM_LOOK_NORMALLY
			|| tnMode == TNM_LOOK_CAREFULLY/* || tnMode == TNM_LOOK_OFFENSIVE*/)
		decideForTurnNeckCommand(nextViewAngle);
	if (0)
		logVariables();
}

void HeadDecision::decideForLookCarefullyToBall(TurnNeckMode &tnMode)
{
	LOGH << "HeadDecision::decideForLookCarefullyToBall" << endl;
	bool p = 0;
	if (worldModel->getCurCycle() % 2 == 0)
	{
		p = 1;
		LOG << "Should Change Direction" << endl;
	}
	float dir[2];
	dir[0] = dir[1] = nextBall.getAbsVec().getDirection();
	dir[0] += 20; //20 = 60/2 - 10.10 is for safety
	dir[1] -= 20;
	if (!nextBody.canLookToDir(dir[p], VMW_NARROW) and nextBody.canLookToDir(dir[!p], VMW_NARROW))
	{
		p = !p;
		LOGH << "Majbooram Az Ye Taraf Negah Konam" << endl;
	}
	LOG << "next Direction " << dir[p] << endl;
	decideForLookToDir(dir[p]);
//	float dir = normalizeAngle(nextBall.getHeadVec().getDirection() + 20 * p);
//	if (fabs(dir + worldModel->getBody().getRelativeHeadDir()) > 180.0)
//		dir = fabs(dir) / dir * 180 * -1;
//	else if (fabs(dir + worldModel->getBody().getRelativeHeadDir()) > 90.0)
//		dir = fabs(dir) / dir * 180;
//	LOGH << "\tdir : " << dir << endl;
//	turnNeckCommand = new TurnNeckCommand(dir);
}

void HeadDecision::decideForLookToDir(float direction)
{
	LOGH << "HeadDecision::decideForLookToDir" << endl;
	float dir;
//	dir = direction - worldModel->getBody().getHeadDir();
//	dir = normalizeAngle(dir);
//	if (fabs(dir + worldModel->getBody().getRelativeHeadDir()) > 180.0)
//		dir = fabs(dir) / dir * 180 * -1;
//	else
//		if (fabs(dir + worldModel->getBody().getRelativeHeadDir()) > 90.0)
//			dir = fabs(dir) / dir * 180;
//	dir = normalizeAngle(dir);
//	LOGH << "\tdir : " << dir << endl;
	if (fabs(Degree::getDeltaAngle(nextBody.getBodyDir(), direction)) <= 90)
	{
		dir = Degree::getDeltaAngle(direction, nextBody.getHeadDir());
	}
	else
	{
		LOGH << "Can't Look Directly To Direction" << endl;
		if (fabs(Degree::getDeltaAngle(nextBody.getBodyDir() + 90, direction))
				< fabs(Degree::getDeltaAngle(nextBody.getBodyDir() - 90, direction)))
			direction = nextBody.getBodyDir() + 90;
		else
			direction = nextBody.getBodyDir() - 90;
		dir = Degree::getDeltaAngle(direction, nextBody.getHeadDir());
	}
	if (dir == 180 and direction < nextBody.getHeadDir()) //Degree ino 180 mide eshtebahi chon fekr mikone mohem nist va 180 = -180
		dir = -180;
	turnNeckCommand = new TurnNeckCommand(dir);
}

void HeadDecision::decideForLookToBall(ViewModeWidth &width)
{
	LOG << "Decide For Look To Ball" << endl;
	int angle = 0;
	float deltaDir = fabs(
			Degree::getDeltaAngle(nextBody.getBodyDir(), nextBall.getAbsVec().getDirection()));
	LOG << "Delta Dir : " << deltaDir << endl;
	int margin = 10;
	decideForLookToDir(nextBall.getAbsVec().getDirection());
	while (true)
	{
		angle += 60;
		if (deltaDir < (angle / 2) - margin + 90) //90 is maximum deltaDir between body and head
		{
			angle = min(angle, 180); //chon momkene margin berine
			LOG << "Found The Best Width : " << angle << endl;
			width = nextBody.viewAngleToViewWidth(angle / 2);
			return;
		}
	}
}

void HeadDecision::decideForLookCarefullyToOppGoalie(TurnNeckMode &tnMode)
{
	LOGH << "\tHeadDecision::decideForLookCarefullyToOppGoalie" << endl;

	float dir;
	Player nextOppGoalie;

	if (worldModel->getOppGoalie())
	{
		nextOppGoalie = *worldModel->getOppGoalie();
		nextOppGoalie.simulateByDynamics(nextBody);
		dir = nextOppGoalie.getHeadVec().getDirection();
	}
	else
	{
		//tnMode = TNM_LOOK_CAREFULLY;
		dir = (Vector(50, 0) - worldModel->getBody().getPos()).getDirection();
		dir -= worldModel->getBody().getHeadDir();
		//return;
	}

	dir = normalizeAngle(dir);
	LOGH << "\tdir : " << dir << endl;
	turnNeckCommand = new TurnNeckCommand(dir);
}

// added by Mehrdad
void HeadDecision::decideForLookNormallyToOurGoalie(TurnNeckMode &tnMode)
{
	LOGH << "\tHeadDecision::decideForLookNormallyToOurGoalie" << endl;

	float dir;
	Player nextTmmGoalie;

	if (worldModel->getTmmGoalie())
	{
		nextTmmGoalie = *worldModel->getTmmGoalie();
		nextTmmGoalie.simulateByDynamics(nextBody);
		dir = nextTmmGoalie.getHeadVec().getDirection();
	}
	else
	{
		Vector goal(-52.5, 0.f);
		dir = (goal - worldModel->getBody().getPos()).getDirection()
				- worldModel->getBody().getHeadDir();
		LOGH << "\tgoalie is not valid, dir : " << dir << endl;
	}

	dir = normalizeAngle(dir);
	LOGH << "\tdir : " << dir << endl;
	if (fabs(dir) > 125.f) // dar haghighat bishtare va dar oon soorat nemitoone dorost bebine
		tnMode = TNM_LOOK_CAREFULLY;
	else
		turnNeckCommand = new TurnNeckCommand(dir);
}

void HeadDecision::decideForLookCarefullyToOppPlayer(TurnNeckMode &tnMode, unsigned attentionNum)
{
	LOGH << "\tHeadDecision::decideForLookCarefullyToOppPlayer" << endl;

	float dir;
	Player nextOppPlayer;

	if (worldModel->getFullPlayer(TID_OPPONENT, attentionNum - 1).isValid())
	{
		LOGH << "\tfullPlayer(attentionNum) is valid" << endl;
		nextOppPlayer = worldModel->getFullPlayer(TID_OPPONENT, attentionNum - 1);
		LOGH << "\tfullPlayer(attentionNum).getPos : "
				<< worldModel->getFullPlayer(TID_OPPONENT, attentionNum - 1).getPos() << endl;
		nextOppPlayer.simulateByDynamics(nextBody);
		dir = nextOppPlayer.getHeadVec().getDirection();
	}
	else
	{
		LOGH << "\tfullPlayer(attentionNum) is not valid" << endl;
		tnMode = TNM_LOOK_CAREFULLY;
		return;
	}

	dir = normalizeAngle(dir);
	LOGH << "\tdir : " << dir << endl;
	turnNeckCommand = new TurnNeckCommand(dir);
}

// added by Mehrdad
void HeadDecision::decideForLookCarefullyToOppAndBall(TurnNeckMode &tnMode, unsigned attentionNum)
{
	LOGH << "\t HeadDecision::decideForLookCarefullyToOppAndBall" << endl;
	Player nextOppPlayer;

	ViewModeWidth vmw = VMW_NARROW;
	if (attentionNum == 0 or attentionNum > 11
			or !worldModel->getFullPlayer(TID_OPPONENT, attentionNum - 1).isValid())
	{
		LOGH << "yaroo valid nis, LOOK TO ball NARROW" << endl;
		decideForLookToDir((nextBall.getPos() - nextBody.getPos()).getDirection());
//		decideForLookToBall(vmw);
		return;
	}
	else
	{
		if (worldModel->getTimer().getCycle() % 2 == 0)
		{
			LOGH << "Look to player" << endl;
			nextOppPlayer = worldModel->getFullPlayer(TID_OPPONENT, attentionNum - 1);
			nextOppPlayer.simulateByDynamics(nextBody);
			decideForLookToDir((nextOppPlayer.getPos() - nextBody.getPos()).getDirection());
		}
		else
		{
			LOGH << "Look to ball" << endl;
			decideForLookToDir((nextBall.getPos() - nextBody.getPos()).getDirection());
//			decideForLookToBall(vmw);
		}
	}
}

void HeadDecision::decideForLookToTmmAndBall(TurnNeckMode &tnMode, unsigned attentionNum)
{
	LOGH << "\t HeadDecision::decideForLookToTmmAndBall" << endl;

	float dir;
	Player nextOppPlayer;

	if (!worldModel->getFullPlayer(TID_TEAMMATE, attentionNum - 1).isValid())
	{
		LOGH << "yaroo valid nis, TNM_LOOK_CAREFULLY" << endl;
		tnMode = TNM_LOOK_CAREFULLY;
		return;
	}
	else
	{
		if (worldModel->getTimer().getCycle() % 2 == 0)
		{
			LOGH << "Look to player" << endl;
			nextOppPlayer = worldModel->getFullPlayer(TID_TEAMMATE, attentionNum - 1);
			nextOppPlayer.simulateByDynamics(nextBody);
			dir = nextOppPlayer.getHeadVec().getDirection();
		}
		else
		{
			LOGH << "Look to ball" << endl;
			dir = normalizeAngle(nextBall.getHeadVec().getDirection());
		}
	}
	dir = normalizeAngle(dir);
	LOGH << "\tdir : " << dir << endl;
	turnNeckCommand = new TurnNeckCommand(dir);
}

void HeadDecision::decideForLookNormallyToBall(TurnNeckMode &tnMode)
{
	LOGH << "\tHeadDecision::decideForLookNormallyToBall" << endl;
	bool p = 0;
	if ((worldModel->getCurCycle() / 2) % 2 == 0)
	{
		p = 1;
		LOGH << "Should Change Direction" << endl;
	}
	float dir[2];
	dir[0] = dir[1] = nextBall.getAbsVec().getDirection();
	dir[0] += 50;
	dir[1] -= 50;
	if (!nextBody.canLookToDir(dir[p], VMW_NORMAL) and nextBody.canLookToDir(dir[!p], VMW_NORMAL))
	{
		p = !p;
		LOGH << "Majbooram Az Ye Taraf Negah Konam" << endl;
	}
	LOG << "next Direction " << dir[p] << endl;
	decideForLookToDir(dir[p]); //50 = 120/2 - 10. 10 is for safety
//
//	float dir;
//	if (worldModel->getBall().isValid())
//	{
//		float headDirRight = nextBall.getAbsVec().getDirection() + (nextViewAngle - 15);
//		float headDirLeft = nextBall.getAbsVec().getDirection() - (nextViewAngle - 15);
//
//		if (abs(Degree::getDeltaAngle(nextBody.getBodyDir(), headDirLeft)) < 90
//				or abs(Degree::getDeltaAngle(nextBody.getBodyDir(), headDirRight)) < 90)
//		{
//			if (worldModel->getSeeDeltaCycle(headDirRight/*, 0.2*/)
//					> worldModel->getSeeDeltaCycle(headDirLeft/*, 0.2*/))
//				dir = headDirRight;
//			else
//				dir = headDirLeft;
//		}
//		else
//		{
//			tnMode = TNM_LOOK_NORMALLY;
//			return;
//		}
//
//		dir = Degree::getDeltaAngle(dir, nextBody.getBodyDir());
//		dir = Degree::getDeltaAngle(dir, nextBody.getRelativeHeadDir());
//	}
//	else
//		dir = -nextBody.getRelativeHeadDir();
//
//	dir = normalizeAngle(dir);
//	LOGH << "\tdir : " << dir << endl;
//	turnNeckCommand = new TurnNeckCommand(dir);
}

void HeadDecision::decideForTurnNeckCommand(float nextViewAngle)
{
	float bodyHeadDir;
	float turnNeckAngle;
	map<float, float> turnNeckWeights;
	//float turnNeckWeights[4];
	unsigned i, j;

	updateViewPartObjectsNum();
	for (i = 0; i < VIEW_PARTS_NUM; i++)
		viewPartWeights[i] = getViewPartWeight(i);
//	LOGH << "bodyDir = " << nextBody.getBodyDir() << endl;
	for (float dir = -82.5f; dir <= 82.5f; dir += 55.f)
	{
		turnNeckWeights[dir] = 0;
		bodyHeadDir = nextBody.getBodyDir()/* - 90 */+ dir;
//		LOGH << "JIIGH: " << dir << endl;
//		LOGH << "BOOGH: " << bodyHeadDir << endl;

		for (j = 0; j < VIEW_PARTS_NUM; j++)
		{
			if (fabs(getDeltaAngle(worldModel->getViewPartDir(j), bodyHeadDir))
					< nextViewAngle - VIEW_PART_SIZE / 2.f)
			{
				//LOGH << "Delta Angle = " << normalizeAngle(worldModel->getViewPartDir(j)) << endl;
				turnNeckWeights[dir] += viewPartWeights[j]/* * viewPartWeights[j]*/;
				//LOGH << "Weight = " << viewPartWeights[j]/* * viewPartWeights[j]*/<< endl;
			}
		}
	}
	/*for (i = 0; i <= 180.0 / TURN_NECK_RESOLUTION; i++)
	 {
	 turnNeckWeights[i] = 0;
	 bodyHeadDir = nextBody.getBodyDir() - 90 + i * TURN_NECK_RESOLUTION;

	 LOGH << "BOOGH: " << bodyHeadDir << endl;

	 for (j = 0; j < VIEW_PARTS_NUM; j++)
	 {
	 if (fabs(getDeltaAngle(worldModel->getViewPartDir(j), bodyHeadDir)) < nextViewAngle - VIEW_PART_SIZE / 2.f)
	 {
	 LOGH << "Delta Angle = " << normalizeAngle(worldModel->getViewPartDir(j)) << endl;
	 turnNeckWeights[i] += viewPartWeights[j] * viewPartWeights[j];
	 LOGH << "Weight = " << viewPartWeights[j] * viewPartWeights[j] << endl;
	 }
	 }
	 }
	 */LOGH << "Turn Neck Weight [" << -82.5 << "] = " << turnNeckWeights[-82.5] << endl;
	float turnNeckWeightFlag = -82.5;
	for (float dir = -27.5f; dir <= 82.5f; dir += 55.f)
	{
		LOGH << "Turn Neck Weight [" << dir << "] = " << turnNeckWeights[dir] << endl;
		if (turnNeckWeights[dir] > turnNeckWeights[turnNeckWeightFlag])
			turnNeckWeightFlag = dir;
	}

	turnNeckAngle = turnNeckWeightFlag;

	LOGH << "Turn Neck Angle = " << turnNeckAngle << endl;
	turnNeckCommand = new TurnNeckCommand(turnNeckAngle - nextBody.getRelativeHeadDir());/*Relative*/
}

void HeadDecision::decideForOffensiveIntercept(TurnNeckMode &tnMode)
{
	LOGH << "HeadDecision::decideForOffensiveIntercept" << endl;

	float dir = normalizeAngle(nextBall.getHeadVec().getDirection());
	LOGH << "\tBall Dir : " << dir << endl;
	dir = (worldModel->getCurCycle() % 2 == 1) ? dir + 25 : dir - 25;
	LOGH << "\tDir : " << dir << endl;
	turnNeckCommand = new TurnNeckCommand(dir);
}

void HeadDecision::decideForLookToReceiver(TurnNeckMode &tnMode, unsigned attentionNum)
{
	LOGH << "\t HeadDecision::decideForLookToReceiver" << endl;

	float dir;

	if (!worldModel->getFullPlayer(TID_TEAMMATE, attentionNum - 1).isValid())
	{
		LOGH << "yaroo valid nis, TNM_LOOK_CAREFULLY" << endl;
		tnMode = TNM_LOOK_CAREFULLY;
		return;
	}
	else
	{
		int p = -1;
		if (worldModel->getCurCycle() % 2 == 0)
			p = 1;
		worldModel->getFullPlayer(TID_TEAMMATE, attentionNum - 1);
		dir = worldModel->getFullPlayer(TID_TEAMMATE, attentionNum - 1).getHeadVec().getDirection()
				+ 20 * p;
	}
	dir = normalizeAngle(dir);
	LOGH << "\tdir : " << dir << endl;
	turnNeckCommand = new TurnNeckCommand(dir);
}

void HeadDecision::decideForOffensiveLook(TurnNeckMode &tnMode)
{
	LOGH << "\tHeadDecision::decideForOffensiveLook" << endl;

//	float dir;

	string players = "AB9";
	/*if (worldModel->getGlobalFastIC().isSelfFastestPlayer())
	 {
	 LOGH << "\tSelf Fastest!" << endl;
	 for (unsigned i = 0;i < players.size();i++)
	 {
	 if (uniNumToChar(worldModel->getBody().getUniNum()) == players[i])
	 {
	 LOGH << "\tIt's MYSELF!" << endl;
	 continue;
	 }
	 if (worldModel->getFullPlayer(TID_TEAMMATE, charToUniNum(players[i]) - 1).getSimCounter() > 2)
	 {
	 Vector meToPlayer;
	 meToPlayer.setByPoints(worldModel->getBody().getPos(), worldModel->getFullPlayer(TID_TEAMMATE, charToUniNum(players[i]) - 1).getPos());
	 dir = meToPlayer.getDirection() - worldModel->getBody().getHeadDir();
	 LOGH << "Player " << charToUniNum(players[i]) << " has SimCounter ("
	 << worldModel->getFullPlayer(TID_TEAMMATE, charToUniNum(players[i]) - 1).getSimCounter() << ")" << endl;
	 if (fabs(getDeltaAngle(nextBody.getBodyDir(), nextBody.getHeadDir() + dir)) > 90.f)
	 {
	 continue;
	 }
	 LOGH << "\tdir : " << dir << endl;
	 turnNeckCommand = new TurnNeckCommand(dir);
	 return;
	 }
	 }
	 }
	 else if (worldModel->getGlobalFastIC().isOurTeamBallPossessor() and worldModel->getGlobalFastIC().getFastestTeammate())
	 {
	 LOGH << "Other Fastest!" << endl;
	 if (isInPlayers(worldModel->getGlobalFastIC().getFastestTeammate()->getUniNum(), players)
	 and worldModel->getGlobalFastIC().getFastestTeammate()->getSimCounter() > 1)
	 {
	 Vector meToPlayer;
	 meToPlayer.setByPoints(worldModel->getBody().getPos(), worldModel->getGlobalFastIC().getFastestTeammate()->getPos());
	 dir = meToPlayer.getDirection() - worldModel->getBody().getHeadDir();
	 LOGH << "\tdir : " << dir << endl;
	 turnNeckCommand = new TurnNeckCommand(dir);
	 return;
	 }
	 }
	 else*/
	{
		LOGH << "Look Carefully!" << endl;
		setWeightsForLookCarefully();
		tnMode = TNM_LOOK_CAREFULLY;
		return;
	}
	LOGH << "Look Offensive!!!" << endl;
	ViewModeWidth nextWidth = owner->getSyncedNarrow();
	float nextViewAngle = worldModel->getBody().viewWidthToViewAngle(nextWidth);
	setWeightsForLookOffensive();
	decideForTurnNeckCommand(nextViewAngle);
}

void HeadDecision::logVariables()
{
	LOGH << "\tHeadDecision View Parts:" << endl;

	for (unsigned i = 0; i < VIEW_PARTS_NUM; i++)
		LOGH << "\tVP[" << i << "] >" << " Ang:" << worldModel->getViewPartDir(i) << " Cyc:"
				<< worldModel->getViewPartCycle(i) << " ObN:" << viewPartObjectsNums[i] << " Wei:"
				<< viewPartWeights[i] << endl;
}

void HeadDecision::setWeightsForLookNormally()
{
	haveBallRate = 1.1;
	havePlayerRate = 1.2;
	objectsNumRate = 0.3;
	offenseRate = 1.0;
}

void HeadDecision::setWeightsForLookCarefully()
{
	haveBallRate = 1.15;
	havePlayerRate = 1.2;
	objectsNumRate = 0.3;
	offenseRate = 1.0;
}

void HeadDecision::setWeightsForLookOffensive()
{
	haveBallRate = 1.15;
	havePlayerRate = 1.2;
	objectsNumRate = 0.3;
	offenseRate = 1.1;
}

void HeadDecision::updateViewPartObjectsNum()
{
	float deltaAngle;
	unsigned i, j;

	for (i = 0; i < VIEW_PARTS_NUM; i++)
	{
		viewPartObjectsNums[i] = 0;

		// Counting full players.
		for (j = 0; j < FULL_PLAYERS_NUM; j++)
		{
			if (worldModel->getFullPlayer(TID_TEAMMATE, j).isAlive()
					&& !worldModel->getFullPlayer(TID_TEAMMATE, j).isBody())
			{
				deltaAngle =
						abs(
								getDeltaAngle(
										worldModel->getFullPlayer(TID_TEAMMATE, j).getAbsVec().getDirection(),
										worldModel->getViewPartDir(i)));

				if (deltaAngle <= VIEW_PART_SIZE / 2)
					viewPartObjectsNums[i]++;
				//if (j + 1 == 9)
				//LOGH << "FULL unum = " << j + 1 << "angle = " << normalizeAngle(worldModel->getViewPartDir(i)) << " dAngle = " << deltaAngle << endl;
			}

			if (worldModel->getFullPlayer(TID_OPPONENT, j).isAlive())
			{
				deltaAngle =
						abs(
								getDeltaAngle(
										worldModel->getFullPlayer(TID_OPPONENT, j).getAbsVec().getDirection(),
										worldModel->getViewPartDir(i)));

				if (deltaAngle <= VIEW_PART_SIZE / 2)
					viewPartObjectsNums[i]++;
			}
		}

		// Counting half players.
		for (j = 0; j < HALF_PLAYERS_NUM; j++)
		{
			if (worldModel->getHalfPlayer(TID_TEAMMATE, j).isAlive())
			{
				deltaAngle =
						abs(
								getDeltaAngle(
										worldModel->getHalfPlayer(TID_TEAMMATE, j).getAbsVec().getDirection(),
										worldModel->getViewPartDir(i)));
//				Vector meToHalf = worldModel->getHalfPlayer(TID_TEAMMATE,j).getPos() - worldModel->getBody().getPos();

				if (deltaAngle <= VIEW_PART_SIZE / 2)
					viewPartObjectsNums[i]++;
				/*if (deltaAngle < 30.f)
				 LOGH << "HALF unum = " << j + 1 << "angle = " << normalizeAngle(worldModel->getViewPartDir(i)) << " dAngle = " << deltaAngle << " pure angle = "
				 << worldModel->getHalfPlayer(TID_TEAMMATE,j).getAbsVec().getDirection() << " must be = " << worldModel->getHalfPlayer(TID_TEAMMATE,j).getPos() << endl;*/
			}

			if (worldModel->getHalfPlayer(TID_OPPONENT, j).isAlive())
			{
				deltaAngle =
						abs(
								getDeltaAngle(
										worldModel->getHalfPlayer(TID_OPPONENT, j).getAbsVec().getDirection(),
										worldModel->getViewPartDir(i)));

				if (deltaAngle <= VIEW_PART_SIZE / 2)
					viewPartObjectsNums[i]++;
			}
		}

		// Counting quarter players.
		for (j = 0; j < QUARTER_PLAYERS_NUM; j++)
			if (worldModel->getQuarterPlayer(j).isAlive())
			{
				deltaAngle = abs(
						getDeltaAngle(worldModel->getQuarterPlayer(j).getAbsVec().getDirection(),
								worldModel->getViewPartDir(i)));

				if (deltaAngle <= VIEW_PART_SIZE / 2)
					viewPartObjectsNums[i]++;
			}

		// Counting the ball.
		if (nextBall.getAbsVec().getMagnitude() > worldModel->getBody().getVisibleDistance())
		{
			deltaAngle = abs(
					getDeltaAngle(nextBall.getAbsVec().getDirection(),
							worldModel->getViewPartDir(i)));

			if (deltaAngle <= VIEW_PART_SIZE / 2)
				viewPartObjectsNums[i]++;
		}

		// Counting the opponent goal
		if (worldModel->getBall().getAbsVec().getMagnitude() < 5
				&& worldModel->getBody().getPos().getX() > 30)
		{
			Vector vec;

			vec.setByPoints(worldModel->getBody().getPos(), Point(52, 5.5));
			deltaAngle = abs(getDeltaAngle(vec.getDirection(), worldModel->getViewPartDir(i)));
			if (deltaAngle <= VIEW_PART_SIZE / 2)
				viewPartObjectsNums[i]++;

			vec.setByPoints(worldModel->getBody().getPos(), Point(52, -5.5));
			deltaAngle = abs(getDeltaAngle(vec.getDirection(), worldModel->getViewPartDir(i)));
			if (deltaAngle <= VIEW_PART_SIZE / 2)
				viewPartObjectsNums[i]++;
		}

		// Counting the forward angle
		if (worldModel->getBall().getAbsVec().getMagnitude() < 3
				&& worldModel->getBody().getPos().getX() < 35)
		{
			Vector vec;

			vec.setAsCartesian(10, 0);
			deltaAngle = abs(getDeltaAngle(vec.getDirection(), worldModel->getViewPartDir(i)));
			if (deltaAngle <= VIEW_PART_SIZE / 2)
				viewPartObjectsNums[i]++;
		}
	}
}

float HeadDecision::getViewPartWeight(unsigned viewPartNum)
{
	float weight;
	float deltaAngle;
	Vector oppGoalUpVec;
	Vector oppGoalDownVec;
//	LOGH << "ang = " << normalizeAngle(worldModel->getViewPartDir(viewPartNum)) << endl;
	// Dont see weight
	weight = min(worldModel->getViewPartCycle(viewPartNum), 5u) + 1;
//	LOGH << "1-weights += " << weight << endl;
	// HaveBall weight
	deltaAngle = abs(
			getDeltaAngle(worldModel->getViewPartDir(viewPartNum),
					nextBall.getAbsVec().getDirection()));
//	float ballRate = reRate(deltaAngle / VIEW_PART_SIZE, 0.f, 10.f, 0.f, 1.f, RRM_REVERSE);
	//if (deltaAngle <= VIEW_PART_SIZE * 4) // SenseArea
	//	weight *= haveBallRate * ballRate;
	if (worldModel->getBody().getDistance(worldModel->getBall().getPos()) > 2.f
			and deltaAngle < VIEW_PART_SIZE / 2.f)
		weight *= haveBallRate /* * ballRate*/;
	if ((worldModel->getBall().getPos().getX() > 48.f
			and fabs(getDeltaAngle(normalizeAngle(worldModel->getViewPartDir(viewPartNum)), 0.f))
					< 40.f)
			or (worldModel->getBall().getPos().getY() > 27.5f
					and fabs(
							getDeltaAngle(normalizeAngle(worldModel->getViewPartDir(viewPartNum)),
									90.f)) < 40.f)
			or (worldModel->getBall().getPos().getY() < -27.5f
					and fabs(
							getDeltaAngle(normalizeAngle(worldModel->getViewPartDir(viewPartNum)),
									-90.f)) < 40.f))
	{
		//LOGH << "Be Out Niga Nakon." << endl;
		weight *= 0.3f;
	}

//	LOGH << "2-weights = " << weight << endl;
	// HavePlayer weight
	/*if (viewPartObjectsNums[viewPartNum] > 0)
	 weight *= havePlayerRate;
	 else
	 weight *= 0.1f;*/
//	LOGH << "3-weights = " << weight << endl;
	// ObjectsNum weight
	//weight += viewPartObjectsNums[viewPartNum] * objectsNumRate;
//	LOGH << "4-weights = " << weight << endl;
	/*Vector toUp, toDown;
	 toUp.setByPoints(worldModel->getBody().getPos(), Point(52.5, 34.f));
	 toDown.setByPoints(worldModel->getBody().getPos(), Point(52.5, -34.f));
	 if (worldModel->getBody().getPos().getX() < 30.f)
	 {
	 if (isBetween(toUp.getDirection(), toDown.getDirection(), worldModel->getViewPartDir(viewPartNum)))
	 weight *= offenseRate;
	 }*/

	return weight;
}
