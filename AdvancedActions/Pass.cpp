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
#include <Logger.h>
#include <WorldModel.h>
#include <BasicKick.h>
#include <iostream>
#include <Degree.h>
#include <Basics.h>

using namespace std;
using namespace Degree;
using namespace Basics;

Pass::Pass(const WorldModel *worldModel):wm(worldModel), target(Point(0,0))
{
}

Pass::~Pass()
{
}

float Pass::getValue()
{
	string mySelf = "";
	mySelf += uniNumToChar(wm->getBody().getUniNum());
	mySelf += "1";
	LOG << "\tMy self is " << mySelf << endl;
	if (wm->getNearestTmmToPointFromNotList(wm->getBall().getPos(), mySelf))
	{
		target.x = wm->getNearestTmmToPointFromNotList(wm->getBall().getPos(), mySelf)->getPos().getX();
		target.y = wm->getNearestTmmToPointFromNotList(wm->getBall().getPos(), mySelf)->getPos().getY();
		LOG << "\tTarget.x Point is: " << target.x << endl;
		LOG << "\tTarget.y Point is: " << target.y << endl;
		if (target != NULL)
			return 1;
	}
		return 0;
}

void Pass::decide(Form &form)
{
	command = KickToPointArriveVelocity(target, 1, wm->getBall(), wm->getBody()).getCommand();
}


Command* Pass::getCommand()
{
	return command;
}