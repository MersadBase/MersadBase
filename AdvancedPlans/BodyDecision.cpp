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
 *  	and is modified by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

// برای توضیحات در مورد کار هر یک از تابع ها فایل BodeDecision.h را ببینید.

#include <BodyDecision.h>
#include <NonPlayOnDecision.h>
#include <Goalie.h>
#include <DefensePlan.h>
#include <Middle.h>
#include <Offense.h>
#include <Basics.h>
#include <Logger.h>

using namespace std;
using namespace Basics;

BodyDecision::BodyDecision()
{
}

BodyDecision::~BodyDecision()
{
}

void BodyDecision::decide(const WorldModel* worldModel, Form &form)
{
// در خط زیر در اصطلاح عبارت Decision Section را لاگ داده ایم که به این معنا است که برنامه ی ما به این خط رسیده است. سیستم لاگ دادن برای باگ گیری کد ها ساخته شده است. به این وسیله می توانیم با سرعت بیشتری کد های خود را عیب یابی کنیم. این سیستم بدین ترتیب کار می کند که برای هر بازیکن در شروع بازی فایلی متناسب با شماره ی آن بازیکن ساخته می شود. سپس در طول بازی اطلاعات مربوط به هر سایکل و چیزهایی را که می خواهیم در‌ آن فایل نوشته شود را با کمک دستور LOG که همانند cout عمل می کند, می نویسیم.
	LOG << "Decision Section" << endl;

	// request for substitute!
	if (worldModel->getBody().getBattery() / worldModel->getBody().getStaminaCapacity() < worldModel->getCurCycle() / 3000 * (3 / 4) and worldModel->getPlayMode() != PM_PLAY_ON)
	{
		LOG << "i wanna going home!!!" << endl;
		// TODO : say request to coach
	}
	// request for substitute!

// در اینجا بررسی می شود که آیا اطلاعات مربوط به توپ کافی هست یا نه. اگر اطلاعات کافی نبود باید دنبال توپ بگردیم تا آن را پیدا کنیم.
	if (worldModel->getBall().isValid())
	{
// در اینجا می بینیم که بازی در حال جریان هست یا نه. مثلا زمانی که توپ اوت برود یا آفساید شود مقدار متغیر getPlayMode عوض می شود و دیگر PM_PLAY_ON نیست و
// مربوط به کارهای NonPlayOn می شود.

		if (worldModel->getPlayMode() == PM_PLAY_ON)
		{
// از worldModel, عضو Body را می گیریم که در آن اطلاعات مربوط بازیکنی که برنامه برای آن در حال اجرا است قرار دارد.(برای اطلاعات بیشتر در مورد ‌Body فایل Body.h
// را ببینید.)‌ سپس شماره ی خودمان را از نمونه ی Body می گیریم. getUniNum مخفف getUniformNumber می باشد.
			if (worldModel->getBody().getUniNum() == 1)
			{
// در این چند خط می خواهیم کد های مربوط به بخش دروازه بان(Goalie) را اجرا کنیم و command(که همان متغیری است که مشخص می کند چه چیزی برای سرور بفرستیم) را
// مقداردهی کنیم.
// در اینجا یک نمونه از کلاس Goalie که مربوط به کارهای دروازه بان است می سازیم و worldModel را به عنوان ورودی تابع سازنده ی آن کلاس می دهیم تا نمونه ی ساخته 
// از کلاس دروازه بان اطلاعات مربوط به بازی را داشته باشد.
				Goalie goalie = Goalie(worldModel);
// تابع decide را صدا می زنیم که در آن form را به عنوان ورودی داده ایم.
				goalie.decide(form);
// از goalie تابع getCommand را صدا می زنیم که command دروازه بان را بر می گرداند و command خودمان را برابر با آن قرار می دهیم تا به سرور فرستاده شود.
				command = goalie.getCommand();
			}
// در اینجا تابع isInPlayers را صدا می کنیم تا ببینیم شماره ی ما بین اعداد ۲ و ۳ و ۴ و ۵ هست یا نه. اگر بود کد های مربوط به دفاع اجرا می شود.البته این شمایید که میگویید چه کسی وظایف چه پستی را انجام دهد. در اینجا اینگونه است که ۲ و ۳ و ۴ و ۵ دفاع و ۶ و ۷ و ۸ خط میانه و ۹ و ۱۰ و ۱۱ در خط حمله اند.در این شرط در حقیقت ما اینجا به جای 
// اینکه چند شرط را به صورت یا(or) با هم چک کنیم یک شرط نوشتیم که همان کار را انجام می دهد. کد داخل شرط معادل کد زیر است: برای توضیحات بیشتر در مورد تابع isInPlayers فایل Basics.h را ببینید.
// worldModel->getBody().getUniNumm() == 2 or worldModel->getBody().getUniNumm() == 3 or worldModel->getBody().getUniNumm() == 4 or worldModel->getBody().getUniNumm() == 5
			else if (isInPlayers(worldModel->getBody().getUniNum(), "2345"))
			{
				DefensePlan defense = DefensePlan(worldModel);
				defense.decide(form);
				command = defense.getCommand();
			}
// مثل بالا با این تفاوت که برای بازیکنان میانی است.
			else if (isInPlayers(worldModel->getBody().getUniNum(), "678"))
			{
				Middle middle = Middle(worldModel);
				middle.decide(form);
				command = middle.getCommand();
			}
// مثل بالا با این تفاوت که برای بازیکنان حمله است.
			else if (isInPlayers(worldModel->getBody().getUniNum(), "9AB"))
			{
				Offense offense = Offense(worldModel);
				offense.decide(form);
				command = offense.getCommand();
			}
		}
		else
		{
// در اینجا ما یک نمونه از کلاس NonPlayOnDecision می سازیم و تابع تصمیم گیری آن را صدا می زنیم و command را مقداردهی می کنیم(دقیقا همان کارهایی که برای Goalie انجام دادیم.)
			NonPlayOnDecision nonPlayOn = NonPlayOnDecision(worldModel);
			nonPlayOn.decide(form);
			command = nonPlayOn.getCommand();
		}
	}
	else
	{
// در اینجا حالت چرخش گردن را مشخص می کنیم که در اینجا روی حالت چرخش برای جستجو تنظیم شده است.
/* form(همان کلاسی که تنظیمات مربوط به سر و سایر چیز ها در آن قرار داشت) --> headForm(تنظیمات مربوط به سر را می گیریم) --> tnMode(TURN NECK MODE) حالت چرخش گردن را مشخص می کنیم
*/
		form.headForm.tnMode = TNM_SEARCH;
// مشخص می کنیم که می خواهیم سایکل های فرد جهت بدن ۱۰۰ درجه بچرخد.
		if (worldModel->getCurCycle() % 2 == 1)
			command = new TurnCommand(100);
		else
			command = new EmptyCommand();
	}
}

Command* BodyDecision::getCommand()
{
	return command;
}
