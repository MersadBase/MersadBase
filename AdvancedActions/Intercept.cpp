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
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <iostream>
#include <Intercept.h>
#include <Ball.h>
#include <Degree.h>
#include <Basics.h>
#include <Logger.h>
#include <BasicDash.h>
#include <BasicTurn.h>
#include <WorldModel.h>
#include <HeadDecision.h>
#include <AdvancedAgent.h>

//#define LOG_PLAYER

using namespace std;
using namespace Basics;
using namespace Degree;

Intercept::Intercept(const WorldModel *wm): wm(wm)
{
	goalieMode = false;
	goalieMinusTime = 0;
	stopICStart = false;
	defenseBlock = false;
	defenseKickable = 0;
	goalieKickable = 0;
	interceptTime = 0;
	maxWithTurn = 0;
	maxMinus = 0;
	maxPlus = 0;
	SRP = false;
	shoot = false;
	interceptPoint = Point(-60, -60);
}

void Intercept::execute(Form &form)
{
	LOG << "Intercept::execute" << endl;

// Look Carefully to Ball
	form.headForm.tnMode = TNM_LOOK_CAREFULLY_TO_BALL;

	form.comment.addComment("InterceptAdvAct");
}

float Intercept::getValue()
{
#ifdef LOG_PLAYER
	LOG << "Intercept::getValue" << endl;
#endif

	stopICStart = false;
	interceptTime = 0;

// Checking for who is fastest teammate or opponent
	if (goalieMode == false && defenseBlock == false && SRP == false)
		firstBall = wm->getBall();

	if (!SRP)
		firstBody = wm->getBody();

	if (firstBall.getDistance(firstBody) <= getKickableDist())
	{
		interceptTime = 0;
		interceptPoint = firstBall.getPos();
		command = new EmptyCommand();
		return 1;
	}

	Vector vec;
	vec.setAsPolar(10, firstBody.getBodyDir());
	Command *dash = DashNormalGotoPoint(firstBody.getPos() + vec, getKickableDist(), firstBody).getCommand();

//---------------------------------------------------------------------------------------------------------------------//	
/**************************************************** Turn Intercept***********************************************************/
//---------------------------------------------------------------------------------------------------------------------//	

	unsigned turnInterceptTime = 0;

	float dashDist = 0;
	Body tmpBody = firstBody;
	Body virBody = firstBody;
	virBall = firstBall;
	tmpBody.setVel().setAsCartesian(0, 0);

	Command *turnCommand = NULL;
	unsigned time = 0;
	Point withTurnPoint, finalWithTurnInterceptPoint(-60, -60);

	if (!SRP)
		maxWithTurn = 99;
	for (unsigned i = 1; i < maxWithTurn; i++)
	{
		tmpBody = firstBody;
		virBody = firstBody;
		virBall = firstBall;
		tmpBody.setVel().setAsCartesian(0, 0);
		dashDist = 0;
		turnInterceptTime = 0;
		while (true)
		{
			if (virBall.getDistance(virBody) < dashDist + getKickableDist())
				break;
			// One cycle past
			if (turnInterceptTime > i - 1)
			{
				tmpBody.simulateByAction(dash);
				tmpBody.simulateByDynamics();
				dashDist = tmpBody.getDistance(firstBody);
			}
			turnInterceptTime++;
			virBody.simulateByDynamics();
			virBall.simulateByDynamics(firstBody);
		}
		withTurnPoint = virBall.getPos();

		TurnToPoint lastTurnCommand(withTurnPoint, firstBody);
		turnCommand = lastTurnCommand.getCommand();
		time = 0;
		Body body = firstBody;
		while (true)
		{
			time++;
			if (lastTurnCommand.isOneTurnPossible())
				break;
			body.simulateByAction(lastTurnCommand.getCommand());
			body.simulateByDynamics();
			lastTurnCommand = TurnToPoint(withTurnPoint, body);
		}

		if (time <= i)
		{
			finalWithTurnInterceptPoint = virBall.getPos();
			break;
		}
	}
// For Debug
	virBody = firstBody;
	virBall = firstBall;
	unsigned pt = time;

	Command *lastTurnCommand = turnCommand;
	for (unsigned t = 0; t < time; t++)
	{
		virBody.simulateByAction(lastTurnCommand);
		virBody.simulateByDynamics();
		lastTurnCommand = TurnToPoint(withTurnPoint, virBody).getCommand();
	}

	Point p[100];
	int ss = -1;

//LOG << "kci: " << getKickableDist() << endl;
	for (unsigned i = 0; i < 100;  i++)
	{
		p[i] = virBody.getPos();
//LOG << "!" << endl;
//LOG << "VirBAL: " << virBall.getPos() << endl;
		for (unsigned j = 0; j <= i; j++)
		{
//LOG << "Dist: " << virBall.getDistance(p[j]) << endl;
			if (virBall.getDistance(p[j]) < getKickableDist())
			{
				ss = j;
				break;
			}
		}
		if (ss != -1)
			break;
		// One cycle past
		virBody.simulateByAction(dash);
		virBody.simulateByDynamics();
		virBall.simulateByDynamics(virBody);
		pt++;
	}

	if (wm->getBody().getPos().getX() > 10
			and (pt > 8 && abs(normalizeAngle(firstBody.getBodyDir() -
			(Vector(withTurnPoint) - firstBody.getPos()).getDirection())) <
			(Vector(withTurnPoint) - firstBody.getPos()).getMagnitude() * .8f))
		turnCommand = dash;
	turnInterceptTime = pt;

#ifdef LOG_PLAYER
	LOG << "~PT: " << pt << endl;
#endif
//---------------------------------------------------------------------------------------------------------------------//	
/************************************************* Minus Dash***********************************************************/
//---------------------------------------------------------------------------------------------------------------------//	

	DashCommand dashMinus(-100);
	virBody = firstBody;
	virBall = firstBall;
	bool minusInterceptPossible;
	unsigned minusInterceptTime = 0;

	Point minusPoints[6], finalMinusInterceptPoint(-60, -60);
	int s = -1;
	if (!SRP)
		maxMinus = 6;
	if (goalieMode)
		maxMinus = goalieMinusTime;
	if (defenseBlock)
		maxMinus = 0;
	for (unsigned i = 0; i < maxMinus; i++)
	{
		minusPoints[i] = virBody.getPos();
		for (unsigned j = 0; j <= i; j++)
		{
			if (virBall.getDistance(minusPoints[j]) < getKickableDist())
			{
				s = j;
				j = i + 1;
			}
		}
		if (s != -1)
		{
			finalMinusInterceptPoint = virBall.getPos();
			break;
		}
		// One cycle past
		virBody.simulateByAction(&dashMinus);
		virBody.simulateByDynamics();
		virBall.simulateByDynamics(virBody);
		minusInterceptTime++;
	}

	Command *minusCommand = new EmptyCommand();

	if ((s >= 1 || (SRP && s >= 0)) && minusInterceptTime > 0 && (firstBody.getStamina() > 1400 || SRP) && !defenseBlock)
	{
		minusInterceptPossible = true;
		minusCommand = new DashCommand(-100);
	}
	else
	{
		minusInterceptPossible = false;
		minusInterceptTime = 999999999;
	}

//---------------------------------------------------------------------------------------------------------------------//	
/************************************************* Plus Dash************************************************************/
//---------------------------------------------------------------------------------------------------------------------//

	virBody = firstBody;
	virBall = firstBall;
	dash = DashNormalGotoPoint(firstBody.getPos() + vec, getKickableDist(), firstBody).getCommand();
	bool plusInterceptPossible;
	unsigned plusInterceptTime = 0;

	Point points[100], finalPlusInterceptPoint(-60, -60);
	s = -1;

	unsigned tmpTime = 0;
	if (!SRP)
		maxPlus = 100;
	for (unsigned i = 0; i < maxPlus;  i++)
	{
//LOG << "!" << endl;
//LOG << "VIR: " << virBall.getPos() << endl;
		points[i] = virBody.getPos();
		for (unsigned j = 0; j <= i; j++)
		{
//LOG << "DIST: " << virBall.getDistance(points[j]) << endl;
			if (virBall.getDistance(points[j]) <= getKickableDist())
			{
				s = j;
				if (s == 0)
					tmpTime = plusInterceptTime;
				break;
			}
		}
		if (s != -1)
		{
			finalPlusInterceptPoint = virBall.getPos();
			break;
		}
		// One cycle past
		virBody.simulateByAction(dash);
		virBody.simulateByDynamics();
		virBall.simulateByDynamics(virBody);
		plusInterceptTime++;
	}

	Command *plusCommand = new EmptyCommand();

	if (s >= 1 && plusInterceptTime > 0)
	{
		plusInterceptPossible = true;
		plusCommand = dash;
	}
	else
	{
		plusInterceptPossible = false;
		plusInterceptTime = 999999999;
	}

//---------------------------------------------------------------------------------------------------------------------//	
/**************************************************** Stop Intercept **********************************************************/
//---------------------------------------------------------------------------------------------------------------------//	

	bool stopInterceptPossible;
	unsigned stopInterceptTime = 1;
	Command *stopCommand = new EmptyCommand();

	stopICStart = true;

	Point finalStopInterceptPoint = firstBall.getPos();
	if (s == 0)
	{
		stopInterceptPossible = true;
		virBody = firstBody;
		virBall = firstBall;
		for (stopInterceptTime = 0; stopInterceptTime < tmpTime; stopInterceptTime++)
		{
			virBody.simulateByDynamics();
			virBall.simulateByDynamics(virBody);
		}

		if (virBody.getDistance(virBall) > getKickableDist())
		{
			virBody = firstBody;
			virBall = firstBall;
			Command *tmpCom = new DashCommand(50);
			virBody.simulateByAction(tmpCom);
			for (unsigned i = 0; i < tmpTime; i++)
			{
				virBody.simulateByDynamics();
				virBall.simulateByDynamics(virBody);
			}
			float firstDist = virBody.getDistance(virBall);
			tmpCom = new DashCommand(-50);
			virBody = firstBody;
			virBall = firstBall;
			virBody.simulateByAction(tmpCom);
			for (unsigned i = 0; i < tmpTime; i++)
			{
				virBody.simulateByDynamics();
				virBall.simulateByDynamics(virBody);
			}
			float secondDist = virBody.getDistance(virBall);
			if (firstDist < secondDist)
				stopCommand = new DashCommand(50);
			else
				stopCommand = new DashCommand(-50);
			virBody = firstBody;
			virBall = firstBall;
			virBody.simulateByAction(stopCommand);
			virBody.simulateByDynamics();
			virBall.simulateByDynamics(virBody);
			if (virBody.getDistance(virBall) > getKickableDist())
			{
				if (firstDist < secondDist)
					stopCommand = new DashCommand(75);
				else
					stopCommand = new DashCommand(-75);
			}
		}
		else
		{
			stopInterceptPossible = false;
			stopInterceptTime = 999999999;
		}
	}
	else
	{
		stopInterceptPossible = false;
		finalStopInterceptPoint = Point(-60, -60);
		stopInterceptTime = 999999999;
	}

// Select Best InterCept From Turn InterCept, Minus InterCept, Plus(Stright) InterCept and Stop InterCept

	unsigned min = turnInterceptTime;

	command = turnCommand;
	interceptTime = turnInterceptTime;
	interceptPoint = finalWithTurnInterceptPoint;

	unsigned log = 1;
	if (!SRP)
	{
		if (plusInterceptPossible && plusInterceptTime <= min)
		{
			min = plusInterceptTime;
			log = 2;
			interceptTime = plusInterceptTime;
			interceptPoint = finalPlusInterceptPoint;
			command = plusCommand;
		}

		if (minusInterceptPossible && minusInterceptTime < min)
		{
			min = minusInterceptTime;
			log = 3;
			interceptTime = minusInterceptTime;
			interceptPoint = finalMinusInterceptPoint;
			command = minusCommand;
		}

		if (stopInterceptPossible && stopInterceptTime < min)
		{
			min = stopInterceptTime;
			log = 4;
			interceptTime = stopInterceptTime;
			interceptPoint = finalStopInterceptPoint;
			command = stopCommand;
		}
	}
	else
	{
		min -= 1;
		if (plusInterceptPossible && plusInterceptTime <= min)
		{
			min = plusInterceptTime;
			log = 2;
			interceptTime = plusInterceptTime;
			interceptPoint = finalPlusInterceptPoint;
			command = plusCommand;
		}

		if (minusInterceptPossible && minusInterceptTime < min)
		{
			min = minusInterceptTime;
			log = 3;
			interceptTime = minusInterceptTime;
			interceptPoint = finalMinusInterceptPoint;
			command = minusCommand;
		}

		if (stopInterceptPossible && stopInterceptTime < min)
		{
			min = stopInterceptTime;
			log = 4;
			interceptTime = stopInterceptTime;
			interceptPoint = finalStopInterceptPoint;
			command = stopCommand;
		}
	}

#ifdef LOG_PLAYER
		LOG << "~Turn Time: " << turnInterceptTime << endl;
		LOG << "~Stright Time: " << plusInterceptTime << endl;
		LOG << "~Minus Time: " << minusInterceptTime << endl;
		LOG << "~Stop Time: " << stopInterceptTime << endl;

		if (log == 1)
			LOG << "~Turn InterCeption" << endl;
		if (log == 2)
			LOG << "~Stright InterCeption" << endl;
		if (log == 3)
			LOG << "~Minus InterCeption" << endl;
		if (log == 4)
			LOG << "~Stop InterCeption" << endl;
		LOG << "InterceptPoint: " << interceptPoint << endl;
		LOG << "InterceptTime: " << interceptTime << endl;
#endif

	return 1;
}

Command *Intercept::getCommand()
{
	return command;
}

float Intercept::getKickableDist()
{
	float kickableDist = (wm->getBall().getSize() + firstBody.getKickableMargin() + firstBody.getSize());
//	if (firstBody.getTeamId() == TID_OPPONENT)
//		kickableDist *= 100 / 91;

	if (stopICStart)
		kickableDist = 0.9;

	if (firstBody.isGoalie() && firstBody.getTeamId() == TID_TEAMMATE
		&& virBall.getPos().getX() <= -36 && abs(virBall.getPos().getY()) <= 20.01)
	{
//		kickableDist = hypot(firstBody.getCatchableAreaL(), (firstBody.getCatchableAreaW() / 2)) - 0.1;
		kickableDist = 1.2;
	}

	if (shoot && virBall.getPos().getX() >= 36 && abs(virBall.getPos().getY()) <= 20.01)
	{
//		kickableDist = hypot(firstBody.getCatchableAreaL(), (firstBody.getCatchableAreaW() / 2)) - 0.1;
		kickableDist = 1.2;
	}

	if (goalieMode)
		kickableDist = goalieKickable;
	if (defenseBlock)
		kickableDist = defenseKickable;

	return kickableDist;
}

void Intercept::setBall(const Ball *ball)
{
	firstBall = *ball;
}

void Intercept::setBody(const Body *body)
{
	firstBody = *body;
}

unsigned Intercept::getInterceptTime()
{
	return interceptTime;
}

void Intercept::setMaxPlus(unsigned a)
{
	maxPlus = a;
}

void Intercept::setMaxMinus(unsigned a)
{
	maxMinus = a;
}

void Intercept::setMaxWithTurn(unsigned a)
{
	maxWithTurn = a;
}

unsigned Intercept::getMaxPlus()
{
	return maxPlus;
}

unsigned Intercept::getMaxMinus()
{
	return maxMinus;
}

unsigned Intercept::getMaxWithTurn()
{
	return maxWithTurn;
}

Point Intercept::getInterceptPoint()
{
	return interceptPoint;
}
