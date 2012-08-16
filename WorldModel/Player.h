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

#ifndef __PLAYER_H
#define __PLAYER_H

#include <Ball.h>
#include <Types.h>
#include <Object.h>
#include <Types.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

#define APVR_RADIUS 7.5
#define PLAYER_VALID_CYCLE 6
#define PLAYER_ALIVE_CYCLE 12

enum PlayerModel
{
	PLM_QUARTER = 0, PLM_HALF = 1, PLM_FULL = 2
};

class Body;
class ConfigGroup;
typedef ConfigGroup Param;

// در این فایل کلاس Player قرار دارد. این کلاس از Object ارث برده است و خواص یک بازیکن را مشخص می کند.

class Player: public Object
{
protected:
	Vector seenPos;
	Vector seenVel;
	unsigned seenCounter;

	bool goalieFlag;
	bool tacklingFlag;
	bool kickingFlag;
	bool foulChargedFlag;
	float stamina;
	float bodyDir;
	float headDir;
	float pointingDir;
	float relativeBodyDir;
	float relativeHeadDir;
	float relativePointingDir;
	unsigned type;
	unsigned uniNum;
	unsigned postNum;
	TeamId teamId;
	PlayerModel model;
	CardModel card;
	unsigned lastStandCycle;
	unsigned tackleRemainCycle;

	unsigned simCounter;
	unsigned lastAPVRTime;
	unsigned lastAPDRTime;
	Vector lastAPVRPosition;
	Vector lastAPDRVelocity;
	int bodyDirCounter;

	unsigned dashExpires;

	// Heterogeneous items
	float staminaIncMax;
	float inertiaMoment;
	float dashPowerRate;
	float kickableMargin;
	float kickRand;
	float extraStamina;
	float effortMax;
	float effortMin;

	// Local serverparams
	float staminaMax;
	float effortDecThr;
	float recoverDecThr;
	float maxNeckAng;
	float minNeckAng;
	float maxNeckMoment;
	float minNeckMoment;
	float catchableAreaL;
	float catchableAreaW;

	float backDashRate;
	float dashAngleStep;
	float maxDashAngle;
	float minDashAngle;
	float maxDashPower;
	float minDashPower;
	float sideDashRate;
	float staminaCapacity;

	bool updatedByHear;

	void executeAPVR(const Body &body);
	void executeAPDR(const Body &body);

public:
	Player(PlayerModel model = PLM_FULL);
	virtual ~Player();

	virtual void kill();
	void virtualUpdate(const Body &body);

// این تابع بازیکن را یک Cycle شبیه سازی می کند. مثلا می خواهید بدانید بازیکن در Cycle بعدی کجا قرار دارد باید بنویسید:
/*	player.simulateByDynamics(body);
 	Point nextPlayerPos = player.getPos();
*/
	void simulateByDynamics(const Body &body);

// این تابع بازیکن را با یک Command شبیه سازی می کند. مثلا اگر می خواهید بدانید بازیکن بر اثر یک حرکت مستقیم در لحظه ی بعدی کجاست باید کد زیر را بنویسید:
/*	Command* dash = new DashCommand(100, 10);
	player.simulateByAction(worldModel->getBody(), dash);
	player.simulateByDynamics(worldModel->getBody());
	Point nextPlayerPos = player.getPos();
*/
	void simulateByAction(const Command *bodyCycleCommand, bool realBody = false);

	void update(const Body &body, PlayMode playMode = PM_PLAY_ON);
	void parse(TeamId teamIdArg, unsigned uniNumArg, bool isGoalie,
			const SExpression &exp, unsigned curTime);
	void parseFullState(TeamId teamIdArg, unsigned uniNumArg,
			bool isGoalieArg, const SExpression &exp, unsigned curTime);
	void updateByHear(float x, float y, float velMag, float velDir,
			const Body &body);
	void updateByRadar(float magnitude, float direction,
			const Body &body, const Ball &ball);

	virtual void setServerParamVars(const Param &serverParam);

// مشخص می کند که این نمونه از Player از نوع Body هست یا نه.
	bool isBody() const;


	bool canBodySee(const Vector &playerPos, float viewAngle, float visibleDist) const;
	void setByAnotherPlayer(const Player &player, const Body &body);

	bool isAlive() const;

// این تابع مشخص می کند که اطلاعات مربوط به Player معتبر هستند یا نه. مثلا اگر آخرین اطلاعات از بازیکن را بیش از ۶ سایکل قبل گرفته باشیم این تابع false بر می گرداند که بیان گر این است که اطلاعات مربوط به بازیکن در این لحظه معتبر نیست.
	bool isValid() const;

	static unsigned uniNumToPostNum(unsigned uniNum);
	static unsigned postNumToUniNum(unsigned postNum);

	// Heterogeneous functions
	void setType(int typeArg, const Param &playerType);
	float getStaminaIncMax() const;
	float getInertiaMoment() const;
	float getDashPowerRate() const;

	float getKickableMargin() const;
	float getKickRand() const;
	float getExtraStamina() const;
	float getEffortMax() const;
	float getEffortMin() const;

	// Getting functions
// این تابع مشخص می کند که بازیکن در نقش دروازه بان هست یا نیست. همان طور که می دانید دروازه بان با شماره اش مشخص نمی شود بلکه باید به عنوان دروازه بان به سرور وصل شود و می تواند شماره ی دروازه بان هر عددی باشد. پس برای اینکه بدانیم یک بازیکن دروازه بان هست یا نه می توانیم از این تابع استفاده کنیم.
	bool isGoalie() const;

// این تابع مشخص می کند که بازیکن در حال تکل زدن هست یا نه.
	bool isTackling() const;

// این تابع مشخص می کند که بازیکن در حال شوت زدن هست یا نه.
	bool isKicking() const;

	bool isFoulCharged() const;

// این تابع مقدار Stamina بازیکن را بر می گرداند. همان طور که می دانید Stamina معیاری برای مشخص کردن میزان خستگی یک بازیکن است.
	float getStamina() const;

// جهت بدن (Body Direction) بازیکن را مشخص می کند.
	float getBodyDir() const;

// جهت سر (Head Direction) بازیکن را مشخص می کند.
	float getHeadDir() const;

// جهتی را که بازیکن به آن نقطه اشاره می کند بر می گرداند.
	float getPointingDir() const;

	float getRelativeBodyDir() const;
	float getRelativeHeadDir() const;
	float getRelativePointingDir() const;

// مشخص می کند که Type یک بازیکن چند است. همان طور که می دانید در سرور شبیه سازی فوتبال Type های مختلفی برای بازیکنان وجود دارد که این Type ها ویژگی های خاص هر بازیکن را مشخص می کند. به عنوان مثال kickable یک Type با Type دیگر متفاوت است. در نسخه ی فعلی سرور (سرور ۱۳) ۱۸ Type مختلف برای بازیکنان مختلف وجود دارد. این تابع مشخص می کند که این بازیکن کدام یک از آن ۱۸ نوع است.
	unsigned getType() const;

// شماره ی بازیکن را بر می گرداند.
	unsigned getUniNum() const;

	unsigned getPostNum() const;
	unsigned getDashExpires() const;

// مشخص می کند که این بازیکن برای کدام تیم است. تیم خودمان یا حریف. تیم با TeamId مشخص می شود که در فایل Types.h تعریف شده است.
	TeamId getTeamId() const;

	PlayerModel getModel() const;

// خروجی این تابع مشخص می کند که اطلاعات مربوط به بازیکن برای چند سایکل قبل است.
	unsigned getSimCounter() const;

	unsigned getBodyDirCounter() const;

	float getBackDashRate() const;
	float getDashAngleStep() const;
	float getMaxDashAngle() const;
	float getMinDashAngle() const;
	float getMaxDashPower() const;
	float getMinDashPower() const;
	float getSideDashRate() const;
	float getStaminaCapacity() const;

	float getStaminaMax() const;
	float getEffortDecThr() const;
	float getRecoverDecThr() const;
	float getMaxNeckAng() const;
	float getMinNeckAng() const;
	float getMaxNeckMoment() const;
	float getMinNeckMoment() const;
	float getCatchableAreaL() const;
	float getCatchableAreaW() const;

	CardModel getCard() const;

	unsigned getLastStandCycle() const;

	unsigned getTackleRemainCycle() const;

	bool isUpdatedByHear() const;

	Vector getSeenPos() const;
	Vector getSeenVel() const;
	unsigned getSeenCounter() const;

	// Setting functions
// با این تابع می شود تنظیم کرد که بازیکن فعلی دروازه بان هست یا نه.
	void setGoalie(bool goalieFlagArg);

// با این تابع می شود تنظیم کرد که بازیکن در حال تکل زدن هست یا نه.
	void setTackling(bool tacklingFlagArg);

// با این تابع می شود تنظیم کرد که بازیکن در حال شوت زدن هست یا نه.
	void setKicking(bool kickingFlagArg);

	void setFoulCharged(bool foulChargedFlagArg);

// با این تابع می توان جهت بدن یک بازیکن را تنظیم کرد.
	void setBodyDir(float bodyDirArg);

// با این تابع می توان جهت سر یک بازیکن را تنظیم کرد.
	void setHeadDir(float headDirArg);

// با این تابع می توان جهت اشاره ی یک بازیکن را تنظیم کرد.
	void setPointingDir(float pointingDirArg);

	void setRelativeBodyDir(float relativeBodyDirArg);
	void setRelativeHeadDir(float relativeHeadDirArg);
	void setRelativePointingDir(float relativePointingDirArg);

// با این تابع می توان مشخص کرد که بازیکن کدام یک از انواع ۱۸ Type است.
	void setType(unsigned typeArg);

// با این تابع می توان شماره ی یک بازیکن را تنظیم کرد.
	void setUniNum(unsigned uniNumArg);

// با این تابع می توان مشخص کرد که بازیکن مربوط به کدام تیم است(تیم خودمان یا تیم حریف).
	void setTeamId(TeamId teamIdArg);
	void setModel(PlayerModel modelArg);

// با این تابع می توان مشخص کرد که اطلاعات مربوط به یک بازیکن مربوط به چند Cycle قبل می باشد.
	void setSimCounter(unsigned simCounterArg);

	void setBodyDirCounter(unsigned bodyDirCounterArg);

	void setCard(CardModel Card_Model);

	void setLastStandCycle(unsigned cycle);

	void setTackleRemainCycle(unsigned cycle);

};

#endif // __PLAYER_H
