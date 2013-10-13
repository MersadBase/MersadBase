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

#ifndef __BODY_H
#define __BODY_H

#include <Player.h>
#include <Types.h>

#ifndef NULL
#	define NULL 0
#endif // NULL
#define UNUM_FAR_LENGTH 20.00
#define UNUM_TOO_FAR_LENGTH 40.00
#define TEAM_FAR_LENGTH 40.00
#define TEAM_TOO_FAR_LENGTH 60.00

class Command;
class SExpression;

// در این فایل کلاس ‌Body قرار دارد. این کلاس از Player ارث برده است. فرق یک Player با Body این است که Body کلاسی است که اطلاعات بیشتری را نسبت Player‌ شامل می شود و این کلاس کلاسی است که اطلاعات بازیکن اجرا شده را نگه داری می کند در حالی که کلاس Player اطلاعات بازیکنان دیگر را نگه داری می کند.

class Body: public Player
{
protected:
	bool seeSynch;
	float viewAngle;
	//TriAnswer collisionStatus;
	bool collisionBall;
	bool collisionPlayer;
	bool collisionPost;

	// SenseBody items except stamina from Player
	float battery;
	float effort;
	float speedAmount;
	float speedDir;
	float armTargetDist;
	float armTargetDir;
	unsigned kickCount;
	unsigned dashCount;
	unsigned turnCount;
	unsigned sayCount;
	unsigned turnNeckCount;
	unsigned catchCount;
	unsigned moveCount;
	unsigned senseBodyCycle;
	unsigned changeViewCount;
	unsigned armMovable;
	unsigned armExpires;
	unsigned armCount;
	unsigned tackleExpires;
	unsigned tackleCount;
	unsigned focusTargetUniNum;
	unsigned focusCount;
	TeamId focusTargetTeamId;
	ViewModeWidth vmWidth;
	ViewModeQuality vmQuality;

	// Last counts
	unsigned lastKickCount;
	unsigned lastDashCount;
	unsigned lastTurnCount;
	unsigned lastSayCount;
	unsigned lastTurnNeckCount;
	unsigned lastCatchCount;
	unsigned lastMoveCount;
	unsigned lastChangeViewCount;
	unsigned lastArmExpires;
	unsigned lastArmCount;
	unsigned lastFocusCount;
	unsigned lastTackleExpires;
	unsigned lastTackleCount;

	// Local serverparam variables
	float visibleDistance;

public:
	Body();

	void setSynch(bool = true);
	bool isSynch() const;

	bool isCollision() const;
	bool isCollisionBall() const;
	bool isCollisionPlayer() const;
	bool isCollisionPost() const;

	float getDashDirRate(float dir) const;
	float getTurnInertia() const;

	void simulateByDynamics(bool realBody = false);
	void simulateByAction(const Command *bodyCycleCommand, bool realBody = false);

	void completeParseSenseBody();
	void parseSenseBody(const SExpression &exp, Side OurFieldSide);
	void update(FieldFlag **flags, FieldGoal **goals, FieldLine **lines, unsigned flagsNum,
			unsigned goalsNum, unsigned linesNum);

	virtual void setServerParamVars(const Param &serverParam);

	float viewWidthToViewAngle(ViewModeWidth width) const;
	ViewModeWidth viewAngleToViewWidth(float angle) const;

	// Counting functions
	bool isKickCountChanged() const;
	bool isDashCountChanged() const;
	bool isTurnCountChanged() const;
	bool isSayCountChanged() const;
	bool isTurnNeckCountChanged() const;
	bool isCatchCountChanged() const;
	bool isMoveCountChanged() const;
	bool isChangeViewCountChanged() const;
	bool isArmExpiresChanged() const;
	bool isArmCountChanged() const;
	bool isFocusCountChanged() const;
	bool isTackleExpiresChanged() const;
	bool isTackleCountChanged() const;
	bool isBallInKickable(const Ball& ball)const;

	// Getting functions
	float getViewAngle() const;
	//TriAnswer getCollisionStatus() const;

	float getEffort() const;
	float getSpeedAmount() const;
	float getSpeedDir() const;
	float getArmTargetDist() const;
	float getArmTargetDir() const;
	unsigned getKickCount() const;
	unsigned getDashCount() const;
	unsigned getTurnCount() const;
	unsigned getSayCount() const;
	unsigned getTurnNeckCount() const;
	unsigned getCatchCount() const;
	unsigned getMoveCount() const;
	unsigned getSenseBodyCycle() const;
	unsigned getChangeViewCount() const;
	unsigned getArmMovable() const;
	unsigned getArmExpires() const;
	unsigned getArmCount() const;
	unsigned getTackleExpires() const;
	unsigned getTackleCount() const;
	unsigned getFocusTargetUniNum() const;
	unsigned getFocusCount() const;
	TeamId getFocusTargetTeamId() const;
	float getBattery() const;

	float getBallAccelMax(const Ball &ball) const;

// این تابع حالت درجه دید را بر می گرداند. همان طور که می دانید سه حالت برای دید وجود دارد که تفاوت آن ها در میزان درجه ی آن هاست.
	ViewModeWidth getVMWidth() const;

	ViewModeQuality getVMQuality() const;

	float getVisibleDistance() const;

	bool canLookToDir(float dir, ViewModeWidth vmw) const;

	// Setting functions
	void setViewAngle(float viewAngleArg);
	//void setCollisionStatus(TriAnswer collisionStatusArg);

	void setVMWidth(ViewModeWidth vmWidthArg);
	void setVMQuality(ViewModeQuality vmQualityArg);

	void setEffort(float effortArg);

};

#endif // __BODY_H
