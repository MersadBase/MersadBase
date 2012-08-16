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

#ifndef __BASIC_TURN_H
#define __BASIC_TURN_H

#include <BasicAction.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

// در این فایل کلاس های مربوط به چرخاندن جهت بدن وجود دارد که کار با آن‌ها کنترل جهت بدن را آسان می کند.

class Ball;
class Player;
class Body;
class Object;

// همه ی کلاس های مربوط به چرخش بدن از این کلاس ارث برده اند که خواص و توابع مشترک بین آن کلاس ها را می توانید در اینجا ببینید.
class BasicTurn: public BasicAction
{
protected:
	float deltaAngle;

	float necessaryTurnAngle;
	float requestedTargetAngle;
	float performedTurnAngle;
	float performedTurnMoment;

	bool oneTurnPossible;

	Command *turnCommand;

public:
	BasicTurn();

	virtual Command *getCommand() = 0;

	float getDeltaAngle();

	float getNecessaryTurnAngle();
	float getRequestedTargetAngle();
	float getPerformedTurnAngle();
	float getPerformedTurnMoment();

// این تابع مشخص می کند که آیا چرخش مورد نظر ما با یک بار چرخیدن ممکن هست یا نه. به عنوان مثال وقتی بازیکن در یک جهتی سرعت گرفته باشد نمی تواند در یک لحظه بدنش را ۱۸۰ درجه بچرخاند و به چند Cycle زمان نیاز دارد.
	bool isOneTurnPossible();
	static int getMinCyclesToTurnToPoint(const Point &myPoint , Body body, float threshold = 0.01f);

	// static functions for public turn-related calculations
	static float getTurnMoment(float angle, const Player &player);
	static float getTurnAngle(float moment, const Player &player);
	static int actionsToTurnAngle(float angle, const Player &player,
			float maxDeltaAngle = 0);
};

// این کلاس برای چرخاندن بدن به میزان درجه ی مورد نظر استفاده می شود. مثلا می خواهیم زاویه ی بدن ۴۰ درجه بچرخد. توجه کنید منظور این نیست که زاویه ی بدن بشود ۴۰ درجه بلکه می خواهیم بدن هر درجه ای که دارد ۴۰ درجه بچرخد.
class TurnAngle: public BasicTurn
{
protected:

public:
// ورودی اول میزان درجه ی مورد نظر برای چرخیدن هست. ورودی دوم هم مشخص می کند که چه Body ای می خواهد بچرخد. یک نمونه از کدی که در آن می خواهد بدن ۴۰ درجه بچرخد در زیر آمده است:
/*
	Command *command;
	TurnAngle t(40, body);
	command = t.getCommand();
*/
// می توان کد بالا را به شکل زیر هم نوشت:
//	command = TurnAngle(40, body).getCommand();
	TurnAngle(float angle, const Body &body, float maxDeltaAngle = 0);
	virtual Command *getCommand();
};

// این کلاس برای چرخاندن جهت بدن به جهت مورد نظر است. توجه کنید که زاویه ی بدن را به زاویه ی مورد نظر می رساند و نه اینکه بدن را به میزان زاویه ی داده شده بچرخاند.
class TurnToAngle: public BasicTurn
{
protected:

public:
// ورودی اول مشخص می کند که زاویه ی مورد نظر چند است. ورودی دوم هم مشخص می کند که چه Body ای می خواهد بچرخد. در زیر یک نمونه ی استفاده از این کلاس آمده است. در این مثال می خواهیم زاویه ی بدن بشود ۶۰ درجه.
/*
	Command *command;
	TurToAngle t(60, body);
	command = t.getCommand();
*/
// می توان کد بالا را به شکل زیر هم نوشت:
//	command = TurnToAngle(60, body).getCommand();
	TurnToAngle(float angle, const Body &body, float maxDeltaAngle = 0);
	virtual Command *getCommand();
};

// کلاس زیر برای چرخاندن جهت بدن به سمت یک نقطه هست که بسیار ساده و کارآمد هست.
class TurnToPoint: public BasicTurn
{
protected:

public:
// ورودی اول نقطه ی مورد نظر و ورودی دوم هم مشخص می کند که چه Body ای می خواهد به سمت نقطه ی مورد نظر بچرخد. در زیر یک نمونه ی کد آمده است که در آن می خواهیم جهت بدن به سمت وسط زمین تنظیم شود.
/*
	Command *command;
	Point point;
	point.x = 0;
	point.y = 0;
	TurnToPoint t(point, body);
	command = t.getCommand();
*/
// می توان کد بالا را به شکل زیر هم نوشت:
//	command = TurnToPoint(Point(0, 0), body).getCommand();

	TurnToPoint(const Point point, const Body &body, float maxDeltaAngle = 0);
	virtual Command *getCommand();
};

// این کلاس همانند کلاس TurnToPoint است با این تفاوت که به جای اینکه یک نقطه بگیرد و به آن سمت بچرخد‌ یک شیء به عنوان ورودی می گیرد و راستای بدن را به سمت آن شیء قرار می دهد.
class TurnToObject: public BasicTurn
{
protected:

public:
	TurnToObject(const Object &object, const Body &body, float maxDeltaAngle = 0);
	virtual Command *getCommand();
};

#endif // __BASIC_TURN_H
