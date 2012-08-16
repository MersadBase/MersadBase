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
 *  This file is created by: Meisam Vosoughpour, Mohammad Salehe
 *  	and is modified by: Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __BASIC_DASH_H
#define __BASIC_DASH_H

#include <BasicAction.h>
#include <WorldModel.h>
#include <Types.h>
#include <Config.h>

#include <Rectangle.h>

// در این فایل کلاس های مختلفی برای رفتن و حرکت کردن به نقاط زمین وجود دارد.

#ifndef NULL
#	define NULL 0
#endif // NULL

// کلاس DashNormalGotoPoint یکی از ساده ترین کلاس ها برای رفتن به هر نقطه است. همان طور که می بینید این کلاس به دو شکل ساخته می شود که توضیحات مربوط به هر کدام از آن‌ها آمده است.
class DashNormalGotoPoint: public BasicAction
{
protected:
	Command *command;
	CommandType mode;
public:
// در این‌جا ورودی اول نقطه ای است که می خواهید به آنجا بروید. ورودی دوم فاصله ای است که مشخص می کند شما تا چه فاصله ای از نقطه ی مورد نظر می خواهید برسید. از آنجاییکه همواره خطا در محاسبات وجود دارد نمی توان گفت در فاصله ی صفر از نقطه ی مورد نظر می خواهیم برویم. در اغلب زمان ها فاصله ی ۰.۷۵ پیشنهاد می شود. ورودی آخر تابع هم که مقدار پیش فرض دارد مشخص می کند که در طول این حرکت با حداکثر چه قدرتی برود.
// در نمونه ی کد زیر نقطه ی مورد نظر با متغیر target مشخص شده است و وسط زمین را مشخص کرده.
// Point target;
// target.x = 0;
// target.y = 0;
// DashNormalGotoPoint d(target, 0.75, worldModel->getBody());
// حال باید command خود را برابر با command نمونه ی ساخته شده از کلاس DashNormalGotoPoint قرار دهیم.
// command = d.getCommand();
// کدهای بالا را می توان به شکل زیر نیز نوشت:
// command = DashNormalGotoPoint(Point(0, 0), 0.75, worldModel->getBody()).getCommand();
	DashNormalGotoPoint(const Point target,
			float distance, const Body &body, float maxDashPower = 100, float dir_thr = 6.f, bool TOF = false);

// در این‌جا تمام ورودی ها مانند ورودی های تابع قبلی است و یک متغیر دیگر هم به آن اضافه شده که idealBodyDir نام دارد. این متغیر مشخص می کند که در طول حرکت به نقطه ی مورد نظر زاویه ی بدن با حداکثر چه اختلافی در راستای حرکت باشد.
	DashNormalGotoPoint(const Point target,
			float distance, const Body &body, float idealBodyDir,
			float maxDashPower);

	CommandType getMode();
	virtual Command *getCommand();
};

class GoalieGotoPoint: public BasicAction
{
protected:
	Command* command;
	CommandType mode;
public:
	GoalieGotoPoint(const Point target, const Body& body, float idealBodyDir = 90);

	virtual Command* getCommand();
	CommandType getMode();
};

class DangerAreaGotoPoint: public BasicAction
{
protected:
	Command* command;
	CommandType mode;
public:
	DangerAreaGotoPoint(const Point target,	const Body& body,
						const Ball &ball, Point ballPos, float dashPower = 100);

	virtual Command* getCommand();
	CommandType getMode();
};

class PerfectGotoPoint: public BasicAction
{
protected:
	Command* command;
	CommandType mode;
public:
	PerfectGotoPoint(const Point target,
			const Body& body, double dist_thr = 1.0,
			double dashPower = 100, double dir_thr = 10,
			bool useBackDash = true, bool forceBackDash = false);

	virtual Command* getCommand();
	CommandType getMode();
};

class FreeKickGotoPoint: public BasicAction
{
protected:
	Command *command;
	int status;

public:
	FreeKickGotoPoint(const Point point,
			float standDirection, float collisionDistance, const Body &body);

	// Return Value = 0:arrived; 1:not arrived
	int getStatus() const;

	virtual Command *getCommand();
};

class ExactGotoPoint: public BasicAction
{
protected:
	Command *command;
	CommandType mode;

public:
	ExactGotoPoint(const Point targetPoint,
			float deltaDistance, const Body &body, bool negDashFlag = false);

	void setDashPower(const Body &body, const Point targetPoint,
			float defaultDashPower, float &dashPower);

	CommandType getMode();

	virtual Command *getCommand();
};

class ForbiddenAreaGotoPoint: public BasicAction
{
protected:
	Command *command;
	int status;
	bool forbiddenSourceFlag;
	bool forbiddenTargetFlag;
	int realTargetPlace;
	Point realTargetPoint;
	bool clockwiseFlag;
	bool goAroundFlag;

public:
	ForbiddenAreaGotoPoint(const Point point,
			float rectX1, float rectY1, float rectY2, const Body &body);
	ForbiddenAreaGotoPoint(const Point point,
			const Point rectNW, const Point rectSE, const Body &body);
	ForbiddenAreaGotoPoint(const Point point,
			const Point circleCenter, float circleRadius,
			const Body &body, bool forbiddenX = false);

	virtual Command *getCommand();
	bool isForbiddenSource() const;
	bool isForbiddenTarget() const;
	int getRealTargetPlace() const;
	Point getRealTargetPoint() const;
	bool isClockwise() const;
	bool isGoAround() const;

	// Return Value = 0:arrived; 1:not arrived
	int getStatus() const;

};

#endif // __BASIC_DASH_H
