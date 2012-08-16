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

#ifndef __WORLD_MODEL_BASE_H
#define __WORLD_MODEL_BASE_H

// در این فایل کلاس WorldModelBase قرار دارد که اطلاعات مربوط به زمین و بازیکن ها و توپ و سایر اطلاعات مربوط به بازی را در خود نگه داری می کند.

#include <Ball.h>
#include <Body.h>
#include <Timer.h>
#include <Config.h>
#include <Object.h>
#include <Player.h>
#include <FieldData.h>

#define FLAGS_NUM 53
#define GOALS_NUM 2
#define LINES_NUM 4
#define FULL_PLAYERS_NUM 11
#define HALF_PLAYERS_NUM 20
#define QUARTER_PLAYERS_NUM 30
#define CHANGE_PLACE_SURE_ANGLE 5
#define CHANGE_PLACE_SURE_DIST 2
#define PUT_OUT_VIS_SURE_DIST 0.2

#define VIEW_PARTS_NUM 72
#define VIEW_PART_SIZE 5
#define EXTRA_VIEW_SIZE ((VIEW_PARTS_NUM * VIEW_PART_SIZE - 360) \
		/ (float)VIEW_PARTS_NUM)
#define MAX_SEE_DELTA_CYCLE 50

class WorldModelBase
{
protected:
	bool seeSynch;

	Timer *timer;
	FieldData fieldData;

	Param serverParam;
	Param playerParam;
	std::vector<Param> playerTypes;

	bool bodyReplacedFlag;
	unsigned seeCycle;
	unsigned seeTime;
	unsigned curCycle;
	unsigned curTime;
	unsigned refereeHearTime;
	unsigned playModeRemainCycle;
	unsigned playOnStartCycle;
	Side fieldSide;
	PlayMode playMode;
	VirtualPlayMode virtualPlayMode;

	float viewPartDirs[VIEW_PARTS_NUM];
	unsigned viewPartCycles[VIEW_PARTS_NUM];

	// Objects on the field
	FieldFlag *flags[FLAGS_NUM];
	FieldGoal *goals[GOALS_NUM];
	FieldLine *lines[LINES_NUM];

	Ball *ball;
	Body *body;
	Player *fullPlayers[2][FULL_PLAYERS_NUM];
	Player *halfPlayers[2][HALF_PLAYERS_NUM];
	Player *quarterPlayers[QUARTER_PLAYERS_NUM];

	unsigned flagsNum;
	unsigned goalsNum;
	unsigned linesNum;

	unsigned lastGoalieSeeCounter;

	std::string opponentTeamName;

public:
	WorldModelBase();
	virtual ~WorldModelBase();

	virtual void recopyPointers(const WorldModelBase &wmBase);

	bool isSeeSynch() const;

	void initServerParam(const SExpression &exp);
	virtual void initPlayerParam(const SExpression &exp);
	virtual void initPlayerType(const SExpression &exp);

	virtual void see(const SExpression &exp);
	virtual void parseSee(const SExpression &exp);
	virtual void parseFlag(const SExpression &exp);
	virtual void parseGoal(const SExpression &exp);
	virtual void parseLine(const SExpression &exp);
	virtual void parseBall(const SExpression &exp);
	virtual void parsePlayer(const SExpression &exp);

	void updatePlayModeRemainCycleAfterSenseBody();
	void parseChangePlayerType(const SExpression &exp);
	void parseOk(const SExpression &exp);

	void resetObjects();
	void updateObjects();
	void replaceBodyInPlayersList(unsigned bodyUniNum);
	void simulateObjects(BallStatus ballStatus,
			const Body &lastBody, const Command *bodyCycleCommand);
	virtual void logObjects();

	void completeUpdateObjects();
	void putOutOfVisibleAngle(Object &object);
	void putOutOfVisibleDistance(Object &object);
	void changePosDevBySee(Object &object);

	// View parts functions
	void initViewPartDirs();
	void resetSeeDeltaCycles();
	void updateSeeDeltaCyclesAfterSee();
	void updateSeeDeltaCyclesAfterSenseBody();
	float getSeeDeltaCycle(float dir/*, float minRate = 0.35*/) const;
	float getRangeSeeDeltaCycle(float dir, float viewSize) const;
	float getSeeViewPartValue(unsigned viewPartNum,
			float headDir, float viewAngle) const;

	// Getting functions
	bool isBodyReplaced() const;
	unsigned getFlagsNum() const;
	unsigned getGoalsNum() const;
	unsigned getLinesNum() const;
	unsigned getSeeTime() const;
	unsigned getSeeCycle() const;
	unsigned getCurTime() const;

	float getSafetyDashPower(float dash, float stamina) const;

// سایکل بازی را بر می گرداند.
	unsigned getCurCycle() const;

// جهت تیم ما را مشخص می کند که تیم ما در سمت چپ زمین بازی می کند یا سمت راست(در تمام جاها فرض بر این است که ما سمت چپ بازی می کنیم و اگر سمت راست هم تیم اجرا شود خود بیس اطلاعات را برای ما به صورتی می فرستد که سمت چپ اجرا شده باشیم.) خروجی این تابع از جنس enum Side است. برای اطلاعات بیشتر در مورد Side فایل Types.h را ببینید.
	Side getFieldSide() const;

// دو تابع زیر شرایط بازی را مشخص می کنند که بازی در چه حالتی قرار دارد. خروجی تابع ها از جنس های PlayMode و VirtualPlayMode است که برای دیدن حالت های مختلف بازی و اطلاعات در مورد آن ها می توانید فایل Types.h را ببینید.
	PlayMode getPlayMode() const;
	VirtualPlayMode getVirtualPlayMode() const;

// این تابع مشخص می کند که از زمانی که بازی در حالت PlayOn قرار گرفته چند سایکل گذشته است.
	unsigned getPlayOnPastCycles() const;

	unsigned getRefereeHearTime() const;

	float getViewPartDir(unsigned num) const;
	unsigned getViewPartCycle(unsigned num) const;

	const Timer &getTimer() const;

// این تابع مشخص می کند که چند سایکل تا پایان حالت فعلی بازی باقی مانده.
	unsigned getPlayModeRemainCycle() const;

// اطلاعات و پارامتر های مربوط به سرور و بازیکنان و انواع تایپ های بازیکنان که خروجی آن از جنس Param است. برای دیدن مثال هایی از سه تابع زیر می توانید فایل های Tackle.cpp و FastIC.cpp را ببینید.
	const Param &getServerParam() const;
	const Param &getPlayerParam() const;
	const Param &getPlayerType(unsigned num) const;

// این تابع همه ی اطلاعات مربوط به توپ را بر می گرداند. جنس این تابع از نوع Ball است. برای اطلاعات بیشتر در مورد Ball فایل ‌‌Ball.h را ببینید.
	const Ball &getBall() const;

// این تابع همه ی اطلاعات مربوط به بازیکنی را که برنامه را برای آن شماره اجرا کردیم بر می گرداند.
	const Body &getBody() const;

// این تابع بازیکنانی رو که در قسمت Full ویژن قرار دارند را به ما بر می گرداند. این تابع دو ورودی می گیرد که اولی مشخص می کند که بازیکنی که اطلاعات در مورد آن می خواهیم از چه تیمی است(تیم خودمان یا تیم حریف) و ورودی بعدی هم شماره ی بازیکنی است که اطلاعات مربوط به آن را می خواهیم. با صدا زدن تابع isValid که در کلاس Player قرار دارد می توانیم بفهمیم که اطلاعات مربوط به بازیکن مورد نظر ما معتبر هست یا نه. وقتی که یک بازیکن را برای مدتی نبینیم اطلاعات مربوط به آن دیگر معتبر نخواهد بود.
// یک نمونه ی کد که در آن شماره بازیکن هایی را که اطلاعات ما در مورد آن ها درست است را می نویسیم:
/* for (unsigned int i = 0; i < 11; i++)
		if (getFullPlayer(TID_TEAMMATE(/// مشخص می کنیم که بازیکن مورد نظر ما هم تیمی است ////, i).isValid())
			LOG << "Number: " << i + 1 << endl;
*/
// همان طور که در مثال دیدید شماره ی بازیکن ها برای این تابع از ۰ تا ۱۰ است. پس اگر ما اطلاعات مربوط با بازیکن پنجم را بخواهیم باید عدد ۴ را وارد کنیم.
	const Player &getFullPlayer(TeamId teamId, unsigned num) const;
// همانند بالا با این تفاوت که برای بازیکنانی است که در قسمت Half ویژن قرار دارند.
	const Player &getHalfPlayer(TeamId teamId, unsigned num) const;
//‌ همانند بالا با این تفاوت که برای بازیکنانی است که در قسمت Quarter ویژن قرار دارند.
	const Player &getQuarterPlayer(unsigned num) const;
	unsigned getLastGoalieSeeCounter() const;

	// Setting functions
	void setBodyReplaced(bool bodyReplacedFlagArg);
	void setFlagsNum(unsigned flagsNumArg);
	void setGoalsNum(unsigned goalsNumArg);
	void setLinesNum(unsigned linesNumArg);
	void setSeeCycle(unsigned seeCycleArg);
	void setCurCycle(unsigned curCycleArg);
	void setCurTime(unsigned curTimeArg);
	void setFieldSide(Side fieldSideArg);
	void setVirtualPlayMode(VirtualPlayMode virtualPlayModeArg);

	Timer &setTimer();
	Ball &setBall();
	Body &setBody();
	Player &setFullPlayer(TeamId teamId, unsigned num);
	Player &setHalfPlayer(TeamId teamId, unsigned num);
	Player &setQuarterPlayer(unsigned num);
	FieldData &setFieldData();

	const std::string getOpponentTeamName() const;
};

#endif // __WORLD_MODEL_BASE_H

