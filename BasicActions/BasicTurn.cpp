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
 *  This file is created by: Mohammad Salehe
 *  	and is modified by: Pooria Kaviani
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <assert.h>

#include <BasicTurn.h>
#include <Degree.h>
#include <Player.h>
#include <Body.h>
#include <Ball.h>
#include <Logger.h>

using namespace std;

BasicTurn::BasicTurn(): BasicAction()
{
}

float BasicTurn::getDeltaAngle()
{
	return deltaAngle;
}

float BasicTurn::getNecessaryTurnAngle()
{
	return necessaryTurnAngle;
}

float BasicTurn::getRequestedTargetAngle()
{
	return requestedTargetAngle;
}

float BasicTurn::getPerformedTurnAngle()
{
	return performedTurnAngle;
}

float BasicTurn::getPerformedTurnMoment()
{
	return performedTurnMoment;
}

bool BasicTurn::isOneTurnPossible()
{
	return oneTurnPossible;
}

float BasicTurn::getTurnMoment(float angle, const Player &player)
{
	float moment = 0;

	angle = Degree::normalizeAngle(angle);
	moment = angle * (1 + player.getVel().getMagnitude() *
			 player.getInertiaMoment());
	return moment;
}

float BasicTurn::getTurnAngle(float moment, const Player &player)
{
	float angle = 0;

	assert(moment <= player.getMaxMoment() &&
			moment >= player.getMinMoment());
	angle = moment / (1 + player.getVel().getMagnitude() *
			player.getInertiaMoment());
	return angle;
}

int BasicTurn::actionsToTurnAngle(float angle, const Player &player, float maxDeltaAngle)
{
	float turnedAngle = 0;
	Player simPlayer(player);
	int i;

	angle = fabs(angle) - maxDeltaAngle;

	for (i = 0; i <= 10; i++)
	{
		if (turnedAngle >= angle)
			return i;
		turnedAngle += getTurnAngle(player.getMaxMoment(), player);
		simPlayer.setVel() *= simPlayer.getDecay();
	}
	assert(i < 10);
	return 10;
}

int BasicTurn::getMinCyclesToTurnToPoint(const Point &myPoint, Body body, float threshold)
{
	int turnCycles = 0;
	float calculatingRate = body.getVel().getMagnitude();
	while (true)
	{
		float thisAngle = (myPoint - body.getPos()).getDirection();
		thisAngle = Degree::getDeltaAngle(body.getBodyDir(), thisAngle);
		calculatingRate = body.getVel().getMagnitude();
		float turnAngle = thisAngle;
		turnAngle = turnAngle * (1 + body.getInertiaMoment() * calculatingRate);
		turnCycles++;
		if (std::fabs(turnAngle) > body.getMaxMoment())
		{
			turnAngle = Degree::normalizeAngle((turnAngle / std::fabs(turnAngle)) * body.getMaxMoment());
		}
		else
			return turnCycles;
		turnAngle = turnAngle / (1 + body.getInertiaMoment() * calculatingRate);
		calculatingRate *= body.getDecay();
		body.setBodyDir(body.getBodyDir() + turnAngle);
		body.simulateByDynamics();
	}
	return turnCycles;
}


// TurnAngle class

TurnAngle::TurnAngle(float angle, const Body &body, float maxDeltaAngle) : BasicTurn()
{
	float requestedTurnMoment;

	necessaryTurnAngle = angle;
	requestedTargetAngle = body.getBodyDir() + angle;
	requestedTurnMoment = getTurnMoment(angle, body);

	if (fabs(requestedTurnMoment) <= body.getMaxMoment() &&
		fabs(requestedTurnMoment) >= body.getMinMoment()) // possible with one turn
	{
		performedTurnMoment = requestedTurnMoment;
		performedTurnAngle = angle;
		oneTurnPossible = true;
		deltaAngle = 0;
	}
	else // not possible with one turn
	{
		if (requestedTurnMoment < 0)
			performedTurnMoment = body.getMinMoment();
		else
			performedTurnMoment = body.getMaxMoment();

		performedTurnAngle = getTurnAngle(performedTurnMoment, body);

		if (fabs(performedTurnAngle - angle) <= maxDeltaAngle) // but it is good
			oneTurnPossible = true;
		else // so it is not good
			oneTurnPossible = false;
		deltaAngle = fabs(performedTurnAngle - angle);
	}

	turnCommand = new TurnCommand(performedTurnMoment);
}

Command *TurnAngle::getCommand()
{
	return turnCommand;
}


TurnToAngle::TurnToAngle(float angle, const Body &body, float maxDeltaAngle) : BasicTurn()
{
	TurnAngle *doTurnAngle = NULL;

	doTurnAngle = new TurnAngle(angle - body.getBodyDir(), body, maxDeltaAngle);

	turnCommand = doTurnAngle->getCommand();
	deltaAngle = doTurnAngle->getDeltaAngle();
	necessaryTurnAngle = doTurnAngle->getNecessaryTurnAngle();
	requestedTargetAngle = doTurnAngle->getRequestedTargetAngle();
	performedTurnAngle = doTurnAngle->getPerformedTurnAngle();
	performedTurnMoment = doTurnAngle->getPerformedTurnMoment();
	oneTurnPossible = doTurnAngle->isOneTurnPossible();

	delete doTurnAngle;
}

Command *TurnToAngle::getCommand()
{
	return turnCommand;
}


TurnToPoint::TurnToPoint(const Point point, const Body &body, float maxDeltaAngle)
		: BasicTurn()
{
	Vector newBodyPosition;
	Vector directionVector;
	TurnToAngle *doTurnToAngle = NULL;

	newBodyPosition = (body.getPos()) + (body.getVel());

	directionVector.setByPoints(newBodyPosition,point);

	doTurnToAngle = new TurnToAngle(directionVector.getDirection(), body, maxDeltaAngle);

	turnCommand = doTurnToAngle->getCommand();
	deltaAngle = doTurnToAngle->getDeltaAngle();
	necessaryTurnAngle = doTurnToAngle->getNecessaryTurnAngle();
	requestedTargetAngle = doTurnToAngle->getRequestedTargetAngle();
	performedTurnAngle = doTurnToAngle->getPerformedTurnAngle();
	performedTurnMoment = doTurnToAngle->getPerformedTurnMoment();
	oneTurnPossible = doTurnToAngle->isOneTurnPossible();

	delete doTurnToAngle;
}

Command *TurnToPoint::getCommand()
{
	return turnCommand;
}


TurnToObject::TurnToObject(const Object &object, const Body &body, float maxDeltaAngle) : BasicTurn()
{
	TurnToPoint *doTurnToPoint = NULL;

	doTurnToPoint = new TurnToPoint(object.getPos(), body, maxDeltaAngle);

	turnCommand = doTurnToPoint->getCommand();
	deltaAngle = doTurnToPoint->getDeltaAngle();
	necessaryTurnAngle = doTurnToPoint->getNecessaryTurnAngle();
	requestedTargetAngle = doTurnToPoint->getRequestedTargetAngle();
	performedTurnAngle = doTurnToPoint->getPerformedTurnAngle();
	performedTurnMoment = doTurnToPoint->getPerformedTurnMoment();
	oneTurnPossible = doTurnToPoint->isOneTurnPossible();

	delete doTurnToPoint;
}

Command *TurnToObject::getCommand()
{
	return turnCommand;
}
