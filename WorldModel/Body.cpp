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

#include <assert.h>

#include <Body.h>
#include <Degree.h>
#include <Config.h>
#include <Logger.h>
#include <Command.h>
#include <SExpression.h>
#include <PossiblePoints.h>

using namespace std;
using namespace Degree;

Body::Body()
{
	//collisionStatus = TA_DONT_KNOW;
	collisionBall = collisionPlayer = collisionPost = false;
	seeSynch = false;
}

void Body::parseSenseBody(const SExpression &exp,Side fieldSide)
{
	SExpAtomic *at;
	SExpression *se1,*se2;

	at = dynamic_cast<SExpAtomic *>(exp[0]);
	assert(at);
	assert(at->asString() == "sense_body");

	// preparing.
	lastKickCount = kickCount;
	lastDashCount = dashCount;
	lastTurnCount = turnCount;
	lastSayCount = sayCount;
	lastTurnNeckCount = turnNeckCount;
	lastCatchCount = catchCount;
	lastMoveCount = moveCount;
	lastChangeViewCount = changeViewCount;
	lastArmExpires = armExpires;
	lastArmCount = armCount;
	lastFocusCount = focusCount;
	lastTackleExpires = tackleExpires;
	lastTackleCount = tackleCount;

	// SenseBody cycle
	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);
	senseBodyCycle = at->asInt();

	// View mode
	se1 = dynamic_cast<SExpression *>(exp[2]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "view_mode");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	if (at->asString()[0] == 'l')
		vmQuality = VMQ_LOW;
	else
	{
		assert(at->asString()[0] == 'h');
		vmQuality = VMQ_HIGH;
	}

	at = dynamic_cast<SExpAtomic *>((*se1)[2]);
	assert(at);
	if (at->asString()[0] == 'w')
		vmWidth = VMW_WIDE;
	else
	{
		assert(at->asString()[0] == 'n');

		if (at->asString()[1] == 'a')
			vmWidth = VMW_NARROW;
		else
		{
			assert(at->asString()[1] == 'o');
			vmWidth = VMW_NORMAL;
		}
	}

	// Stamina
	se1 = dynamic_cast<SExpression *>(exp[3]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "stamina");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	stamina = at->asFloat();

	at = dynamic_cast<SExpAtomic *>((*se1)[2]);
	assert(at);
	effort = at->asFloat();

	at = dynamic_cast<SExpAtomic *>((*se1)[3]);
	assert(at);
	battery = at->asFloat();

	// Speed
	se1 = dynamic_cast<SExpression *>(exp[4]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "speed");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	speedAmount = at->asFloat();

	at = dynamic_cast<SExpAtomic *>((*se1)[2]);
	assert(at);
	speedDir = at->asFloat();

	// Head angle
	se1 = dynamic_cast<SExpression *>(exp[5]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "head_angle");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	relativeHeadDir = at->asFloat();

	// Kick
	se1 = dynamic_cast<SExpression *>(exp[6]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "kick");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	kickCount = at->asInt();

	// Dash
	se1 = dynamic_cast<SExpression *>(exp[7]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "dash");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	dashCount = at->asInt();

	// Turn
	se1 = dynamic_cast<SExpression *>(exp[8]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "turn");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	turnCount = at->asInt();

	// Say
	se1 = dynamic_cast<SExpression *>(exp[9]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "say");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	sayCount = at->asInt();

	// Turn Neck
	se1 = dynamic_cast<SExpression *>(exp[10]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "turn_neck");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	turnNeckCount = at->asInt();

	// Catch
	se1 = dynamic_cast<SExpression *>(exp[11]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "catch");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	catchCount = at->asInt();

	// Move
	se1 = dynamic_cast<SExpression *>(exp[12]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "move");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	moveCount = at->asInt();

	// Change View
	se1 = dynamic_cast<SExpression *>(exp[13]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "change_view");

	at = dynamic_cast<SExpAtomic *>((*se1)[1]);
	assert(at);
	changeViewCount = at->asInt();

	// Arm
	se1 = dynamic_cast<SExpression *>(exp[14]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "arm");

	se2 = dynamic_cast<SExpression *>((*se1)[1]);
	assert(se2);
	at = dynamic_cast<SExpAtomic *>((*se2)[0]);
	assert(at);
	assert(at->asString() == "movable");
	at = dynamic_cast<SExpAtomic *>((*se2)[1]);
	assert(at);
	armMovable = at->asInt();

	se2 = dynamic_cast<SExpression *>((*se1)[2]);
	assert(se2);
	at = dynamic_cast<SExpAtomic *>((*se2)[0]);
	assert(at);
	assert(at->asString() == "expires");
	at = dynamic_cast<SExpAtomic *>((*se2)[1]);
	assert(at);
	armExpires = at->asInt();

	se2 = dynamic_cast<SExpression *>((*se1)[3]);
	assert(se2);
	at = dynamic_cast<SExpAtomic *>((*se2)[0]);
	assert(at);
	assert(at->asString() == "target");

	at = dynamic_cast<SExpAtomic *>((*se2)[1]);
	assert(at);
	armTargetDist = at->asFloat();

	at = dynamic_cast<SExpAtomic *>((*se2)[2]);
	assert(at);
	armTargetDir = at->asFloat();

	se2 = dynamic_cast<SExpression *>((*se1)[4]);
	assert(se2);
	at = dynamic_cast<SExpAtomic *>((*se2)[0]);
	assert(at);
	assert(at->asString() == "count");
	at = dynamic_cast<SExpAtomic *>((*se2)[1]);
	assert(at);
	armCount = at->asInt();

	// Focus
	se1 = dynamic_cast<SExpression *>(exp[15]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "focus");

	se2 = dynamic_cast<SExpression *>((*se1)[1]);
	assert(se2);
	at = dynamic_cast<SExpAtomic *>((*se2)[0]);
	assert(at);
	assert(at->asString() == "target");
	at = dynamic_cast<SExpAtomic *>((*se2)[1]);
	assert(at);
	if (at->asString() == "none")
		focusTargetUniNum = 0; // it means there is no focus.
	else
	{
		if (at->asString()[0] == 'l')
			if (fieldSide == SI_LEFT)
				focusTargetTeamId = TID_TEAMMATE;
			else
				focusTargetTeamId = TID_OPPONENT;
		else
		{
			assert(at->asString()[0] == 'r');
			if (fieldSide == SI_RIGHT)
				focusTargetTeamId = TID_TEAMMATE;
			else
				focusTargetTeamId = TID_OPPONENT;
		}

		at = dynamic_cast<SExpAtomic *>((*se2)[2]);
		assert(at);
		focusTargetUniNum = at->asInt();
	}

	se2 = dynamic_cast<SExpression *>((*se1)[2]);
	assert(se2);
	at = dynamic_cast<SExpAtomic *>((*se2)[0]);
	assert(at);
	assert(at->asString() == "count");
	at = dynamic_cast<SExpAtomic *>((*se2)[1]);
	assert(at);
	focusCount = at->asInt();

	// Tackle
	se1 = dynamic_cast<SExpression *>(exp[16]);
	assert(se1);
	at = dynamic_cast<SExpAtomic *>((*se1)[0]);
	assert(at);
	assert(at->asString() == "tackle");

	se2 = dynamic_cast<SExpression *>((*se1)[1]);
	assert(se2);
	at = dynamic_cast<SExpAtomic *>((*se2)[0]);
	assert(at);
	assert(at->asString() == "expires");
	at = dynamic_cast<SExpAtomic *>((*se2)[1]);
	assert(at);
	tackleExpires = at->asInt();

	se2 = dynamic_cast<SExpression *>((*se1)[2]);
	assert(se2);
	at = dynamic_cast<SExpAtomic *>((*se2)[0]);
	assert(at);
	assert(at->asString() == "count");
	at = dynamic_cast<SExpAtomic *>((*se2)[1]);
	assert(at);
	tackleCount = at->asInt();

	// Collision
	if (exp.size() > 16)
	{
		se1 = dynamic_cast<SExpression *>(exp[17]);
		assert(se1);

		at = dynamic_cast<SExpAtomic *>((*se1)[0]);
		assert(at);

		assert(at->asString() == "collision");

		SExpElement *at1 = (*se1)[1];
		assert(at1);

		if (at1->type() == ST_EXPRESSION)
		{
			for (int coll = 1;coll < se1->size();coll++)
			{
				at1 = (*se1)[coll];
				SExpression *tmp = dynamic_cast<SExpression *>(at1);
				assert(tmp);
				at = dynamic_cast<SExpAtomic *>((*tmp)[0]);
				assert(at);
				if (at->asString()[0] == 'b')
				{
					LOG << "collision BALL!" << endl;
					collisionBall = true;
				}
				else if (at->asString()[0] == 'p')
					if (at->asString()[1] == 'l')
					{
						LOG << "collision PLAYER!" << endl;
						collisionPlayer = true;
					}
					else
					{
						assert(at->asString()[1] == 'o');
						LOG << "collision POST!" << endl;
						collisionPost = true;
					}
			}
		}
		else
		{
			at = dynamic_cast<SExpAtomic *>((*se1)[1]);
			assert(at);
			assert(at->asString()[0] == 'n');
			LOG << "collision NONE!" << endl;
			collisionBall = collisionPlayer = collisionPost = false;
		}
	}

	// completing.
//	posDeviation = 0;
//	validPosDeviation = 0;
//	velDeviation = 0;

	relativeHeadDir *= -1;
	speedDir *= -1;
	armTargetDir *= -1;

	headDir = normalizeAngle(bodyDir + relativeHeadDir);

	if (seeSynch)
	{
		if (vmWidth == VMW_NARROW)
			viewAngle = 30;
		else if (vmWidth == VMW_NORMAL)
			viewAngle = 60;
		else if (vmWidth == VMW_WIDE)
			viewAngle = 90;
	}
	else
	{
		if (vmWidth == VMW_NARROW)
			viewAngle = 22.5;
		else if (vmWidth == VMW_NORMAL)
			viewAngle = 45;
		else if (vmWidth == VMW_WIDE)
			viewAngle = 90;
	}
	LOG << "h = " << isCollisionBall() << endl;
}

void Body::update(FieldFlag **flags, FieldGoal **goals, FieldLine **lines,
		unsigned flagsNum, unsigned goalsNum, unsigned linesNum)
{
	float angle;
	unsigned i;
	unsigned farestLine = 0;

	// Updating headDir and bodyDir
	if (linesNum != 0)
	{
		for (i = 1; i < linesNum; i++)
			if (lines[i]->getHeadVec().getMagnitude() >
				lines[farestLine]->getHeadVec().getMagnitude())
				farestLine = i;

		angle = lines[farestLine]->getHeadVec().getDirection();
		headDir = lines[farestLine]->getPrependOrient() -
				((angle < 0) ? (90 + angle) : -(90 - angle));

		headDir = normalizeAngle(headDir);
		bodyDir = normalizeAngle(headDir - relativeHeadDir);
	}

	// Updating Position
	if (flagsNum > 0)
		position = possiblePoints.calculateBodyPosition(*this, (const FieldFlag**)(flags), flagsNum, posDeviation, quantizeStep);
	seePosDeviation = posDeviation;
	setSimCounter(0);

	// Updating Velocity
	velocity.setAsPolar(speedAmount,headDir + speedDir);
}

void Body::simulateByDynamics(bool realBody)
{
	simCounter++;

	position += velocity;
	velocity *= decay;

	posDeviation += 0.5;
	if (posDeviation > MAX_POS_DEV)
		posDeviation = MAX_POS_DEV;

	velDeviation += 0.02;
	if (velDeviation > MAX_VEL_DEV)
		velDeviation = MAX_VEL_DEV;

	if (!realBody)
	{
		stamina += staminaIncMax;
		if (stamina > staminaMax)
			stamina = staminaMax;
	}

	//setCollisionStatus(TA_DONT_KNOW);
	//collisionBall = collisionPlayer = collisionPost = false;
}

void Body::completeParseSenseBody()
{
	// updating velocity and checking for body collision.
	Vector lastVelocity(velocity);
	velocity.setAsPolar(speedAmount,headDir + speedDir);

	/*if (lastVelocity.getMagnitude() > 0.02 ||
		velocity.getMagnitude() > 0.02)
	{
		Vector deltaVelocity(lastVelocity - velocity);

//LOG << "COLL: deltaVelocity: " << deltaVelocity << endl;
//LOG << "COLL: velocity.getMagnitude() / 5.0: "
//	<< velocity.getMagnitude() / 5.0 << endl;
//LOG << "COLL: deltaDir: " << getDeltaAngle(lastVelocity.getDirection(),
//					velocity.getDirection()) << endl;

		if (deltaVelocity.getMagnitude() >
				fmax(velocity.getMagnitude() / 5.0, 0.05) ||
			fabs(getDeltaAngle(lastVelocity.getDirection(),
					velocity.getDirection())) > 45)
		{
			setCollisionStatus(TA_YES);
			LOG << "*** Body Collision ***" << endl;
		}
		else
			setCollisionStatus(TA_NO);
	}
	else
		setCollisionStatus(TA_DONT_KNOW);*/

	// updating body pointing dir
	pointingDir = Degree::normalizeAngle(headDir + armTargetDir);
}

void Body::simulateByAction(const Command *bodyCycleCommand, bool realBody)
{
	// Dash
	if (dynamic_cast<const DashCommand *>(bodyCycleCommand))
	{
		Vector dashAcclerate;
		const DashCommand *dashCommand =
				dynamic_cast<const DashCommand *>(bodyCycleCommand);
		float dir = dashCommand->getDirection();
		float dirRate = (fabs(dir) > 90.0 ? backDashRate - ((backDashRate - sideDashRate) * (1.0 - (fabs(dir) - 90.0) / 90.0))
                            : sideDashRate + ((1.0 - sideDashRate) * (1.0 - fabs(dir) / 90.0)));
                dirRate = min(max(0.f, dirRate), 1.f);
		dashAcclerate.setAsPolar(dashCommand->getPower() * dirRate * dashPowerRate * effort, bodyDir - dir);
		if (dashAcclerate.getMagnitude() > accelMax)
			dashAcclerate.setAsPolar(accelMax, dashAcclerate.getDirection());

		velocity += dashAcclerate;
		if (velocity.getMagnitude() > speedMax)
			velocity.setAsPolar(speedMax, velocity.getDirection());

		/*LOG << "YYY Player Type = " << getType() << " speedMax = " << speedMax << " velocity = " << velocity.getMagnitude()
		<< " dashAccel = " << dashAcclerate.getMagnitude() << " Power = " << dashCommand->getPower() << " Rate = " << dashPowerRate
		<< " effort = " << effort << endl;*/

		if (!realBody)
		{
			if (dashCommand->getPower() >= 0)
				stamina -= dashCommand->getPower();
			else
				stamina += 2 * dashCommand->getPower();

			if (stamina < 0)
				stamina = 0;
		}
	}

	// Turn
	else if (dynamic_cast<const TurnCommand *>(bodyCycleCommand))
	{
		float turnAngle;
		const TurnCommand *turnCommand =
				dynamic_cast<const TurnCommand *>(bodyCycleCommand);

		turnAngle = turnCommand->getDirection();

		// CHECK: check maxMoment & minMoment
		if (turnAngle > maxMoment ||
			turnAngle < minMoment)
			assert(0);

		// Actual Turn Angle formula.
		turnAngle = turnAngle /
					(1 + inertiaMoment * velocity.getMagnitude());
		assert(turnAngle <= maxMoment);
		assert(turnAngle >= minMoment);

		bodyDir = normalizeAngle(bodyDir + turnAngle);
		headDir = normalizeAngle(bodyDir + relativeHeadDir);
	}

	// Move
	else if (dynamic_cast<const MoveCommand *>(bodyCycleCommand))
	{
		const MoveCommand *moveCommand =
				dynamic_cast<const MoveCommand *>(bodyCycleCommand);

		position.setAsCartesian(moveCommand->getX(), moveCommand->getY());
	}

	// Tackle
	else if (dynamic_cast<const TackleCommand *>(bodyCycleCommand))
	{
//		const TackleCommand *tackleCommand =
//				dynamic_cast<const TackleCommand *>(bodyCycleCommand);
		// TODO:
	}

	// PointTo
/*	else if (typeid(*bodyCycleCommand) == typeid(PointToCommand))
	{
		const PointToCommand *pointToCommand =
				dynamic_cast<const PointToCommand *>(bodyCycleCommand);
		// TODO:
	}*/
}

void Body::setServerParamVars(const Param &serverParam)
{
	Player::setServerParamVars(serverParam);

	visibleDistance = serverParam["visible_distance"].asFloat();
}

float Body::viewWidthToViewAngle(ViewModeWidth width) const
{
	if (seeSynch)
	{
		switch (width)
		{
		case VMW_NARROW:
			return 30;
		case VMW_NORMAL:
			return 60;
		case VMW_WIDE:
			return 90;
		}
	}
	else
	{
		switch (width)
		{
		case VMW_NARROW:
			return 22.5;
		case VMW_NORMAL:
			return 45;
		case VMW_WIDE:
			return 90;
		}
	}


	return 0;
}

// Counting functions
bool Body::isKickCountChanged() const
{
	if (kickCount == lastKickCount)
		return false;
	return true;
}

bool Body::isDashCountChanged() const
{
	if (dashCount == lastDashCount)
		return false;
	return true;
}

bool Body::isTurnCountChanged() const
{
	if (turnCount == lastTurnCount)
		return false;
	return true;
}

bool Body::isSayCountChanged() const
{
	if (sayCount == lastSayCount)
		return false;
	return true;
}

bool Body::isTurnNeckCountChanged() const
{
	if (turnNeckCount == lastTurnNeckCount)
		return false;
	return true;
}

bool Body::isCatchCountChanged() const
{
	if (catchCount == lastCatchCount)
		return false;
	return true;
}

bool Body::isMoveCountChanged() const
{
	if (moveCount == lastMoveCount)
		return false;
	return true;
}

bool Body::isChangeViewCountChanged() const
{
	if (changeViewCount == lastChangeViewCount)
		return false;
	return true;
}

bool Body::isArmExpiresChanged() const
{
	if (armExpires == lastArmExpires)
		return false;
	return true;
}

bool Body::isArmCountChanged() const
{
	if (armCount == lastArmCount)
		return false;
	return true;
}

bool Body::isFocusCountChanged() const
{
	if (focusCount == lastFocusCount)
		return false;
	return true;
}

bool Body::isTackleExpiresChanged() const
{
	if (tackleExpires == lastTackleExpires)
		return false;
	return true;
}

bool Body::isTackleCountChanged() const
{
	if (tackleCount == lastTackleCount)
		return false;
	return true;
}

// Getting functions
float Body::getViewAngle() const
{
	return viewAngle;
}

/*TriAnswer Body::getCollisionStatus() const
{
	return collisionStatus;
}*/

float Body::getEffort() const
{
	return effort;
}

float Body::getSpeedAmount() const
{
	return speedAmount;
}

float Body::getSpeedDir() const
{
	return speedDir;
}

float Body::getArmTargetDist() const
{
	return armTargetDist;
}

float Body::getArmTargetDir() const
{
	return armTargetDir;
}

unsigned Body::getKickCount() const
{
	return kickCount;
}

unsigned Body::getDashCount() const
{
	return dashCount;
}

unsigned Body::getTurnCount() const
{
	return turnCount;
}

unsigned Body::getSayCount() const
{
	return sayCount;
}

unsigned Body::getTurnNeckCount() const
{
	return turnNeckCount;
}

unsigned Body::getCatchCount() const
{
	return catchCount;
}

unsigned Body::getMoveCount() const
{
	return moveCount;
}

unsigned Body::getSenseBodyCycle() const
{
	return senseBodyCycle;
}

unsigned Body::getChangeViewCount() const
{
	return changeViewCount;
}

unsigned Body::getArmMovable() const
{
	return armMovable;
}

unsigned Body::getArmExpires() const
{
	return armExpires;
}

unsigned Body::getArmCount() const
{
	return armCount;
}

unsigned Body::getTackleExpires() const
{
	return tackleExpires;
}

unsigned Body::getTackleCount() const
{
	return tackleCount;
}

unsigned Body::getFocusTargetUniNum() const
{
	return focusTargetUniNum;
}

unsigned Body::getFocusCount() const
{
	return focusCount;
}

TeamId Body::getFocusTargetTeamId() const
{
	return focusTargetTeamId;
}

ViewModeWidth Body::getVMWidth() const
{
	return vmWidth;
}

ViewModeQuality Body::getVMQuality() const
{
	return vmQuality;
}

float Body::getVisibleDistance() const
{
	return visibleDistance;
}

// Setting functions
void Body::setViewAngle(float viewAngleArg)
{
	viewAngle = viewAngleArg;
}

/*void Body::setCollisionStatus(TriAnswer collisionStatusArg)
{
	collisionStatus = collisionStatusArg;
}
*/
void Body::setVMWidth(ViewModeWidth vmWidthArg)
{
	vmWidth = vmWidthArg;

	if (seeSynch)
	{
		if (vmWidth == VMW_NARROW)
			viewAngle = 30;
		else if (vmWidth == VMW_NORMAL)
			viewAngle = 60;
		else if (vmWidth == VMW_WIDE)
			viewAngle = 90;
	}
	else
	{
		if (vmWidth == VMW_NARROW)
			viewAngle = 22.5;
		else if (vmWidth == VMW_NORMAL)
			viewAngle = 45;
		else if (vmWidth == VMW_WIDE)
			viewAngle = 90;
	}
}

void Body::setVMQuality(ViewModeQuality vmQualityArg)
{
	vmQuality = vmQualityArg;
}

void Body::setSynch(bool synch)
{
	seeSynch = synch;
}

bool Body::isSynch() const
{
	return seeSynch;
}

bool Body::isCollision() const
{
	return collisionPost || collisionPlayer || collisionBall;
}

bool Body::isCollisionBall() const
{
	return collisionBall;
}

bool Body::isCollisionPlayer() const
{
	return collisionPlayer;
}

bool Body::isCollisionPost() const
{
	return collisionPost;
}

void Body::setEffort(float effortArg)
{
	effort = effortArg;
}

float Body::getBattery() const
{
	return battery;
}

