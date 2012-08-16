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
 *  This file is created by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 9 July 2010, 17 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Penalty.h>
#include <cmath>
#include <Vector.h>
#include <BasicKick.h>
#include <Command.h>
#include <FastIC.h>
#include <Intercept.h>
#include <Logger.h>
#include <Rectangle.h>
#include <vector>
#include <Dribble.h>
#include <BasicDash.h>
#include <WorldModel.h>
#include <Tackle.h>
#include <BasicTackle.h>
#include <Basics.h>
#include <Degree.h>

using namespace std;
using namespace Degree;
using namespace Basics;

void Penalty::execute(Form& form)
{
	const float dist_thr = 1.0, dir_thr = 15, dash_power = 90;
	Point homePos = getPos(worldModel->getBody().getUniNum());
	LOG << "my homePos : " << homePos << endl;

	command = new EmptyCommand();

	LOG << "ourKicker : " << worldModel->getOurPenaltyKicker() << endl;
	switch (worldModel->getPlayMode())
	{
	case PM_PENALTY_SETUP:
	case PM_PENALTY_READY:
	{
		LOG << "its our turn" << endl;
		if (worldModel->getOurPenaltyKicker() == worldModel->getBody().getUniNum())
			// decide for kicker
			doKicker(form);
		else
			// decide for other players
			command = PerfectGotoPoint(homePos, worldModel->getBody(), dist_thr, dash_power).getCommand();
	}
		break;
	case PM_PENALTY_SETUP_OPP:
	case PM_PENALTY_READY_OPP:
	default:
		if (worldModel->getBody().isGoalie())
			homePos = Point(-40, 0.f);

		if (worldModel->getBody().isGoalie() and worldModel->getVirtualPlayMode() == VPM_PENALTY_TAKEN_OPP)
			// decide for goalie
			obtainBall(form);
		else
			// decide for other players
			command = PerfectGotoPoint(homePos, worldModel->getBody(), dist_thr, dash_power).getCommand();
		break;
	}
}

void Penalty::doKicker(Form& form)
{
	if (worldModel->isBallKickable() and worldModel->getPlayMode() == PM_PENALTY_READY)
	{
		LOG << "ball is in kickable area" << endl;
		// **** it's your task to decide about kicker!
	}
	else
	{
		LOG << "return intercept" << endl;
		Intercept intercept = Intercept(worldModel);
		intercept.getValue();
		intercept.execute(form);
		command = intercept.getCommand();
	}
}

void Penalty::obtainBall(Form& form)
{
	if (worldModel->isBallKickable())
	{
		command = FastKickToPoint(Point(0.0, 0.0), worldModel->getBall(), worldModel->getBody()).getCommand();
	}
	else
	{
		// A simple action is intercepting ball
		LOG << "return intercept" << endl;
		Intercept intercept = Intercept(worldModel);
		intercept.getValue();
		intercept.execute(form);
		command = intercept.getCommand();
	}
}

Point Penalty::getPos(unsigned num)
{
	if (num == 1)
		return Point(-54, 25);
	Point pos;
	pos.x = -5 + (int)num - 1;
	pos.y = num % 2? 5 : -5;
	return pos;
}

Command* Penalty::getCommand()
{
	return command;
}

