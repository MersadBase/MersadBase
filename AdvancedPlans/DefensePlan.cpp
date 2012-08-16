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
 *  This file is created by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <DefensePlan.h>

#include <Offense.h>
#include <Tackle.h>
#include <Middle.h>
#include <Command.h>
#include <Logger.h>
#include <Shoot.h>
#include <Tackle.h>
#include <Dribble.h>
#include <Pass.h>
#include <BasicTurn.h>
#include <BasicKick.h>
#include <BasicDash.h>
#include <Intercept.h>

using namespace std;

// در این فایل کدهای مربوط به بخش دفاع قرار دارند.

DefensePlan::DefensePlan(const WorldModel *worldModel): worldModel(worldModel)
{
}

DefensePlan::~DefensePlan()
{
}

void DefensePlan::decide(Form &form)
{
	Tackle tackle(worldModel);

	if (worldModel->isBallKickable())
	{
		LOG << "\t tup tu kickable e " << endl;
		doKick(form);
	}
	else if (worldModel->getGlobalFastIC().isSelfFastestTeammate())
	{
		if(tackle.execute(form))
		{
			command = tackle.getCommand();
		}
		else
		{
// حال که ما سریع ترین بازیکنی هستیم که می توانیم توپ را بگیریم باید توپ را بگیریم. همان طور که می دانید سریع ترین حالتی که توپ را بگیریم این نیست که مستقیم به سمت توپ حرکت کنیم. سریع ترین حالت این است که توپ را در مسیرش قطع کنیم. برای این کار می توانیم از کلاس Intercept استفاده کنیم. توضیحات مربوط به هر کدام از تابع های زیر در فایل Intercept.h آمده است.
			Intercept intercept = Intercept(worldModel);
			intercept.getValue();
			intercept.execute(form);
// در خط زیر command دفاع را برابر با command نمونه ی ساخته شده از کلاس Intercept قرار می دهیم.
			command = intercept.getCommand();
		}
	}
	else
	{
		doPositioning();
	}
}

void DefensePlan::doPositioning()
{
// حال که نمی خواهیم به دنبال توپ برویم باید جایگیری کنیم. برای پیدا کردن جای خود می توانیم از تابع زیر که از فایل PlayOnPositioning.conf اطلاعات مربوط به جایگیری را می خواند, استفاده کنیم.
	Point targetPoint = worldModel->getFormation().getPosition(worldModel->getBody().getUniNum(), worldModel->getBall().getPos());
// حال که نقطه ی مورد نظر به دست آمده باید به سمت آن نقطه حرکت کنیم. برای این کار از کلاس DashNormalGotoPoint استفاده می کنیم. اطلاعات مربوط به این کلاس در فایل BasicDash.h آمده است.
	command = DashNormalGotoPoint(targetPoint, 0.75, worldModel->getBody()).getCommand();
}

void DefensePlan::doKick(Form& form)
{
	command = new EmptyCommand();
	return;

	Shoot shoot(worldModel);
	Dribble dribble(worldModel);
	Pass pass(worldModel);
	LOG << "\tsakhtan ha tamum shod" << endl;
	if (shoot.execute())
	{
		LOG << "\tShoot value dad " << endl;
		command = shoot.getCommand();
	}
	else
	{
		LOG << "\tsalam" << endl;
		LOG << "\tDribble.getValue() = " << dribble.getValue() << endl;
		LOG << "\tPass.getValue() = " << pass.getValue() << endl;
		if(dribble.getValue() >= pass.getValue())
		{
			LOG << "\tdribble mikonam " << endl;
			dribble.decide(form);
			command = dribble.getCommand();
		}
		else
		{
			LOG << "\t Pass midam " << endl;
			pass.decide(form);
			command = pass.getCommand();
		}
	}
}

Command* DefensePlan::getCommand()
{
	return command;
}

