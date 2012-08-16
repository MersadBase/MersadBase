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

#ifndef __DEGREE_H
#define __DEGREE_H

#include <Types.h>

// در این فایل توابعی قرار دارند که برای کار با زاویه ساخته شده اند. به کمک این توابع شما راحت تر خواهید توانست با زاویه کار کنید.

namespace Degree
{
// این تابع زاویه ی شما را normalize می کند. در normalize کردن زاویه ی شما به بازه ی -۱۸۰ تا ۱۸۰ درجه می رسد. به عنوان مثال اگر شما به عنوان ورودی به تابع عدد -۷۵۰ را بدهید تابع عدد -۳۰ را بر می گرداند.
	extern float normalizeAngle(float angle);

// این تابع مقدار مطلق یک زاویه را بر می گرداند. منظور از مقدار مطلق مقداری است که بین ۰ تا ۳۶۰ درجه باشد. به عنوان مثال اگر شما به تابع عدد -۳۰ را بدهید تابع به شما ۳۳۰ را خروجی می دهد.
	extern float absoluteAngle(float angle);

// این تابع مشخص می کند که یک زاویه بین دو زاویه ی دیگر قرار دارد یا نه. دو ورودی اول این تابع دو زاویه ای هستند که می خواهیم ببینیم زاویه ی سوم بین آن دو قرار دارد یا نه.
	extern bool isBetween(float angle1, float angle2, float checkAngle);
	extern float getDeltaAngle(float angle1,float angle2);

// اختلاف درجه دو زاویه را نشان می دهد. به عنوان مثال اگر ورودی های ۳۰ و ۳۳۰ را به عنوان ورودی بدهیم تابع به ما ۶۰ را خروجی می دهد.
	extern float getDeltaAngle(float angle1, float angle2);

// این تابع زاویه ی بین سه نقطه را می دهد. در زیر دو مثال از استفاده از این تابع و خروجی آن را داریم:
// getDegree(Point(1, 1), Point(0, 0), Point(-1, 1)) --> 90
// getDegree(Point(-1, 1), Point(0, 0), Point(1, 1)) --> -90
	extern float getDegree(Point pos1, Point pos2, Point pos3);

// این تابع یک نقطه را با میزان درجه ی داده شده به تابع حول مبدا مختصات می چرخاند.
	extern Point turn(Point point, float deg);

// همه ی توابع زیر توابع مثلثاتی هستند که واحد آنها درجه هست(نه رادیان)
	extern float sin(float direction);
	extern float cos(float direction);
	extern float tan(float direction);
	extern float cot(float direction);
	extern float arcSin(float value);
	extern float arcCos(float value);
	extern float arcTan(float value);
	extern float arcCot(float value);
	extern float arcTan2(float y, float x);
	extern float arcCot2(float y, float x);
	extern float getDegree(Point pos1, Point pos2, Point pos3);
	extern Point turn(Point point, float deg);
}

#endif // __DEGREE_H
