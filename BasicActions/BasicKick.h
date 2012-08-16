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
 *  This file is created by: Mostafa Rokooey
 *  	and is modified by: Pooria Kaviani
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __BASIC_KICK_H
#define __BASIC_KICK_H

#include <Types.h>
#include <Vector.h>
#include <BasicAction.h>

#include <HalfLine.h>

// در این فایل کلاس های مختلفی برای شوت زدن به نقاط مختلف و جهت های گوناگون وجود دارد.

#ifndef NULL
#	define NULL 0
#endif // NULL

class Object;
class Ball;
class Player;
class Body;

// همه ی کلاس های مربوط به شوت زدن از کلاس BasicKick ارث می برند که توابع مشترک بین آن ها در این کلاس مشخص شده است.
class BasicKick: public BasicAction
{
protected:
	Vector kickVector;
	Vector trajectoryVector;
	bool possibleFlag;

public:
	BasicKick();

	Vector getKickVec();
	Vector getTrajVec();

// مهم ترین تابعی که در شوت های مختلف با آن سر و کار داریم این تابع است که مشخص می کند آیا شوتی که ما انتظار داریم امکان پذیر هست یا نه. به عنوان مثال ممکن است ما انتظار داشته باشیم توپ را با سرعت ۱۰ به جهتی شوت کند که این امکان پذیر نیست. ما باید پس از ساختن هر نمونه از کلاس های Kick این تابع را چک کنیم که آیا شوت مورد نظر ممکن هست یا نه.
	bool isPossible();
	float needKick(float power, const Body &body, const Ball &ball);
	float directionOfMaxPowerKick(float direction, const Body &body,
		const Ball &ball, float maxKickPower = 100);

	static float getMaxDirectionVelocity(float direction, const Player &body, const Ball &ball);
};

// این کلاسی است برای شوت زدن یک توپ با سرعت دلخواه و در جهت دلخواه استفاده می شود.
class KickToDirectionVelocity: public BasicKick
{
protected:
	float direction;
	float velocity;
	const Ball &ball;
	const Body &body;
	float possibleArea;
	Command *kickCommand;

public:
// ورودی اول جهتی است که می خواهیم در آن راستا شوت شود. این زاویه نسبت به جهت بدن نیست و زاویه ای مطلق است. ورودی دوم سرعت شوت را مشخص می کند. ورودی های بعدی هم ball و ‌body هستند که باید مشخص شود چه Body ای می خواهد چه ‌توپی را شوت بزند. یک نمونه ی کد در زیر آمده است. در این مثال می خواهیم توپ را با سرعت ۱ و در جهت ۶۰ درجه شوت کنیم.
/*	Command *command;
	KickToDirectionVelocity k(60, 1, worldModel->getBall(), worldModel->getBody());
	if (k.isPossible()) // در اینجا بررسی می شود که آیا می توان توپ را با سرعت ۱ در جهت ۶۰ درجه شوت کرد یا نه
	{
// اگر چنین ضربه ای ممکن بود command خود را برابر command نمونه ی ساخته شده از کلاس قرار می دهیم.
		command = k.getCommand();
	}
	else
	{
چنین ضربه ای ممکن نیست و باید کار دیگری بکنیم...
		command = new EmptyComman();
	}
*/
	KickToDirectionVelocity(float direction, float velocity,
		const Ball &ball, const Body &body, float possibleArea = 0);
	virtual Command *getCommand();

};

// کلاس زیر همانند کلاس KickToDirectionVelocity هست با این تفاوت که جای اینکه توپ را به یک جهت شوت کند توپ را به سمت یک نقطه شوت می کند. از آنجایی‌که توضیحات این کلاس نیز همانند توضیحات کلاس قبلی است صرفا یک نمونه ی کد می آوریم. در این نمونه ی کد می خواهیم توپ را به سمت مرکز دروازه ی حریف با سرعت ۱ شوت کنیم.
/*
	Command *command;
	KickToPointVelocity k(Point(52.5, 0), 1,worldModel->getBall(),worldModel->getBody());
	if (k.isPossible())
		command = k.getCommand();
	else
		command = new EmptyCommand();
*/
class KickToPointVelocity: public BasicKick
{
protected:
	Point point;
	float velocity;
	const Ball &ball;
	const Body &body;
	float possibleArea;
	Command *kickCommand;

public:
	KickToPointVelocity(const Point point, float velocity, const Ball &ball,
		const Body &body, float possibleArea = 0);
	virtual Command *getCommand();
};

// این کلاس همانند کلاس KickToPointVelocity است با این تفاوت که ورودی ای که به عنوان سرعت می گیرد سرعتی نیست که می خواهیم توپ در این لحظه داشته باشد بلکه سرعتی است که می خواهیم توپ وقتی به نقطه ی مورد نظر رسید داشته باشد.
class KickToPointArriveVelocity: public BasicKick
{
protected:
	Point point;
	float velocity;
	const Ball &ball;
	const Body &body;
	float possibleArea;
	Command *kickCommand;

public:
	KickToPointArriveVelocity(const Point point, float velocity, const Ball &ball,
		const Body &body, float possibleArea = 0);
	virtual Command *getCommand();
};

// این کلاس جهت شوت زدن به یک نقطه استفاده می شود که در آن می توان مشخص کرد توپ پس از چند Cycle به نقطه ی مورد نظر برسد.
class KickToPointCycles: public BasicKick
{
protected:
	Point point;
	float cycles;
	const Ball &ball;
	const Body &body;
	float possibleArea;
	Command *kickCommand;

public:
// ورودی اول نقطه ی هدف شوت است. ورودی دوم تعداد سایکل هایی است که می خواهیم توپ پس از آن تعداد به نقطه ی مورد نظر برسد. ورودی های بعدی هم ball و ‌body هستند که باید مشخص شود چه Body ای می خواهد چه ‌توپی را شوت بزند. در زیر مثالی از کار کردن با این کلاس آمده است که می خواهد توپ پس از پنج Cycle به مرکز زمین برسد
/*
	Command *command;
	KickToPointCycles k(Point(52.5, 0), 5, worldModel->getBall(), worldModel->getBody());
	if (k.isPossible())
		command = k.getCommand();
	else
		command = new EmptyCommand();
*/
	KickToPointCycles(const Point point, float cycles, const Ball &ball, const Body &body,
		float possibleArea = 0);
	virtual Command *getCommand();
};

// این کلاس جهت شوت زدن توپ به یک جهت با بیشترین قدرت استفاده می شود.
class FastKickToDir: public BasicKick
{
protected:
	float dir;
	const Ball &ball;
	const Body &body;
	Command *kickCommand;

public:
// ورودی اول جهت مورد نظر برای شوت و ورودی های بعدی هم ball و ‌body هستند که باید مشخص شود چه Body ای می خواهد چه ‌توپی را شوت بزند. یک نمونه از استفاده از این کد در زیر آمده است. در این مثال می خواهیم توپ را در جهت ۳۰ درجه با بیشترین قدرت شوت کنیم.
/*	Command *command;
	FastKickToDir k(30, worldModel->getBall(), worldModel->getBody());
	if (k.isPossible())
		command = k.getCommand();
	else
		command = new EmptyCommand();
*/

	FastKickToDir(float dir, const Ball &ball, const Body &body);
	virtual Command *getCommand();
};

// این کلاس همانند کلاس FastKickToDir است با این تفاوت که به جای اینکه مشخص کنیم در چه جهتی شوت بزند می گوییم به سمت چه نقطه ای شوت بزند. و به جای ورودی جهت, یک نقطه می گیرد.
class FastKickToPoint: public BasicKick
{
protected:
	Point point;
	const Ball &ball;
	const Body &body;
	Command *kickCommand;

public:
	FastKickToPoint(const Point point, const Ball &ball, const Body &body);
	virtual Command *getCommand();
};

/*class MultiStepKickToDir: public BasicKick
{
protected:
	float dir;
	float velocity;
	unsigned steps;
	const Ball &ball;
	const Body &body;
	Command *kickCommand;
public:
	MultiStepKickToDir(float dir, float velocity, unsigned steps, const Ball &ball, const Body &body);
	virtual Command *getCommand();
};*/

#endif // __BASIC_DASH_H
