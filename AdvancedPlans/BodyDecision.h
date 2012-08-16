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

#ifndef __BODY_DECISION_H_
#define __BODY_DECISION_H_

#include <WorldModel.h>
#include <DecideCommonTypes.h>

// در این فایل کلاس BodyDecision قرار دارد که بیس در هر چرخه یک نمونه از این کلاس می سازد و تابع تصمیم گیری آن(decide) را صدا می زند. شما باید در این تابع
// متغیر command را مقدار دهی کنید و زمانی که کار تابع شما به پایان رسید بیس command شما را برای سرور ارسال می کند.

class BodyDecision
{
private:
// متغیر command که تعیین می کند چه چیزی قرار است برای سرور ارسال شود(برای توضیحات بیشتر در مورد Command فایل Command.h‌ را ببینید.)
	Command *command;

public:
	BodyDecision();
	~BodyDecision();

// تابع تصمیم گیری که دو ورودی می گیرد. ورودی اول از جنس WorldModel است که اطلاعات زمین و بازیکنان و توپ و هر جیزی که مربوط به جریان بازی است در آن قرار دارد
// و ورودی دیگر هم از جنس Form است که برای انتخاب حالت تصمیم گیری سر بازیکن ها و تنظیمات و اطلاعات مربوط به Say کردن می باشد.
	void decide(const WorldModel* worldModel, Form &form);

// تابعی که command را بر می گرداند.
	Command *getCommand();
};

#endif // __BODY_DECISION_H
