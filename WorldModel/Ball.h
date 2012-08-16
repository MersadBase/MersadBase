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

#ifndef __BALL_H
#define __BALL_H

#include <Object.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

#define ABVR_RADIUS 7.5
#define BALL_VALID_CYCLE 15

class Body;
class Command;

class Ball: public Object
{
protected:
	Vector seenPos;
	Vector seenVel;
	unsigned seenCounter;

	bool collisionFlag;
	bool absoluteUpdateFlag;
	bool updateByHearPermittedFlag;
	bool updatedByHearFlag;

	unsigned latestSeeTime;

	Vector lastVelocity;

	unsigned simCounter;
	unsigned lastABVRTime;
	Vector lastABVRPosition;

	static unsigned lastCollisionCycle;

	// Local serverparams for Updates and Simulates
	float kickPowerRate;
	float kickRandFactorL;
	float kickRandFactorR;
	float stoppedBallVel;

	float tacklePowerRate;

	void executeABVR(const Body &body);

public:
	Ball();
	virtual ~Ball();

	virtual void kill();
	void update(const Body &body, bool canAVBR, unsigned curCycle);
	void virtualUpdate(const Body &body);
	void updateByHear(float x, float y, float velMag, float velDir, const Body &body);

// این تابع توپ را یک Cycle شبیه سازی می کند. مثلا می خواهید بدانید توپ در Cycle بعدی کجا قرار دارد باید بنویسید:
/*	ball.simulateByDynamics(worldModel->getBody());
 	Point nextBallPos = worldModel->getBall().getPos();
*/
	void simulateByDynamics(const Body &body, unsigned checkCollisionLevel = 0, BallStatus ballStatus = BS_NONE,
			unsigned curTime = 0xFFFF);

// این تابع توپ را با یک Command شبیه سازی می کند. مثلا اگر می خواهید بدانید توپ بر اثر یک شوت در لحظه ی بعدی کجاست باید کد زیر را بنویسید:
/*	Command* kick = new KickCommand(100, 10);
	ball.simulateByAction(worldModel->getBody(),kick);
	ball.simulateByDynamics(worldModel->getBody());
	Point nextBallPos = worldModel->getBall().getPos();
*/
	void simulateByAction(const Body &body, const Command *bodyCycleCommand, bool ballCatched = false);

	void setServerParamVars(const Param &serverParam);
	void parseFullState(const SExpression &exp, unsigned curTime);

// این تابع مشخص می کند که اطلاعات مربوط به Ball معتبر هستند یا نه. مثلا اگر آخرین اطلاعات از توپ را بیش از ۱۵ سایکل قبل گرفته باشیم این تابع false بر می گرداند که بیان گر این است که اطلاعات مربوط به توپ در این لحظه معتبر نیست.
	bool isValid() const;

	// Getting functions
// مشخص می کند که توپ با شیء دیگری برخورد داشته است یا نه.
	bool isCollision() const;
	bool isAbsoluteUpdate() const;
	bool isUpdateByHearPermitted() const;
	bool isUpdatedByHear() const;

// خروجی این تابع مشخص می کند که اطلاعات مربوط به توپ برای چند سایکل قبل است.
	unsigned getSimCounter() const;

	float getKickPowerRate() const;
	float getKickRandFactorL() const;
	float getKickRandFactorR() const;
	float getStoppedBallVel() const;

	float getTacklePowerRate() const;

	unsigned getLatestSeeTime() const;
	Vector getSeenPos() const;
	Vector getSeenVel() const;
	unsigned getSeenCounter() const;

	// Setting functions
	void setCollision(bool collisionFlagArg);
	void setAbsoluteUpdate(bool absoluteUpdateFlagArg);
	void setUpdateByHearPermitted(bool updateByHearPermittedFlagArg);
	void setSimCounter(unsigned simCounterArg);
};

#endif // __BALL_H
