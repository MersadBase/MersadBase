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
 *  	and is modified by: Mohammad Salehe, Sassan Haradji, Pooria Kaviani, Mohammad Zareie, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __BASICS_H
#define __BASICS_H

#include <string>

// در این فایل توابعی وجود دارند که کار با آنها بسیاری از کدهای شما را کوتاه می کند و در کد شما دوباره نویسی این توابع نخواهد بود.

enum ReRateMode {
    RRM_DIRECT,
    RRM_REVERSE
};

namespace Basics
{
// این تابع می بیند که آیا یک شماره ی بازیکن در لیست داده شده هستند یا نه. مثلا آیا بازیکن i در بین بازیکنان "13467" وجود دارد یا نه. اگر بخواهیم این را با کمک if بنویسیم باید پنج or بنویسیم تا این شرط چک شود. این کار را به کمک این تابع می توانیم به صورت زیر انجام دهیم:
// if (isInPlayers(i, "13467"))
// که بسیار کوتاه تر از شکلی بود که قرار بود چند or نوشته شود. در این تابع شما بازیکنان مورد نظر خود را به صورت یک رشته به تابع می دهید و برای مشخص کردن بازیکنان ۱۰ و ۱۱ از A و B استفاده کنید.
	extern bool isInPlayers(const unsigned num, const std::string&);
	extern unsigned charToUniNum(const char str);
	extern char uniNumToChar(const unsigned num);
	extern unsigned firstPlayerInList(const std::string list,
			const std::string blackList);

// این تابع یک مقدار و دو بازه می گیرد. که این مقدار با value و بازه ی اول با a و b و بازه ی دوم هم با c و d مشخص شده اند. تابع reRate فرض می کند که value در بازه ی اول قرار دارد و آن مقدار را طوری به بازه ی دوم می برد که جای value در بین دو سر بازه ی اول قرار دارد نسبت به بازه ی جدید تغییر نکند.
	extern float reRate(float value, float a, float b, float c, float d,
			ReRateMode mode = RRM_DIRECT);
	extern float sigmoid(float x);
	extern unsigned changeIntBase(int number, int base, std::string &str);
	extern unsigned baghimande(int number, unsigned base);

// این تابع علامت عدد ورودی اش را مشخص می کند. اگر ورودی بیشتر از صفر باشد ۱ و اگر کمتر از صفر باشد -۱ و اگر هم خود صفر باشد صفر را به عنوان علامت خروجی می دهد.
	extern int sign(float number);
	extern unsigned pairPlayer(unsigned, unsigned = 0);
	extern std::string pairPlayers(std::string, unsigned = 0);

	template < typename  T >
	extern const T& min_max(const T& low, const T& x, const T& high)
	{
		return min( max( low, x ), high );
	}
}


#endif // __‌BASICS_H

