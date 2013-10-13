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

Offense::Offense(const WorldModel *wm): wm(wm)
{
}

Offense::~Offense()
{
}

void Offense::decide(Form &form)
{
	Tackle tackle(wm);
	if (wm->getGlobalFastIC().isSelfFastestTeammate())
	{
		if (wm->isBallKickable())
		{
			Shoot shoot(wm);
			Dribble dribble(wm);
			Pass pass(wm);
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
				Intercept intercept = Intercept(wm);
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

void Offense::positioning()
{
	Point targetPoint = wm->getPlayOnFormation().getPosition(wm->getBody().getUniNum()-1, wm->getBall().getPos());
	command = DashNormalGotoPoint(targetPoint, 0.75, wm->getBody()).getCommand();
}



Command* Offense::getCommand()
{
	return command;
}
