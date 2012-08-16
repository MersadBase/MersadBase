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
 *  	and is modified by: Pooria Kaviani
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <cassert>

#include <Config.h>
#include <Degree.h>
#include <Logger.h>
#include <Object.h>
#include <Defines.h>
#include <SExpression.h>

using namespace std;
using namespace Degree;

// class Object

Object::Object()
{
	seeTime = 0;
	velSeeTime = 0;
	posDeviation = MAX_POS_DEV;
	seePosDeviation = MAX_POS_DEV;
	velDeviation = MAX_VEL_DEV;

	readyToUpdateFlag = false;
}

Object::~Object()
{
}

void Object::parse(const SExpression &exp, unsigned curTime)
{
	SExpAtomic *at;

	seeDistChange = NOVALUE;
	seeDirChange = -NOVALUE;
	seeDirection = -NOVALUE;

	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);
	seeDistance = at->asFloat();

	unsigned decreaseNum = 0;
	at = dynamic_cast<SExpAtomic *>(exp[exp.size() - 1]);
	if (at->toString() == "t" or at->toString() == "k")
		decreaseNum = 1;

	if (exp.size() - decreaseNum > 2)
	{
		at = dynamic_cast<SExpAtomic *>(exp[2]);
		assert(at);
		seeDirection = at->asFloat();
	}

	if (exp.size() - decreaseNum > 4)
	{
		at = dynamic_cast<SExpAtomic *>(exp[3]);
		assert(at);
		if (at->asFloat() < 100)
			seeDistChange = at->asFloat();
		else
			LOG << "seeDistChange > 100 !!" << endl;

		at = dynamic_cast<SExpAtomic *>(exp[4]);
		assert(at);
		seeDirChange = at->asFloat();

		velSeeTime = curTime;
	}

	seeDirection *= -1;
	seeDirChange *= -1;

	seeTime = curTime;

	readyToUpdateFlag = true;
}

void Object::setServerParamVars(const Param &serverParam)
{
	maxMoment = serverParam["maxmoment"].asFloat();
	minMoment = serverParam["minmoment"].asFloat();
	maxPower = serverParam["maxpower"].asFloat();
	minPower = serverParam["minpower"].asFloat();
	quantizeStep = serverParam["quantize_step"].asFloat();
	quantizeStepL = serverParam["quantize_step_l"].asFloat();
}

float Object::getDistance(const Object &obj) const
{
	return hypot(getPos().getX() - obj.getPos().getX(),
				 getPos().getY() - obj.getPos().getY());
}

float Object::getDistance(const Point &point) const
{
	return hypot(getPos().getX() - point.x,
				 getPos().getY() - point.y);
}

void Object::kill()
{
	posDeviation = MAX_POS_DEV;
	seePosDeviation = MAX_POS_DEV;
	velDeviation = MAX_VEL_DEV;

	seeTime = 0;
	velSeeTime = 0;
}

// Getting functions
bool Object::isReadyToUpdate() const
{
	return readyToUpdateFlag;
}

float Object::getPosDeviation() const
{
	return posDeviation;
}

float Object::getSeePosDeviation() const
{
	return seePosDeviation;
}

float Object::getVelDeviation() const
{
	return velDeviation;
}

float Object::getSeeDistance() const
{
	return seeDistance;
}

float Object::getSeeDirection() const
{
	return seeDirection;
}

float Object::getSeeDistChange() const
{
	return seeDistChange;
}

float Object::getSeeDirChange() const
{
	return seeDirChange;
}

unsigned Object::getSeeTime() const
{
	return seeTime;
}

unsigned Object::getVelSeeTime() const
{
	return velSeeTime;
}

float Object::getAccelMax() const
{
	return accelMax;
}

float Object::getDecay() const
{
	return decay;
}

float Object::getRand() const
{
	return rand;
}

float Object::getSize() const
{
	return size;
}

float Object::getSpeedMax() const
{
	return speedMax;
}

float Object::getMaxMoment() const
{
	return maxMoment;
}

float Object::getMinMoment() const
{
	return minMoment;
}

float Object::getMaxPower() const
{
	return maxPower;
}

float Object::getMinPower() const
{
	return minPower;
}

float Object::getQuantizeStep() const
{
	return quantizeStep;
}

float Object::getQuantizeStepL() const
{
	return quantizeStepL;
}

const Vector &Object::getPos() const
{
	return (const Vector &)position;
}

const Vector &Object::getVel() const
{
	return (const Vector &)velocity;
}

const Vector &Object::getAbsVec() const
{
	return (const Vector &)absVector;
}

const Vector &Object::getBodyVec() const
{
	return (const Vector &)bodyVector;
}

const Vector &Object::getHeadVec() const
{
	return (const Vector &)headVector;
}

// Setting functions
void Object::setReadyToUpdate(bool readyToUpdateFlagArg)
{
	readyToUpdateFlag = readyToUpdateFlagArg;
}

void Object::setPosDeviation(float posDeviationArg)
{
	posDeviation = posDeviationArg;
}

void Object::setSeePosDeviation(float seePosDeviationArg)
{
	seePosDeviation = seePosDeviationArg;
}

void Object::setVelDeviation(float velDeviationArg)
{
	velDeviation = velDeviationArg;
}

void Object::setSeeDistance(float seeDistanceArg)
{
	seeDistance = seeDistanceArg;
}

void Object::setSeeDirection(float seeDirectionArg)
{
	seeDirection = seeDirectionArg;
}

void Object::setSeeDistChange(float seeDistChangeArg)
{
	seeDistChange = seeDistChangeArg;
}

void Object::setSeeDirChange(float seeDirChangeArg)
{
	seeDirChange = seeDirChangeArg;
}

void Object::setSeeTime(unsigned seeTimeArg)
{
	seeTime = seeTimeArg;
}

void Object::setVelSeeTime(unsigned velSeeTimeArg)
{
	velSeeTime = velSeeTimeArg;
}

void Object::setDecay(float decayArg)
{
	decay = decayArg;
}

Vector &Object::setPos()
{
	return (Vector &)position;
}

Vector &Object::setVel()
{
	return (Vector &)velocity;
}

Vector &Object::setAbsVec()
{
	return (Vector &)absVector;
}

Vector &Object::setBodyVec()
{
	return (Vector &)bodyVector;
}

Vector &Object::setHeadVec()
{
	return (Vector &)headVector;
}


// class Flag

void FieldFlag::parse(int idArg, const SExpression &exp, unsigned curTime)
{
	Object::parse(exp, curTime);

	id = idArg;
}

void FieldFlag::update(const FieldData &fieldData)
{
	Point point;

	point = fieldData.getFlagPos(id);
	position.setAsCartesian(point.x,point.y);
	headVector.setAsPolar(seeDistance,seeDirection);
	posDeviation = seeDistance / 20;
	seePosDeviation = posDeviation;
}

int FieldFlag::getId() const
{
	return id;
}


// class Line

void FieldLine::parse(int idArg, const SExpression &exp, unsigned curTime)
{
	Object::parse(exp, curTime);

	id = idArg;
}

void FieldLine::update(const FieldData &fieldData)
{
	Point point;

	point = fieldData.getLinePos(id);
	position.setAsCartesian(point.x,point.y);
	headVector.setAsPolar(seeDistance,seeDirection);
	posDeviation = seeDistance / 20;
	seePosDeviation = posDeviation;

	prependOrient = fieldData.getPrependOrient(id);
}

int FieldLine::getId() const
{
	return id;
}

float FieldLine::getPrependOrient() const
{
	return prependOrient;
}


// class Goal

void FieldGoal::parse(int idArg, const SExpression &exp, unsigned curTime)
{
	Object::parse(exp, curTime);

	id = idArg;
}

void FieldGoal::parseFullState(int idArg, const SExpression &exp,
	   unsigned curTime)
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

	seeTime = curTime;
	id = idArg;
}

void FieldGoal::update(const FieldData &fieldData)
{
	Point point;

	point = fieldData.getGoalPos(id);
	position.setAsCartesian(point.x,point.y);
	headVector.setAsPolar(seeDistance,seeDirection);
	posDeviation = seeDistance / 20;
	seePosDeviation = posDeviation;
}

int FieldGoal::getId() const
{
	return id;
}
