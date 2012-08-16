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
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Goalie.h>

#include <Intercept.h>

#include <Tackle.h>

#include <Logger.h>
#include <Basics.h>
#include <BasicDash.h>
#include <BasicTurn.h>
#include <BasicKick.h>

#include <Degree.h>
#include <cmath>

using namespace Basics;
using namespace Degree;
using namespace std;

unsigned Goalie::lastTeammateKick(0);
bool Goalie::backPass(false);

Goalie::Goalie(const WorldModel* worldModel)
{
	this->worldModel = worldModel;
}

void Goalie::decide(Form& form)
{
	command = new EmptyCommand();

	goalieLine = getGoalieLine();
	goalieY = getGoalieY();

	if (worldModel->getBall().getPos().getDistance(worldModel->getBody().getPos()) < 25)
	{
		form.headForm.tnMode = TNM_LOOK_CAREFULLY_TO_BALL;
	}
	LOG << "[FastIC] selfReachCycle : " << worldModel->getGlobalFastIC().getSelfReachCycle() << endl;

	if (CatchDecision())
	{
		LOG << "goalie decided to catch the ball" << endl;
		return;
	}

	if (worldModel->isBallKickable())
	{
		Point targetPoint(worldModel->getBody().getPos().getX(), 34);
		if (worldModel->getBody().getPos().getY() < 0)
			targetPoint.y = -34;
		command = FastKickToPoint(targetPoint, worldModel->getBall(), worldModel->getBody()).getCommand();
		return;
	}

	Tackle tackle(worldModel);
	if (tackle.execute(form))
	{
		LOG << "Tackle Value is true, tackling" << endl;
		command = tackle.getCommand();
		return;
	}

	if (GoalieIntercept(form))
	{
		LOG << "Goalie Intercepting!!!" << endl;
		return;
	}

	if (worldModel->getGlobalFastIC().isSelfFastestTeammate() and worldModel->getBody().getPos().getDistance(Point(-52.5, 0)) < 8.f)
	{
		LOG << "Goalie Blocking!!!" << endl;
		Intercept intercept(worldModel);
		intercept.getValue();
		intercept.execute(form);
		command = intercept.getCommand();
		return;
	}

	Point ballPos = worldModel->getBall().getPos();
	if (worldModel->getGlobalFastIC().getFastestPlayers().size())
		ballPos = worldModel->getGlobalFastIC().getFastestPlayers()[0].receivePoint;
	LOG << "Goalie Positioning Selected" << endl;
	float idealBodyDir = 90;
	if (worldModel->getBall().getPos().getY() < worldModel->getBody().getPos().getY())
		idealBodyDir *= -1;
	command = GoalieGotoPoint(Point(goalieLine, goalieY), worldModel->getBody(), idealBodyDir).getCommand();
}

bool Goalie::GoalieIntercept(Form& form)
{
	Intercept intercept(worldModel);
	intercept.getValue();
	LOG << "lll" << worldModel->getGlobalFastIC().isSelfFastestPlayer() << endl;
	if (intercept.getInterceptPoint().x > -52.5 and 
		(worldModel->getGlobalFastIC().isSelfFastestPlayer() or worldModel->getGlobalFastIC().getFastestPlayerReachCycle() > intercept.getInterceptTime()) and fabs(intercept.getInterceptPoint().getY()) < 35)
	{
		LOG << "return standard intercept" << endl;
		intercept.execute(form);
		command = intercept.getCommand();
		return true;
	}
	if (worldModel->getGlobalFastIC().getFastestPlayers().size() == 0 or worldModel->getGlobalFastIC().isSelfFastestPlayer())
	{
		LOG << "use TOF intercept" << endl;
		Point GoalPoint, ballFinalPos;
		Ball virtualBall = worldModel->getBall();
		for (int i = 0; i < 20; i++)
			virtualBall.simulateByDynamics(worldModel->getBody());
		ballFinalPos = virtualBall.getPos();
		Line ballRoute (worldModel->getBall().getPos(), ballFinalPos);
		GoalPoint.x = worldModel->getBody().getPos().getX();
		GoalPoint.y = ballRoute.getY(-52.f);
		if (fabs(GoalPoint.y) < 8.5)
		{
			LOG << "toop dare mire too goal!!" << endl;
			command = DashNormalGotoPoint(GoalPoint, 0.1, worldModel->getBody()).getCommand();
			LOG << "intercept Point = " << GoalPoint << endl;
			return true;
		}
		else
			LOG << "toop to oute" << endl;
	}

	LOG << "intercept value is false" << endl;
	return false;
}

bool Goalie::CatchDecision()
{
	unsigned forgetBackPass = 30;

	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		if (worldModel->getFullPlayer(TID_OPPONENT, i).isValid())
		{
			LOG << "player[TID_OPPONENT, " << i+1 << "] is Kicking : " << worldModel->getFullPlayer(TID_OPPONENT, i).isKicking() << endl;
			if (worldModel->getFullPlayer(TID_OPPONENT, i).isKicking())
				backPass = false;
		}
	}

	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		if (worldModel->getFullPlayer(TID_TEAMMATE, i).isValid())
		{
			LOG << "player[TID_TEAMMATE, " << i+1 << "] is Kicking : " << worldModel->getFullPlayer(TID_TEAMMATE, i).isKicking() << endl;
			if (worldModel->getFullPlayer(TID_TEAMMATE, i).getTackleRemainCycle() > 0)
				LOG << "player[TID_TEAMMATE, " << i+1 << "] tackleRemainCycle : " << worldModel->getFullPlayer(TID_TEAMMATE, i).getTackleRemainCycle() << endl;
			if (worldModel->getFullPlayer(TID_TEAMMATE, i).isKicking() or worldModel->getFullPlayer(TID_TEAMMATE, i).getTackleRemainCycle() >= 9)
			{
				backPass = true;
				lastTeammateKick = worldModel->getCurCycle();
			}
		}
	}

	if (worldModel->getCurCycle() - lastTeammateKick > forgetBackPass)
		backPass = false;

	Point top_left(-53, 20),
			bottom_right(-36, -20);
	Rectangle dangerField(top_left, bottom_right);
	LOG << "Ball is " << (worldModel->isBallCatchable() ? "": "NOT") << " Catchable."<< endl;
	LOG << "Ball Distance = " << worldModel->getBall().getDistance(worldModel->getBody()) << endl;
	LOG << "backPass : " << backPass << endl;
	if (worldModel->isBallCatchable() and dangerField.isInRectangle(worldModel->getBall().getPos()) and !worldModel->isBallInTmmKickable())
	{
		LOG << "ball is catchable" << endl;
		Ball finalBall = worldModel->getBall();
		for (int i = 0; i < 30; i++)
			finalBall.simulateByDynamics(worldModel->getBody());
		Line ballRoute(worldModel->getBall().getPos(), finalBall.getPos());
		bool oppPossessor = false;
		if ((!worldModel->getGlobalFastIC().getFastestPlayers().size()) or worldModel->getGlobalFastIC().getFastestPlayers()[0].receivePoint.x < -52.f)
			oppPossessor = true;

		LOG << "oppPossessor : " << oppPossessor << endl;

		float direction = worldModel->getBall().getBodyVec().getDirection();
		if (!backPass)
		{
			LOG << "ball is catchable, and !backPass. catch ball" << endl;
			// add digonal direction (rideman Akiyama)
			float diagonal = arcTan2(worldModel->getServerParam()["catchable_area_w"].asFloat() * 0.5f, worldModel->getServerParam()["catchable_area_l"].asFloat());
			LOG << "zavieye catch = " << direction << " diagonal = " << diagonal << " end = " << direction - diagonal << endl;
			command = new CatchCommand(direction - diagonal);
			return true;
		}
		else if (fabs(ballRoute.getY(-52.5)) < 7.5 and oppPossessor and !worldModel->isBallKickable())
		{
			LOG << "behtare backPass ro catch konim" << endl;
			float diagonal = arcTan2(worldModel->getServerParam()["catchable_area_w"].asFloat() * 0.5f, worldModel->getServerParam()["catchable_area_l"].asFloat());
			LOG << "zavieye catch = " << direction << " diagonal = " << diagonal << " end = " << direction - diagonal << endl;
			command = new CatchCommand(direction - diagonal);
			return true;
		}
	}
	return false;
}

float Goalie::getGoalieLine()
{
	float line = -50.68;
	LOG << "goalieLine : " << line << endl;
	goalieLine = line;
	return line;
}

float Goalie::getGoalieY()
{
	if (worldModel->getBall().getPos().getX() <= goalieLine)
	{
		float y = worldModel->getBall().getPos().getY() > 0? 7.2:-7.2;
		LOG << "x toop az goalie Line aghab tare, goalieY : " << y << endl;
		return y;
	}

	Vector ballStopPos = worldModel->getBall().getPos();
	if (worldModel->getGlobalFastIC().getFastestPlayers().size())
		ballStopPos = worldModel->getGlobalFastIC().getFastestPlayers()[0].receivePoint;
	LOG << "ballStopPos : " << ballStopPos << endl;

	Vector tir1(-52.5, 7.2), tir2(-52.5, -7.2);
	HalfLine up(ballStopPos, tir1);
	HalfLine down(ballStopPos, tir2);

	LOG << "upDir : " << up.getDirection() << endl;
	LOG << "downDir : " << down.getDirection() << endl;
	float avgDir = getDeltaAngle(up.getDirection(), down.getDirection()) / 2 + down.getDirection();
	avgDir = normalizeAngle(avgDir);
	LOG << "avgDir : " << avgDir << endl;
	Vector target;
	Line avgLine;
	avgLine.setBySourceDir(ballStopPos, avgDir);
	target = Vector(goalieLine, avgLine.getY(goalieLine));
	target = Vector(target.getX(), min_max(-7.2, (double)target.getY(), 7.2));
	LOG << "goalieY : " << target.getY() << endl;
	return target.getY();
}

Command* Goalie::getCommand()
{
	return command;
}

