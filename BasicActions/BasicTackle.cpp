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

#include <BasicTackle.h>
#include <Logger.h>
#include <Degree.h>

#include <cmath>

using namespace Degree;
using namespace std;

TackleToDir::TackleToDir(const WorldModel *worldModel, float direction,const Body& body, bool foul)
{
	Vector target;
	target.setAsPolar(10, direction);
	target += worldModel->getBall().getPos();
	TackleToPoint tackleToPoint(worldModel, target, body, foul);
	velocity = tackleToPoint.getVelocity();
	effPower = tackleToPoint.getEffectivePower();
	possible = tackleToPoint.isPossible();
	command = tackleToPoint.getCommand();
}

bool TackleToDir::isPossible()
{
	return possible;
}

Vector TackleToDir::getVelocity()
{
	return velocity;
}

float TackleToDir::getEffectivePower()
{
	return effPower;
}

Command* TackleToDir::getCommand()
{
	return command;
}

TackleToPoint::TackleToPoint(const WorldModel *worldModel, Point target,const Body& body, bool foul)
{
	if ( worldModel->getTackleProb() < 0.01 )
	{
		possible = false;
	}

	float target_angle = Vector(target - worldModel->getBall().getPos() ).getDirection();
	float target_rel_angle = normalizeAngle(target_angle - worldModel->getBody().getBodyDir());

	effPower = worldModel->getTackleEffectivePower(target_angle);

	//LOG << "eff power = " << eff_power << endl;
	Vector accel;
	accel.setAsPolar(effPower, target_angle);

	if (accel.getMagnitude() > worldModel->getBall().getAccelMax())
		accel.setAsPolar(worldModel->getBall().getAccelMax(), accel.getDirection());
	//LOG << "accel = " << accel.getMagnitude() << " max = " << worldModel->getBall().getAccelMax() << endl;

	velocity = worldModel->getBall().getVel() + accel;

	if (velocity.getMagnitude() > worldModel->getBall().getSpeedMax())
		velocity.setAsPolar(worldModel->getBall().getSpeedMax(), velocity.getDirection());
	//LOG << "velocity = " << velocity.getMagnitude() << " max = " << worldModel->getBall().getSpeedMax() << endl;

	if ( fabs(Degree::getDeltaAngle( velocity.getDirection(), target_angle )) > 90.0 // never accelerate to the target direction
	     or velocity.getMagnitude() < 0.1 ) // too small speed
	{
    		possible = false;
	}

	possible = true;
	command = new TackleCommand(-target_rel_angle);
}

bool TackleToPoint::isPossible()
{
	return possible;
}

Vector TackleToPoint::getVelocity()
{
	return velocity;
}

float TackleToPoint::getEffectivePower()
{
	return effPower;
}

Command* TackleToPoint::getCommand()
{
	return command;
}

