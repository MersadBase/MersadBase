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
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Logger.h>
#include <Shoot.h>
#include <Ball.h>
#include <Degree.h>
#include <cmath>
#include <Command.h>
#include <BasicTurn.h>
#include <BasicKick.h>
#include <BasicDash.h>
#include <Intercept.h>

using namespace std;

Shoot::Shoot(const WorldModel *worldModel): wm(worldModel)
{
}

Shoot::~Shoot()
{
}

bool Shoot::execute()
{
	LOG << "Shoot::execute" << endl;
	Point centerOfGoal(52.f, 0.f);
	if (wm->getBall().getPos().getDistance(centerOfGoal) > 20)
	{
		LOG << "\tgoal is so far" << endl;
		return 0;
	}

	fastIC = new FastIC(wm);

	for (int i = 0;i < 11;i++)
	{
		int delay = 3;
		const Player * opp = &wm->getFullPlayer(TID_OPPONENT, i);
		if (opp->isValid())
		{
			float kickable = 0.9f;
			fastIC->addPlayer(opp, delay, kickable);
		}
	}
	// comment
	fastIC->setMaxCycles(60);
	fastIC->setMaxOpponentCount(1);
	fastIC->setMaxCycleAfterFirstFastestPlayer(5);

	for (float goalY = 7 ; goalY >= -7 ; goalY -= 0.5)
	{
		Point target (52.5 , goalY);

		Vector checkSpeed;
		checkSpeed.setAsPolar(2.9, Vector(target - wm->getBall().getPos()).getDirection());
		fastIC->setBall(wm->getBall().getPos(), checkSpeed);
		fastIC->refresh();
		fastIC->calculate();

		if (fastIC->getFastestOpponentReachPoint().x < 52.5)
		{
			continue;
		}
		else
		{
			LOG << "i've found a good target for shoot, " << target << endl;
			command = FastKickToPoint(target, wm->getBall(), wm->getBody()).getCommand();
			return true;
		}
	
	}
	LOG << "shoot value is false" << endl;
	return false;
}

Command* Shoot::getCommand()
{
	return command;
}

