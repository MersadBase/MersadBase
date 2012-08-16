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
 *  This file is created by: Pooria Kaviani
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __INTERCEPT_H
#define __INTERCEPT_H

#include <DecideCommonTypes.h>
#include <Ball.h>
#include <Body.h>

class Form;
class WorldModel;
class Command;

// همان طور که می دانید سریع ترین حالتی که توپ را بگیریم این نیست که مستقیم به سمت توپ حرکت کنیم. سریع ترین حالت این است که توپ را در مسیرش قطع کنیم. برای این کار می توانیم از کلاس Intercept استفاده کنیم. برای استفاده از این کلاس باید یک نمونه از آن کلاس را بسازیم. سپس تابع getValue را صدا بزنیم. این تابع محاسبات مربوط به Intercept را انجام می دهد. در ادامه تابع execute را صدا بزنیم که این تابع Intercept را اجرا می کند. فرق این تابع با تابع getValue این است که این تابع تنظیمات مربوط به سر را انجام می دهد و در getValue این محاسبات را انجام نمی دهد. ممکن است ما زمانی فقط بخواهیم از محاسبات Intercept ‌استفاده کنیم و خود Intercept اجرا نشود که در آن زمان فقط تابع getValue را صدا می کنیم. در زیر نمونه ای از کدی که در آن از Intercept استفاده شده آمده است.
/*
	Intercept intercept(worldModel);
	intercept.getValue();
	intercept.execute(form);
	command = intercept.getCommand();
*/

class Intercept
{
protected:
	const WorldModel *wm;
	Command *command;
	Ball virBall;
	Ball firstBall;
	Body firstBody;
	float getKickableDist();
	bool stopICStart;
	unsigned interceptTime;
	unsigned maxWithTurn;
	unsigned maxMinus;
	unsigned maxPlus;
	Point interceptPoint;

public:
	Intercept(const WorldModel *wm);
	bool goalieMode;
	float goalieKickable;
	unsigned goalieMinusTime;
	void execute(Form &form);
	float getValue();

// دو تابع زیر مشخص می کنند که محاسبات Intercept برای چه توپ چه Body ای انجام شود.
	void setBall(const Ball *ball);
	void setBody(const Body *body);

	bool defenseBlock;
	float defenseKickable;

	bool SRP, shoot;
	void setMaxPlus(unsigned a);
	void setMaxMinus(unsigned a);
	void setMaxWithTurn(unsigned a);
	unsigned getMaxPlus();
	unsigned getMaxMinus();
	unsigned getMaxWithTurn();
	Point getInterceptPoint();
	unsigned getInterceptTime();

// تابع زیر command به دست آمده پس از محاسبات Intercept را بر می گرداند.
	Command *getCommand();
};

#endif // __INTERCEPT_H
