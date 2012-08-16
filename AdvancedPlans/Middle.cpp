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
 *  	and is modified by: Mohammad Zareie
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Offense.h>
#include <Tackle.h>
#include <Middle.h>
#include <Command.h>
#include <Logger.h>
#include <Shoot.h>
#include <Tackle.h>
#include <Dribble.h>
#include <Pass.h>
#include <BasicTurn.h>
#include <BasicKick.h>
#include <BasicDash.h>
#include <Intercept.h>

using namespace std;

Middle::Middle(const WorldModel *worldModel): worldModel(worldModel)
{
}

Middle::~Middle()
{
}

void Middle::decide(Form &form)
{
	Tackle tackle(worldModel);
	if (worldModel->getGlobalFastIC().isSelfFastestTeammate())
	{
		if (worldModel->isBallKickable())
		{
	command = new EmptyCommand();
	return;

			Shoot shoot(worldModel);
			Dribble dribble(worldModel);
			Pass pass(worldModel);
			if (shoot.execute())
			{
				command = shoot.getCommand();
			}
			else if(dribble.getValue() >= pass.getValue())
			{
				dribble.decide(form);
				command = dribble.getCommand();
			}
			else
			{
				pass.decide(form);
				command = pass.getCommand();
			}
		}
		else
		{
			if(tackle.execute(form))
			{
				command = tackle.getCommand();
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
	}
	else
	{
		positioning();
	}
}

void Middle::positioning()
{
	Point targetPoint = worldModel->getFormation().getPosition(worldModel->getBody().getUniNum(), worldModel->getBall().getPos());
	command = DashNormalGotoPoint(targetPoint, 0.75, worldModel->getBody()).getCommand();
}

Command* Middle::getCommand()
{
	return command;
}
