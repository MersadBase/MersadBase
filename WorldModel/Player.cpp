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
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#include <cassert>
#include <cmath>

#include <Command.h>
#include <Body.h>
#include <Config.h>
#include <Logger.h>
#include <Player.h>
#include <Degree.h>
#include <Basics.h>
#include <Defines.h>
#include <SExpression.h>
#include <PossiblePoints.h>

using namespace std;

Player::Player(PlayerModel model) :
		model(model)
{
	goalieFlag = false;
	tacklingFlag = false;
	kickingFlag = false;
	foulChargedFlag = false;
	pointingDir = 0;
	bodyDir = 0;
	headDir = 0;
	uniNum = 0;
//	postNum = 0;
	type = 0;
	teamId = TID_TEAMMATE;

	backDashRate = 0.f;
	dashAngleStep = 0.f;
	maxDashAngle = 0.f;
	minDashAngle = 0.f;
	maxDashPower = 0.f;
	minDashPower = 0.f;
	sideDashRate = 0.f;
	staminaCapacity = 0.f;

	relativePointingDir = 0;
	relativeBodyDir = 0;
	relativeHeadDir = 0;

	setSimCounter(0xFFFF);
	setBodyDirCounter(0xFFFF);
	lastAPVRTime = 0;

	dashExpires = 0;

	updatedByHear = false;
}

Player::~Player()
{
}

void Player::kill()
{
	Object::kill();

	setSimCounter(0xFFFF);
	setBodyDirCounter(0xFFFF);
}

void Player::parse(TeamId teamIdArg, unsigned uniNumArg, bool isGoalie, const SExpression &exp,
		unsigned curTime)
{
	SExpAtomic *at;

	tacklingFlag = false;
	kickingFlag = false;
	foulChargedFlag = false;
	relativeBodyDir = -NOVALUE;
	relativeHeadDir = -NOVALUE;
	relativePointingDir = -NOVALUE;

	Object::parse(exp, curTime);

	goalieFlag = isGoalie;
	setUniNum(uniNumArg);
	teamId = teamIdArg;

	if (teamId == TID_UNKNOWN)
		model = PLM_QUARTER;
	else if (uniNum == 0)
		model = PLM_HALF;
	else
		model = PLM_FULL;

	unsigned decreaseNum = 0;
	at = dynamic_cast<SExpAtomic *>(exp[exp.size() - 1]);
	if (at->toString() == "t")
		decreaseNum = 1;

	if (exp.size() - decreaseNum > 6)
	{
		at = dynamic_cast<SExpAtomic *>(exp[5]);
		assert(at);
		relativeBodyDir = at->asFloat();
		bodyDirCounter = 0;

		at = dynamic_cast<SExpAtomic *>(exp[6]);
		assert(at);
		relativeHeadDir = at->asFloat();
	}

	if (model != PLM_QUARTER)
	{
		at = dynamic_cast<SExpAtomic *>(exp[exp.size() - 1]);
		assert(at);
		if (at->toString() == "t")
			tacklingFlag = true;
		else if (at->toString() == "k")
			kickingFlag = true;
		else if (at->toString() == "f")
			foulChargedFlag = true;
	}

	if ((exp.size() - decreaseNum) % 2 == 0) // see format is very very poor in this case.
	{
		if (tacklingFlag or kickingFlag)
			at = dynamic_cast<SExpAtomic *>(exp[exp.size() - 2]);
		else
			at = dynamic_cast<SExpAtomic *>(exp[exp.size() - 1]);
		assert(at);
		relativePointingDir = at->asFloat();
	}

	relativeBodyDir *= -1;
	relativeHeadDir *= -1;
	relativePointingDir *= -1;
}

void Player::parseFullState(TeamId teamIdArg, unsigned uniNumArg, bool isGoalieArg,
		const SExpression &exp, unsigned curTime)
{
	SExpAtomic *at;
	float x, y;

	seeDistChange = NOVALUE;
	seeDirChange = -NOVALUE;
	seeDirection = -NOVALUE;

	teamId = teamIdArg;
	uniNum = uniNumArg;

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

	at = dynamic_cast<SExpAtomic *>(exp[5]);
	assert(at);
	bodyDir = at->asFloat() * (-1);
	bodyDirCounter = 0;

	at = dynamic_cast<SExpAtomic *>(exp[6]);
	assert(at);
	headDir = at->asFloat() * (-1);

	tacklingFlag = false;
	kickingFlag = false;
	foulChargedFlag = false;
	pointingDir = 0;

	if (exp.size() >= 8)
	{
		at = dynamic_cast<SExpAtomic *>(exp[7]);
		assert(at);
		if (at->toString() == "t")
		{
			tacklingFlag = true;
		}
		else if (at->toString() == "k")
		{
			kickingFlag = true;
		}
		else if (at->toString() == "f")
		{
			foulChargedFlag = true;
		}
		else if (at->toString() == "y")
		{
			card = CM_YELLOW;
		}
		else
		{
			pointingDir = at->asFloat() * (-1);
		}
	}

	if (exp.size() >= 9)
	{
		at = dynamic_cast<SExpAtomic *>(exp[8]);
		assert(at);
		if (at->toString() == "t")
		{
			tacklingFlag = true;
		}
		else if (at->toString() == "k")
		{
			kickingFlag = true;
		}
		else if (at->toString() == "f")
		{
			foulChargedFlag = true;
		}
		else if (at->toString() == "y")
		{
			card = CM_YELLOW;
		}
	}

	if (exp.size() >= 10)
	{
		at = dynamic_cast<SExpAtomic *>(exp[9]);
		assert(at);
		if (at->toString() == "t")
		{
			tacklingFlag = true;
		}
		else if (at->toString() == "k")
		{
			kickingFlag = true;
		}
		else if (at->toString() == "f")
		{
			foulChargedFlag = true;
		}
		else if (at->toString() == "y")
		{
			card = CM_YELLOW;
		}
	}

	goalieFlag = isGoalieArg;
	posDeviation = 0;
	model = PLM_FULL;

	seeTime = curTime;
}

void Player::setServerParamVars(const Param &serverParam)
{
	Object::setServerParamVars(serverParam);

	accelMax = serverParam["player_accel_max"].asFloat();
	decay = serverParam["player_decay"].asFloat();
	rand = serverParam["player_rand"].asFloat();
	size = serverParam["player_size"].asFloat();
	speedMax = serverParam["player_speed_max"].asFloat();

	staminaMax = serverParam["stamina_max"].asFloat();
	effortDecThr = serverParam["effort_dec_thr"].asFloat();
	recoverDecThr = serverParam["recover_dec_thr"].asFloat();
	maxNeckAng = serverParam["maxneckang"].asFloat();
	minNeckAng = serverParam["minneckang"].asFloat();
	maxNeckMoment = serverParam["maxneckmoment"].asFloat();
	minNeckMoment = serverParam["minneckmoment"].asFloat();
	catchableAreaL = serverParam["catchable_area_l"].asFloat();
	catchableAreaW = serverParam["catchable_area_w"].asFloat();

	backDashRate = serverParam["back_dash_rate"].asFloat(); // AAAAAAAAAAAAAA
	dashAngleStep = serverParam["dash_angle_step"].asFloat();
	maxDashAngle = serverParam["max_dash_angle"].asFloat();
	minDashAngle = serverParam["min_dash_angle"].asFloat();
	maxDashPower = serverParam["max_dash_power"].asFloat();
	minDashPower = serverParam["min_dash_power"].asFloat();
	sideDashRate = serverParam["side_dash_rate"].asFloat();
	staminaCapacity = serverParam["stamina_capacity"].asFloat();
}

void Player::update(const Body &body, PlayMode playMode)
{
	position = possiblePoints.calculatePlayerPosition(*this, body, posDeviation, quantizeStep);
	seePosDeviation = posDeviation;

	absVector.setByPoints(body.getPos(), position);
	bodyVector = absVector;
	bodyVector.rotate(-body.getBodyDir());
	headVector = absVector;
	headVector.rotate(-body.getHeadDir());

	setSimCounter(0);

	if (relativeBodyDir != NOVALUE)
	{
		bodyDir = Degree::normalizeAngle(body.getHeadDir() + relativeBodyDir);
		headDir = Degree::normalizeAngle(body.getHeadDir() + relativeHeadDir);
	}

	if (seeDistChange != NOVALUE)
	{
		velocity.setByDistDirChange(headVector, seeDistChange, seeDirChange);
		velocity.rotate(body.getHeadDir());
		velocity += body.getVel();
		seenVel = velocity;
		seenPos = position;
		seenCounter = 0;
	}

	if (relativePointingDir != NOVALUE)
		pointingDir = Degree::normalizeAngle(body.getHeadDir() + relativePointingDir);
	else
		pointingDir = NOVALUE;

	float maxSenseStamina;
	if (playMode == PM_PLAY_ON)
		maxSenseStamina = 6000;
	else
		maxSenseStamina = 7000;

	if (pointingDir != NOVALUE and teamId == TID_TEAMMATE)
		stamina = Basics::reRate(Degree::absoluteAngle(pointingDir), 0, 270, 2400, maxSenseStamina,
				RRM_DIRECT);
	else
		stamina = staminaMax;

	executeAPVR(body);
	if (bodyDirCounter)
		executeAPDR(body);

	if (seeTime == velSeeTime && velocity.getMagnitude() > 0.2)
	{
		LOG << "APMR: started for player " << teamId << " " << uniNum << "." << endl;
		dashExpires = 2;
	}

	updatedByHear = false;
}

void Player::simulateByDynamics(const Body &body)
{
	seenCounter++;
	simCounter++;
	bodyDirCounter++;

	/*	if (dashExpires > 0 && bodyDir != NOVALUE)
	 {
	 LOG << "APMR: accelerated for player " << teamId << " " << uniNum
	 << "." << endl;

	 Vector accel;
	 accel.setAsPolar(0.6, bodyDir);

	 velocity += accel;
	 if (velocity.getMagnitude() > getSpeedMax())
	 velocity.setAsPolar(getSpeedMax(), velocity.getDirection());
	 }
	 */
	position += velocity;
	velocity *= decay;

	float posDevInc = 0.8 + (simCounter - 1) * 0.2;
//	if (posDevInc > 1.0)
	posDevInc = 1.0;

	posDeviation += posDevInc;
	if (posDeviation > MAX_POS_DEV)
		posDeviation = MAX_POS_DEV;

	seePosDeviation += posDevInc;
	if (seePosDeviation > MAX_POS_DEV)
		seePosDeviation = MAX_POS_DEV;

	velDeviation += 0.02;
	if (velDeviation > MAX_VEL_DEV)
		velDeviation = MAX_VEL_DEV;

	absVector.setByPoints(body.getPos(), position);
	bodyVector = absVector;
	bodyVector.rotate(-body.getBodyDir());
	headVector = absVector;
	headVector.rotate(-body.getHeadDir());

//	bodyDir = headDir = NOVALUE;

	if (dashExpires > 0)
		dashExpires--;

	//stamina += 45; // this is just an approximate amount.
	if (stamina > 8000)
		stamina = 8000;
}

void Player::simulateByAction(const Command *bodyCycleCommand, bool realBody)
{
	// Dash
	if (dynamic_cast<const DashCommand *>(bodyCycleCommand))
	{
		Vector dashAcclerate;
		const DashCommand *dashCommand = dynamic_cast<const DashCommand *>(bodyCycleCommand);

		dashAcclerate.setAsPolar(dashCommand->getPower() * dashPowerRate * 1/* effort */, bodyDir);
		if (dashAcclerate.getMagnitude() > accelMax)
			dashAcclerate.setAsPolar(accelMax, dashAcclerate.getDirection());

		velocity += dashAcclerate;
		if (velocity.getMagnitude() > speedMax)
			velocity.setAsPolar(speedMax, velocity.getDirection());

		/*LOG << "YYY Player Type = " << getType() << " speedMax = " << speedMax << " velocity = " << velocity.getMagnitude()
		 << " dashAccel = " << dashAcclerate.getMagnitude() << " Power = " << dashCommand->getPower() << " Rate = " << dashPowerRate << endl;*/

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
}

void Player::virtualUpdate(const Body &body)
{
	position = body.getPos() + absVector;
}

void Player::setByAnotherPlayer(const Player &player, const Body &body)
{
	// Object vars
	posDeviation = player.getPosDeviation();
	seePosDeviation = player.getSeePosDeviation();
	velDeviation = player.getVelDeviation();
	seeDistance = player.getSeeDistance();
	seeDirection = player.getSeeDirection();
	seeDistChange = player.getSeeDistChange();
	seeDirChange = player.getSeeDirChange();
	seeTime = player.getSeeTime();
	velSeeTime = player.getVelSeeTime();

	position = player.getPos();
	velocity = player.getVel();
	absVector = player.getAbsVec();
	bodyVector = player.getBodyVec();
	headVector = player.getHeadVec();

	// Player vars
	goalieFlag = isGoalie() | player.isGoalie();
	tacklingFlag = player.isTackling();
	kickingFlag = player.isKicking();
	foulChargedFlag = player.isFoulCharged();
	stamina = player.getStamina();
	bodyDir = player.getBodyDir();
	headDir = player.getHeadDir();
	pointingDir = player.getPointingDir();
	relativeBodyDir = player.getRelativeBodyDir();
	relativeHeadDir = player.getRelativeHeadDir();
	relativePointingDir = player.getRelativePointingDir();

	card = player.getCard();
	tackleRemainCycle = player.getTackleRemainCycle();
	lastStandCycle = player.getLastStandCycle();

	simCounter = player.getSimCounter();
	bodyDirCounter = player.getBodyDirCounter();
	dashExpires = player.getDashExpires();

	// last* variables do not be copied.

	if (model == PLM_FULL && player.getModel() == PLM_FULL)
	{
		setUniNum(player.getUniNum());
		type = player.getType();

		staminaIncMax = player.getStaminaIncMax();
		inertiaMoment = player.getInertiaMoment();
		dashPowerRate = player.getDashPowerRate();
		kickableMargin = player.getKickableMargin();
		kickRand = player.getKickRand();
		extraStamina = player.getExtraStamina();
		effortMax = player.getEffortMax();
		effortMin = player.getEffortMin();
	}

	if (model >= PLM_HALF && player.getModel() >= PLM_HALF)
		teamId = player.getTeamId();

	// some update functions
	executeAPVR(body);
	if (bodyDirCounter)
		executeAPDR(body);
}

bool Player::isBody() const
{
	if (dynamic_cast<const Body *>(this))
		return true;
	return false;
}

bool Player::isAlive() const
{
	if (model != PLM_FULL)
		return isValid();

	if (simCounter < PLAYER_ALIVE_CYCLE or simCounter == 65535)
		return true;
	return false;
}

bool Player::isValid() const
{
	if (simCounter < PLAYER_VALID_CYCLE/* or (model == PLM_FULL and teamId == TID_OPPONENT and goalieFlag)*/)
		return true;
	return false;
}

bool Player::isValid(unsigned validCycle) const
{
	if (simCounter
			< validCycle/* or (model == PLM_FULL and teamId == TID_OPPONENT and goalieFlag)*/)
		return true;
	return false;
}

void Player::executeAPVR(const Body &body)
{
	// Advance Player Velocity Recognizer (APVR)
	if (model == PLM_FULL && seeDistChange == NOVALUE && absVector.getMagnitude() < APVR_RADIUS
			&& !body.isCollisionPlayer() && lastAPVRTime == seeTime - 1)
	{
		Vector lastBodyVel(body.getVel());
		lastBodyVel /= body.getDecay();

		Vector virtualVel(absVector - lastAPVRPosition + lastBodyVel);
		virtualVel *= getDecay();

		velocity = virtualVel;
		velSeeTime = seeTime; // it must be in Parsing but it is an exeption.

		LOG << "APVR: executed for player " << teamId << " " << uniNum << "." << endl;
	}

	if (absVector.getMagnitude() < APVR_RADIUS)
	{
		lastAPVRPosition = absVector;
		lastAPVRTime = seeTime;
	}
	else
		lastAPVRTime = 0;
}

void Player::executeAPDR(const Body &body)
{
	// Advanced Player Direction Recognizer (APDR)
	if (model == PLM_FULL && velSeeTime == seeTime && lastAPDRTime == seeTime - 1)
	{
		Vector Accelerate(velocity);
		Accelerate /= getDecay();
		Accelerate -= lastAPDRVelocity;

		if (Accelerate.getMagnitude() > 0.15)
		{
			bodyDir = Accelerate.getDirection();
			setBodyDirCounter(0);
			LOG << "APDR: executed for player " << teamId << " " << uniNum << "." << endl;
		}
	}

	if (velSeeTime == seeTime)
	{
		lastAPDRVelocity = velocity;
		lastAPDRTime = seeTime;
	}
	else
		lastAPDRTime = 0;
}

bool Player::canBodySee(const Vector &playerPos, float viewAngle, float visibleDist) const
{
	float farLength;

	if (model == PLM_FULL)
		farLength = UNUM_FAR_LENGTH;
	else if (model == PLM_HALF)
		farLength = TEAM_FAR_LENGTH;
	else
		farLength = 0xFFFF;

	if (absVector.getMagnitude() + 0.75 * posDeviation < visibleDist)
		return true;

	if (abs(headVector.getDirection())
			+ Degree::arcTan(0.75 * posDeviation / absVector.getMagnitude()) < viewAngle
			&& absVector.getMagnitude() + 0.75 * posDeviation < farLength)
		return true;

	return false;
}

unsigned Player::uniNumToPostNum(unsigned uniNum)
{
	switch (uniNum)
	{
	case 2:
		return 11;
	case 3:
		return 9;
	case 4:
		return 10;
	case 5:
		return 8;
	case 6:
		return 5;
	case 7:
		return 4;
	case 8:
		return 3;
	case 9:
		return 2;
	case 10:
		return 6;
	case 11:
		return 7;
	}

	return uniNum;
}

unsigned Player::postNumToUniNum(unsigned postNum)
{
	switch (postNum)
	{
	case 2:
		return 9;
	case 3:
		return 8;
	case 4:
		return 7;
	case 5:
		return 6;
	case 6:
		return 10;
	case 7:
		return 11;
	case 8:
		return 5;
	case 9:
		return 3;
	case 10:
		return 4;
	case 11:
		return 2;
	}

	return postNum;
}

void Player::updateByHear(float x, float y, float velMag, float velDir, const Body &body)
{
	LOG << "Player::updateByHear" << endl;

	position.setAsCartesian(x, y);
	velocity.setAsPolar(velMag, velDir);

	posDeviation = velocity.getMagnitude() / 2;
	velDeviation = 0;

	setSimCounter(0);

	// Setting vectors
	absVector.setByPoints(body.getPos(), position);
	bodyVector = absVector;
	bodyVector.rotate(-body.getBodyDir());
	headVector = absVector;
	headVector.rotate(-body.getHeadDir());

	updatedByHear = true;
}

void Player::updateByRadar(float magnitude, float direction, const Body &body, const Ball &ball)
{
//	LOG << "Player::updateByRadar" << endl;

	Vector vec;
	vec.setAsPolar(magnitude, direction);

	position = ball.getPos() + vec;

	posDeviation = 1.00; // ?
	velDeviation = 0;

	setSimCounter(0);

	// Setting vectors
	absVector.setByPoints(body.getPos(), position);
	bodyVector = absVector;
	bodyVector.rotate(-body.getBodyDir());
	headVector = absVector;
	headVector.rotate(-body.getHeadDir());

}

// Heterogeneous functions
void Player::setType(int typeArg, const Param &playerType)
{
	type = typeArg;

	// heterogeneous items
	staminaIncMax = playerType["stamina_inc_max"].asFloat();
	inertiaMoment = playerType["inertia_moment"].asFloat();
	dashPowerRate = playerType["dash_power_rate"].asFloat();
	kickableMargin = playerType["kickable_margin"].asFloat();
	kickRand = playerType["kick_rand"].asFloat();
	extraStamina = playerType["extra_stamina"].asFloat();
	effortMax = playerType["effort_max"].asFloat();
	effortMin = playerType["effort_min"].asFloat();

	// public items
	speedMax = playerType["player_speed_max"].asFloat();
	decay = playerType["player_decay"].asFloat();
	size = playerType["player_size"].asFloat();
}

float Player::getStaminaIncMax() const
{
	return staminaIncMax;
}

float Player::getInertiaMoment() const
{
	return inertiaMoment;
}

float Player::getDashPowerRate() const
{
	return dashPowerRate;
}

float Player::getKickableMargin() const
{
	return kickableMargin;
}

float Player::getKickRand() const
{
	return kickRand;
}

float Player::getExtraStamina() const
{
	return extraStamina;
}

float Player::getEffortMax() const
{
	return effortMax;
}

float Player::getEffortMin() const
{
	return effortMin;
}

// Getting functions
bool Player::isGoalie() const
{
	return goalieFlag;
}

bool Player::isTackling() const
{
	return tacklingFlag;
}

bool Player::isKicking() const
{
	return kickingFlag;
}

bool Player::isFoulCharged() const
{
	return foulChargedFlag;
}

float Player::getStamina() const
{
	return stamina;
}

float Player::getBodyDir() const
{
	return bodyDir;
}

float Player::getHeadDir() const
{
	return headDir;
}

float Player::getPointingDir() const
{
	return pointingDir;
}

float Player::getRelativeBodyDir() const
{
	return relativeBodyDir;
}

float Player::getRelativeHeadDir() const
{
	return relativeHeadDir;
}

float Player::getRelativePointingDir() const
{
	return relativePointingDir;
}

unsigned Player::getType() const
{
	return type;
}

unsigned Player::getUniNum() const
{
	return uniNum;
}

unsigned Player::getPostNum() const
{
	return postNum;
}

TeamId Player::getTeamId() const
{
	return teamId;
}

PlayerModel Player::getModel() const
{
	return model;
}

unsigned Player::getSimCounter() const
{
	return simCounter;
}

unsigned Player::getBodyDirCounter() const
{
	return bodyDirCounter;
}

float Player::getBackDashRate() const
{
	return backDashRate;
}

float Player::getDashAngleStep() const
{
	return dashAngleStep;
}

float Player::getMaxDashAngle() const
{
	return maxDashAngle;
}

float Player::getMinDashAngle() const
{
	return minDashAngle;
}

float Player::getMaxDashPower() const
{
	return maxDashPower;
}

float Player::getMinDashPower() const
{
	return minDashPower;
}

float Player::getSideDashRate() const
{
	return sideDashRate;
}

float Player::getStaminaCapacity() const
{
	return staminaCapacity;
}

float Player::getStaminaMax() const
{
	return staminaMax;
}

float Player::getEffortDecThr() const
{
	return effortDecThr;
}

float Player::getRecoverDecThr() const
{
	return recoverDecThr;
}

float Player::getMaxNeckAng() const
{
	return maxNeckAng;
}

float Player::getMinNeckAng() const
{
	return minNeckAng;
}

float Player::getMaxNeckMoment() const
{
	return maxNeckMoment;
}

float Player::getMinNeckMoment() const
{
	return minNeckMoment;
}

float Player::getCatchableAreaL() const
{
	return catchableAreaL;
}

float Player::getCatchableAreaW() const
{
	return catchableAreaW;
}

unsigned Player::getDashExpires() const
{
	return dashExpires;
}

bool Player::isUpdatedByHear() const
{
	return updatedByHear;
}

Vector Player::getSeenPos() const
{
	return seenPos;
}

Vector Player::getSeenVel() const
{
	return seenVel;
}

unsigned Player::getSeenCounter() const
{
	return seenCounter;
}

CardModel Player::getCard() const
{
	return card;
}

unsigned Player::getTackleRemainCycle() const
{
	return tackleRemainCycle;
}

unsigned Player::getLastStandCycle() const
{
	return lastStandCycle;
}

// Setting functions
void Player::setGoalie(bool goalieFlagArg)
{
	goalieFlag = goalieFlagArg;
}

void Player::setTackling(bool tacklingFlagArg)
{
	tacklingFlag = tacklingFlagArg;
}

void Player::setKicking(bool kickingFlagArg)
{
	kickingFlag = kickingFlagArg;
}

void Player::setFoulCharged(bool foulChargedFlagArg)
{
	foulChargedFlag = foulChargedFlagArg;
}

void Player::setBodyDir(float bodyDirArg)
{
	bodyDir = bodyDirArg;
}

void Player::setHeadDir(float headDirArg)
{
	headDir = headDirArg;
}

void Player::setPointingDir(float pointingDirArg)
{
	pointingDir = pointingDirArg;
}

void Player::setRelativeBodyDir(float relativeBodyDirArg)
{
	relativeBodyDir = relativeBodyDirArg;
}

void Player::setRelativeHeadDir(float relativeHeadDirArg)
{
	relativeHeadDir = relativeHeadDirArg;
}

void Player::setRelativePointingDir(float relativePointingDirArg)
{
	relativePointingDir = relativePointingDirArg;
}

void Player::setType(unsigned typeArg)
{
	type = typeArg;
}

void Player::setUniNum(unsigned uniNumArg)
{
	uniNum = uniNumArg;
	postNum = uniNumToPostNum(uniNum);
}

void Player::setTeamId(TeamId teamIdArg)
{
	teamId = teamIdArg;
}

void Player::setModel(PlayerModel modelArg)
{
	model = modelArg;
}

void Player::setSimCounter(unsigned simCounterArg)
{
	simCounter = simCounterArg;
}

void Player::setBodyDirCounter(unsigned bodyDirCounterArg)
{
	bodyDirCounter = bodyDirCounterArg;
}

void Player::setCard(CardModel cardModel)
{
	card = cardModel;
}

void Player::setLastStandCycle(unsigned cycle)
{
	lastStandCycle = cycle;
}

void Player::setTackleRemainCycle(unsigned cycle)
{
	tackleRemainCycle = cycle;
}

