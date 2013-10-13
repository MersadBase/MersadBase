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
 *  This file is created by: Ahmad Boorghany
 *  	and is modified by: Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cassert>
#include <cmath>

#include <Ball.h>
#include <Body.h>
#include <Config.h>
#include <Rectangle.h>
#include <Logger.h>
#include <Command.h>
#include <Defines.h>
#include <PossiblePoints.h>

unsigned Ball::lastCollisionCycle = 0;

using namespace std;

Ball::Ball()
{
	setCollision(false);
	setAbsoluteUpdate(false);
	setUpdateByHearPermitted(true);

	setSimCounter(0xFFFF);
	lastABVRTime = 0;
	updatedByHearFlag = false;

	latestSeeTime = 0;
}

Ball::~Ball()
{
}

void Ball::kill()
{
	Object::kill();

	setSimCounter(0xFFFF);
}

void Ball::setServerParamVars(const Param &serverParam)
{
	Object::setServerParamVars(serverParam);

	accelMax = serverParam["ball_accel_max"].asFloat();
	decay = serverParam["ball_decay"].asFloat();
	rand = serverParam["ball_rand"].asFloat();
	size = serverParam["ball_size"].asFloat();
	speedMax = serverParam["ball_speed_max"].asFloat();

	kickPowerRate = serverParam["kick_power_rate"].asFloat();
	kickRandFactorL = serverParam["kick_rand_factor_l"].asFloat();
	kickRandFactorR = serverParam["kick_rand_factor_r"].asFloat();
	stoppedBallVel = serverParam["stopped_ball_vel"].asFloat();

	tacklePowerRate = serverParam["tackle_power_rate"].asFloat();
	LOG << "\t ********** TAVAJOH, tacklePowerRate : " << tacklePowerRate << endl;
}

void Ball::update(const Body &body, bool canAVBR, unsigned curCycle)
{
	// Added By Pooria
//	Vector lastPosition = position;

	setCollision(false);

	position = possiblePoints.calculateBallPosition(*this, body, posDeviation, quantizeStep);
	seePosDeviation = posDeviation;

	absVector.setByPoints(body.getPos(), position);
	bodyVector = absVector;
	bodyVector.rotate(-body.getBodyDir());
	headVector = absVector;
	headVector.rotate(-body.getHeadDir());

	setSimCounter(0);
	setAbsoluteUpdate(false);
	latestSeeTime = curCycle;

	// Added By Pooria
	/*ptf = false;
	if (isCollision() and (position - lastPosition).getMagnitude() >= 1)
	{
		LOG << "Pooria Toff Run" << endl;
		ptf = true;
	}*/

	if (seeDistChange != NOVALUE)
	{
		velocity.setByDistDirChange(headVector, seeDistChange, seeDirChange);
		velocity.rotate(body.getHeadDir());
		velocity += body.getVel();
		lastVelocity = velocity;
		seenVel = velocity;
		seenPos = position;
		seenCounter = 0;
	}
	/*else if (updatedByHearFlag)
		velocity = lastVelocity;*/

	/*LOG << "coll = " << body.isCollisionBall() << endl;
	if (body.isCollisionBall() and curCycle != lastCollisionCycle)
	{
		setCollision(true);
		LOG << "*** Ball Collision ***" << endl;

		Vector backCollision;
		backCollision.setAsPolar(body.getSize() + getSize() -
				absVector.getMagnitude(), velocity.getDirection());
		position -= backCollision;
		velocity *= -0.1;

		absVector.setByPoints(body.getPos(), position);
		bodyVector = absVector;
		bodyVector.rotate(-body.getBodyDir());
		headVector = absVector;
		headVector.rotate(-body.getHeadDir());
		lastCollisionCycle = curCycle;
	}*/
	LOG << "ball vel = " << velocity << endl;

	updatedByHearFlag = false;

	if (canAVBR)
	    executeABVR(body);
}

void Ball::simulateByDynamics(const Body &body, unsigned checkCollisionLevel, BallStatus ballStatus, unsigned curTime)
{
	simCounter++;
	seenCounter++;

	if (curTime == 0xFFFF)
		ballStatus = BS_NONE;
	else if (curTime - seeTime > 5 && posDeviation >= 3)
		ballStatus = BS_KICKABLE_OPP;

	switch (ballStatus)
	{
	case BS_NONE:
		position += velocity;
		velocity *= decay;

		posDeviation += 1.5;
		seePosDeviation += 1.5;
		velDeviation += 0.03;
		break;

	case BS_KICKABLE_TMM:
	case BS_KICKABLE_OPP:
	case BS_KICKABLE_TMM_OPP:
		velocity *= decay;

		posDeviation += 1.5;
		seePosDeviation += 1.5;
		velDeviation += 0.03;
		break;

	case BS_KICKABLE_BODY:
	case BS_KICKABLE_BODY_TMM:
	case BS_KICKABLE_BODY_OPP:
	case BS_FREE_BALL:
	case BS_FREE_BALL_TMM:
	case BS_FREE_BALL_OPP:
	case BS_FREE_BALL_BODY:
	case BS_FREE_BALL_TMM_OPP:
	case BS_FREE_BALL_BODY_TMM:
	case BS_FREE_BALL_BODY_OPP:
		position += velocity;
		velocity *= decay;

		posDeviation += fmax(velocity.getMagnitude() * 0.3, 0.3);
		seePosDeviation += velocity.getMagnitude() * 0.5 + 0.1;
		velDeviation += 0.01;
		break;
	}

	if (posDeviation > MAX_POS_DEV)
		posDeviation = MAX_POS_DEV;
	if (seePosDeviation > MAX_POS_DEV)
		seePosDeviation = MAX_POS_DEV;
	if (velDeviation > MAX_VEL_DEV)
		velDeviation = MAX_VEL_DEV;

	// Setting vectors
	absVector.setByPoints(body.getPos(), position);
	bodyVector = absVector;
	bodyVector.rotate(-body.getBodyDir());
	headVector = absVector;
	headVector.rotate(-body.getHeadDir());

	lastVelocity *= decay;

	// Checking collision
	// Levels: 0 means do not check for collision.
	//		   1 means check only by ball distance (for usual simulating).
	//		   2 means check by body collision and ball distance (only for main cycle simulating).

//LOG << "COLL: absVector.getMagnitude(): " << absVector.getMagnitude() << endl;
//LOG << "COLL: body.getSize() + getSize(): " << body.getSize() + getSize() << endl;
//LOG << "COLL: body.getCollisionStatus(): " << body.getCollisionStatus() << endl;

	/*if ((checkCollisionLevel == 1 &&
			absVector.getMagnitude() <= body.getSize() + getSize() - 0.075) ||
		(checkCollisionLevel == 2 && body.isCollisionBall() &&
			absVector.getMagnitude() <=	body.getSize() + getSize() + 0.20) ||
		(checkCollisionLevel == 2 && body.getCollisionStatus() == TA_DONT_KNOW &&
			absVector.getMagnitude() <=	body.getSize() + getSize()))
	*/
	if (checkCollisionLevel && body.isCollisionBall())
	{
		setCollision(true);
		LOG << "*** Ball Collision ***" << endl;

		Vector backCollision;
		backCollision.setAsPolar(body.getSize() + getSize() -
				absVector.getMagnitude(), velocity.getDirection());
		position -= backCollision;
		velocity *= -0.1;

		absVector.setByPoints(body.getPos(), position);
		bodyVector = absVector;
		bodyVector.rotate(-body.getBodyDir());
		headVector = absVector;
		headVector.rotate(-body.getHeadDir());
	}
	else
		setCollision(false);
}

void Ball::simulateByAction(const Body &body, const Command *bodyCycleCommand,
		bool ballCatched)
{
	// NOTE: body must come from previous cycle.
	// NOTE: simulateByAction must call before any changes to Vectors or
	//	 position or velocity.

	// Kick
	if (dynamic_cast<const KickCommand *>(bodyCycleCommand))
	{
		float effectiveKickPower;
		Vector kickAcclerate;
		const KickCommand *kickCommand =
			dynamic_cast<const KickCommand *>(bodyCycleCommand);

		if (kickCommand->getDirection() > body.getMaxMoment() ||
			kickCommand->getDirection() < body.getMinMoment())
			assert(0);

/*		if (kickCommand->getPower() > body.getMaxPower ||
			kickCommand->getPower() < 0)
			assert(0);*/

		// Effective Kick Power formula.
		effectiveKickPower = 1
			- 0.25 * ((bodyVector.getMagnitude() - body.getSize() - size) /
				body.getKickableMargin())
			- 0.25 * (abs(bodyVector.getDirection()) / 180);
		kickAcclerate.setAsPolar(
			fmin(kickCommand->getPower(), body.getMaxPower()) *
				kickPowerRate *	effectiveKickPower,
			body.getBodyDir() + kickCommand->getDirection());

		if (kickAcclerate.getMagnitude() > accelMax)
			kickAcclerate.setAsPolar(accelMax, kickAcclerate.getDirection());

		velocity += kickAcclerate;
		if (velocity.getMagnitude() > speedMax)
			velocity.setAsPolar(speedMax, velocity.getDirection());
	}

	// Tackle
	else if (dynamic_cast<const TackleCommand *>(bodyCycleCommand))
	{
//		const TackleCommand *tackleCommand =
//			dynamic_cast<const TackleCommand *>(bodyCycleCommand);
		// TODO:
	}

	// Catch
	else if (dynamic_cast<const CatchCommand *>(bodyCycleCommand))
	{
//		const CatchCommand *catchCommand =
//			dynamic_cast<const CatchCommand *>(bodyCycleCommand);
		// TODO:
	}

	// Move
	else if (dynamic_cast<const MoveCommand *>(bodyCycleCommand))
	{
		const MoveCommand *moveCommand =
			dynamic_cast<const MoveCommand *>(bodyCycleCommand);
		if (ballCatched)
		{
			position.setAsCartesian(moveCommand->getX() + absVector.getX(),
					moveCommand->getY() + absVector.getY());
			velocity.setAsCartesian(0, 0);
		}
	}
}

void Ball::virtualUpdate(const Body &body)
{
	if (!isAbsoluteUpdate())
		position = body.getPos() + absVector;
	else
	{
		absVector.setByPoints(body.getPos(), position);
		bodyVector = absVector;
		bodyVector.rotate(-body.getBodyDir());
		headVector = absVector;
		headVector.rotate(-body.getHeadDir());
	}
}

void Ball::updateByHear(float x, float y, float velMag, float velDir,
		const Body &body)
{
	//LOG << "Ball::updateByHear" << endl;

	position.setAsCartesian(x, y);
	velocity.setAsPolar(velMag, velDir);

	posDeviation = 0.5; //velocity.getMagnitude() / 2;
	velDeviation = 0;

	setSimCounter(0);
	setAbsoluteUpdate(true);

	// Setting vectors
	absVector.setByPoints(body.getPos(), position);
	bodyVector = absVector;
	bodyVector.rotate(-body.getBodyDir());
	headVector = absVector;
	headVector.rotate(-body.getHeadDir());

	updatedByHearFlag = true;
}

void Ball::executeABVR(const Body &body)
{
	// Advance Ball Velocity Recognizer (ABVR)
	if (seeDistChange == NOVALUE &&
		absVector.getMagnitude() < ABVR_RADIUS &&
		((!isCollision() && !body.isCollisionBall() && lastABVRTime == seeTime - 1)))
	{
		Vector lastBodyVel(body.getVel());
		lastBodyVel /= body.getDecay();

		velocity = absVector - lastABVRPosition + lastBodyVel;
		velocity *= getDecay();

		velSeeTime = seeTime; // it must be in Parsing not in Updating but it is an exeption.

		LOG << "ABVR: executed for the ball." << endl;
	}

	if (absVector.getMagnitude() < ABVR_RADIUS)
	{
		lastABVRPosition = absVector;
		lastABVRTime = seeTime;
	}
	else
		lastABVRTime = 0;
}

void Ball::parseFullState(const SExpression &exp, unsigned curTime)
{
	SExpAtomic *at;
	float x, y;

	seeDistChange = NOVALUE;
	seeDirChange = -NOVALUE;
	seeDirection = -NOVALUE;

	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);
	x = at->asFloat();

	at = dynamic_cast<SExpAtomic *>(exp[2]);
	assert(at);
	y = at->asFloat() * (-1);

	position.setAsCartesian(x, y);

	at = dynamic_cast<SExpAtomic *>(exp[3]);
	assert(at);
	x = at->asFloat();

	at = dynamic_cast<SExpAtomic *>(exp[4]);
	assert(at);
	y = at->asFloat() * (-1);

	velocity.setAsCartesian(x, y);

	posDeviation = 0;
	setSimCounter(0);

	seeTime = curTime;
}

bool Ball::isValid() const
{
	if (simCounter < BALL_VALID_CYCLE)
		return true;
	return false;
}

// Getting functions
unsigned Ball::getSimCounter() const
{
	return simCounter;
}

bool Ball::isCollision() const
{
	return collisionFlag;
}

bool Ball::isAbsoluteUpdate() const
{
	return absoluteUpdateFlag;
}

float Ball::getKickPowerRate() const
{
	return kickPowerRate;
}

float Ball::getKickRandFactorL() const
{
	return kickRandFactorL;
}

float Ball::getKickRandFactorR() const
{
	return kickRandFactorR;
}

float Ball::getStoppedBallVel() const
{
	return stoppedBallVel;
}

unsigned Ball::getLatestSeeTime() const
{
	return latestSeeTime;
}

bool Ball::isUpdateByHearPermitted() const
{
	return updateByHearPermittedFlag;
}

bool Ball::isUpdatedByHear() const
{
	return updatedByHearFlag;
}

bool Ball::isOut(float margin) const
{
	return !Rectangle(-52.5 - margin, -34 - margin, 52.5 + margin, 34 + margin).isInRectangle(
			getPos());
}

Vector Ball::getSeenPos() const
{
	return seenPos;
}

Vector Ball::getSeenVel() const
{
	return seenVel;
}

unsigned Ball::getSeenCounter() const
{
	return seenCounter;
}
// Setting functions
void Ball::setSimCounter(unsigned simCounterArg)
{
	simCounter = simCounterArg;
}

void Ball::setCollision(bool collisionFlagArg)
{
	collisionFlag = collisionFlagArg;
}

void Ball::setAbsoluteUpdate(bool absoluteUpdateFlagArg)
{
	absoluteUpdateFlag = absoluteUpdateFlagArg;
}

void Ball::setUpdateByHearPermitted(bool updateByHearPermittedFlagArg)
{
	updateByHearPermittedFlag = updateByHearPermittedFlagArg;
}

float Ball::getTacklePowerRate() const
{
	return tacklePowerRate;
}

float Ball::getEffectiveKickPower(const Body &body) const
{
	return (1
			- 0.25
					* ((bodyVector.getMagnitude() - body.getSize() - size)
							/ body.getKickableMargin())
			- 0.25 * (abs(bodyVector.getDirection()) / 180));
}
