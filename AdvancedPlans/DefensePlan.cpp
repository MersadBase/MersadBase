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

#include <DefensePlan.h>
#include <Share.h>
#include <Offense.h>
#include <Tackle.h>
#include <MultiStepKick.h>
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

#define LOGD LOG

DefensePlan::DefensePlan(const WorldModel *wm) :
		wm(wm)
{
}

DefensePlan::~DefensePlan()
{
}

void DefensePlan::decide(Form &form)
{
	Tackle tackle(wm);

	if (wm->isBallKickable())
	{
		LOG << "\t tup tu kickable e " << endl;
		doKick(form);
	}
	else if (wm->getGlobalFastIC().isSelfFastestTeammate()
			&& wm->getCurCycle() - Share::lastPass > 10 && wm->getBall().getPos().getX() < 40)
	{
		if (tackle.execute(form))
		{
			command = tackle.getCommand();
		}
		else
		{
			Intercept intercept = Intercept(wm);
			LOGD << "Intercept Created" << endl;
			intercept.getValue();
			LOGD << "Intercept GetValued" << endl;
			intercept.execute(form);
			LOGD << "Intercept Exectued" << endl;
			command = intercept.getCommand();
			LOGD << "Intercept Commanded" << endl;
		}
	}
	else
	{
		doPositioning();
	}
}

void DefensePlan::doPositioning()
{
//	Point targetPoint = wm->getFormation().getPosition(wm->getBody().getUniNum(), wm->getBall().getPos());
	Point targetPoint = wm->getPlayOnFormation().getPosition(wm->getBody().getUniNum()-1, wm->getBall().getPos());
//	Point targetPoint;
//	targetPoint.y = 0;
//	if (wm->getBody().getUniNum() == 2)
//		targetPoint.x = 20;
//	else
//		targetPoint.x = -20;
	command = DashNormalGotoPoint(targetPoint, 0.75, wm->getBody()).getCommand();
}

void DefensePlan::doKick(Form& form)
{
//	Point target;
//	if (wm->getBody().getUniNum() == 2)
//		target.x = -20;
//	else
//		target.x = 20;
//	target.y = -10;
//	target.y += wm->getCurCycle() % 20;
//	MultiStepKick hi(wm, wm->getBody(), wm->getBall(), target, 3, 10);
//	hi.calculate();
//	command = hi.getCommand();
//	Share::chain.clear();
//	for (int i = 1; i < hi.getMaxAvailableStep(); i++)
//	{
//		Share::chain.push_back(hi.getChain(i));
//		Share::chain.back().cycle = wm->getCurCycle() + i;
//	}
//	Share::lastPass = wm->getCurCycle();
//	return;
//	HoldBall hb(wm);
//	hb.decide();
//	command = hb.getCommand();
//	return;

//	command = new EmptyCommand();
//	return;

	Shoot shoot(wm);
	Dribble dribble(wm);
	Pass pass(wm);
	LOG << "\tsakhtan ha tamum shod" << endl;
	if (shoot.execute())
	{
		LOG << "\tShoot value dad " << endl;
		command = shoot.getCommand();
	}
	else
	{
		LOG << "\tsalam" << endl;
		LOG << "\tDribble.getValue() = " << dribble.getValue() << endl;
		LOG << "\tPass.getValue() = " << pass.getValue() << endl;
		if (dribble.getValue() >= pass.getValue() && false)
		{
			LOG << "\tdribble mikonam " << endl;
			dribble.decide(form);
			command = dribble.getCommand();
		}
		else
		{
			LOG << "\t Pass midam " << endl;
			pass.decide(form);
			command = pass.getCommand();
		}
	}
}

Command* DefensePlan::getCommand()
{
	return command;
}
