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
 *  	and is modified by: Mohammad Zareie, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cassert>
#include <cmath>
#include <Logger.h>
#include <Types.h>
#include <Command.h>
#include <BasicTurn.h>
#include <BasicKick.h>
#include <BasicDash.h>
#include <NonPlayOnDecision.h>
#include <Pass.h>
#include <Intercept.h>
#include <Basics.h>
#include <Rectangle.h>
#include <Penalty.h>

#include <DecideCommonTypes.h>

#define BEFORE_KICK_OFF_POINT Point(-100, -100)
#define VPM_GOALIE_POINT Point(-90, -100)

using namespace std;
using namespace Basics;

NonPlayOnDecision::NonPlayOnDecision(const WorldModel *worldModel): worldModel(worldModel)
{
}

NonPlayOnDecision::~NonPlayOnDecision()
{
}

void NonPlayOnDecision::decide(Form &form)
{
	command = new EmptyCommand();

	unsigned kicker = 0;
	float minDistance = 1000;
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		positions[i] = worldModel->getFormation().getExactPosition(i + 1, worldModel->getBall().getPos());
		if (positions[i].getDistance(worldModel->getBall().getPos()) < minDistance and i != 0)
		{
			minDistance = positions[i].getDistance(worldModel->getBall().getPos());
			kicker = i + 1;
		}
	}

	if (worldModel->getPlayMode() == PM_FREE_KICK and
		worldModel->getVirtualPlayMode() == VPM_GOALIE_CATCH_BALL)
	{
		kicker = 1;
		for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
			positions[i] = worldModel->getNonPlayOnFormation().getExactPosition(i + 1, VPM_GOALIE_POINT);
	}

	LOG << "PlayMode : " << worldModel->getPlayMode() << endl;
	LOG << "VirtualPlayMode : " << worldModel->getVirtualPlayMode() << endl;
	switch (worldModel->getPlayMode())
	{
	case PM_BEFORE_KICK_OFF:
	case PM_KICK_OFF_OPP:
	case PM_GOAL:
	case PM_GOAL_OPP:
	{
		Point movePoint(-1, 0);
//		if (worldModel->getBody().getUniNum() == 1)
//			movePoint = Point(-52.5, 0);
		movePoint = worldModel->getNonPlayOnFormation().getExactPosition(worldModel->getBody().getUniNum(), BEFORE_KICK_OFF_POINT);
		LOG << "Point: " << movePoint << endl;
		if (worldModel->getBody().getPos().getDistance(movePoint) > 0.01)
			command = new MoveCommand(movePoint);
		else
			command = TurnToPoint(Point(0, 0), worldModel->getBody()).getCommand();
	}
		break;

	case PM_KICK_OFF:
		if (worldModel->getPlayModeRemainCycle() < 180 and isStaminaEnough() and worldModel->getBody().getUniNum() == kicker)
			command = FastKickToPoint(worldModel->getNonPlayOnFormation().getExactPosition(9, BEFORE_KICK_OFF_POINT),
					worldModel->getBall(), worldModel->getBody()).getCommand();
		break;

	case PM_CORNER_KICK:
	case PM_FREE_KICK_FAULT_OPP:
	case PM_KICK_IN:
	case PM_BACK_PASS_OPP:
	case PM_OFFSIDE:
	case PM_FREE_KICK_FAULT:
	case PM_CATCH_FAULT:
	case PM_GOAL_KICK:
	case PM_FREE_KICK:
	case PM_FOUL_CHARGE_OPP:
	{
		Point point = positions[worldModel->getBody().getUniNum() - 1];
		if (worldModel->getBody().getUniNum() == kicker and kicker != 1)
				point = worldModel->getBall().getPos();
		LOG << "Point: " << point << endl;

		if (worldModel->getBody().getPos().getDistance(point) > 0.7)
		{
			if (worldModel->getBody().getUniNum() == kicker and kicker == 1)
				command = new MoveCommand(point);
			else
				command = DashNormalGotoPoint(point, 0.5, worldModel->getBody(), 30).getCommand();
		}
		else if (worldModel->getCurCycle() % 2 == 1)
			command = new TurnCommand(100);

		if (worldModel->getBody().getUniNum() == kicker and worldModel->isBallKickable() and isAllPlayersReady())
		{
			Pass pass(worldModel);
			if (pass.getValue() > 0)
			{
				pass.decide(form);
				command = pass.getCommand();
			}
		}
	}
		break;

	case PM_CORNER_KICK_OPP:
	case PM_KICK_IN_OPP:
	case PM_BACK_PASS:
	case PM_OFFSIDE_OPP:
	case PM_CATCH_FAULT_OPP:
	case PM_GOAL_KICK_OPP:
	case PM_FREE_KICK_OPP:
	case PM_FOUL_CHARGE:
	{
		Point point = positions[worldModel->getBody().getUniNum() - 1];
		LOG << "Point: " << point << endl;
		if (worldModel->getBody().getPos().getDistance(point) > 0.7 and
			worldModel->getBody().getStamina() >= 3000)
			command = DashNormalGotoPoint(point, 0.5, worldModel->getBody(), 60).getCommand();
		else
			command = new TurnCommand(80);
	}
		break;
	
	case PM_PENALTY_SETUP:
	case PM_PENALTY_SETUP_OPP:
	case PM_PENALTY_READY:
	case PM_PENALTY_READY_OPP:
	{
		LOG << "Penalty" << endl;
		Penalty penalty(worldModel);
		penalty.execute(form);
		command = penalty.getCommand();
	}

	case PM_TIME_UP_WITHOUT_A_TEAM:
	case PM_TIME_UP:
	default:
		break;
	}
}

Command* NonPlayOnDecision::getCommand()
{
	return command;
}

bool NonPlayOnDecision::isStaminaEnough()
{
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
		if (worldModel->getFullPlayer(TID_TEAMMATE, i).isValid() and
			worldModel->getFullPlayer(TID_TEAMMATE, i).getStamina() < 3999)
		{
			LOG << "Bad Stamina Player: " << i + 1 << endl;
			LOG << "D: " << worldModel->getFullPlayer(TID_TEAMMATE, i).getStaminaMax() << endl;
			return false;
		}

	return true;
}

bool NonPlayOnDecision::areAllPlayersReady()
{
	if (not isStaminaEnough())
		return false;

	return true;
}

bool NonPlayOnDecision::isAllPlayersReady()
{
	if (!isStaminaEnough())
		return false;

	for (unsigned i = 1; i < FULL_PLAYERS_NUM; i++)
		if (worldModel->getFullPlayer(TID_TEAMMATE, i).getDistance(positions[i]) > 2 and
			i + 1 != worldModel->getBody().getUniNum() and positions[i].getDistance(worldModel->getBody().getPos()) < 40)
	{
			LOG << "Bad Player: " << i + 1 << endl;
			LOG << "Distance: " << worldModel->getFullPlayer(TID_TEAMMATE, i).getDistance(positions[i]) << endl;
			return false;
	}

	return true;
}
