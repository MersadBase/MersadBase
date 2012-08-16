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


#include <Dribble.h>
#include <iostream>
#include <WorldModel.h>
#include <Logger.h>
#include <Ball.h>
#include <Degree.h>
#include <cmath>
#include <Command.h>
#include <BasicKick.h>
#include <BasicDash.h>

using namespace std;
using namespace Degree;

Dribble::Dribble(const WorldModel *worldModel):wm(worldModel)
{
}

Dribble::~Dribble()
{
}

void Dribble::decide(Form &form)
{
	targetPoint.x = 0;
	targetPoint.y = 0;
	if (wm->getBall().getPos().getX() > 0)
		targetPoint = Point(52.5, 0); 
	else
	{
		targetPoint.x = wm->getBody().getPos().getX() + 10;
		targetPoint.y = wm->getBody().getPos().getY();
	}
	command = KickToPointVelocity(targetPoint,0.6 ,wm->getBall(), 
		wm->getBody()).getCommand();
}

float Dribble::getValue()
{
	if (wm->getGlobalFastIC().getFastestOpponent())
	{
		if (wm->getNearestOppToPoint(wm->getBody().getPos())->getDistance(wm->getBody().getPos()) <= 5)
			return 0;
	}
	return 1;
}

Command* Dribble::getCommand()
{
	return command;
}
