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
 *  Released on Monday 13 September 2010, 22 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __WORLD_MODEL_UTILITIES_H
#define __WORLD_MODEL_UTILITIES_H

#include <WorldModelBase.h>
#include <Formation.h>

// در این فایل توابع ابزاری(Utilities) کلاس WorldModel قرار دارند.

class WorldModelUtilities: public WorldModelBase
{
protected:
	static Formation formation, beforeKickOffFormation, nonPlayOnFormation;
	static bool formationSet;

public:
	int checkPlayModeForPenalty() const;

// تعداد بازیکنان حریف را که عقب تر از خط که به عنوان ورودی به تابع داده ایم را بر می گرداند.
	int getOppPlayersBeforeLine(float lineX) const;

// تعداد بازیکنانی را که بین دو خط که به عنوان ورودی به تابع داده ایم را بر می گرداند.
	int getOppPlayersBetweenLines(float line1X, float line2X) const;

	float getTackleProb(bool foul = false) const;

	float getTackleProb(const Ball &theBall, const Body &body, bool foul = false) const;

	float getTackleProb(const Ball &theBall, const Player *player, bool foul = false) const;

	float getTackleEffectivePower(float Direction) const;

// مشخص می کند که آیا توپ قابل ضربه زدن هست یا نه. یعنی توپ در محوطه ی kickable بازیکن ما قرار دارد یا نه.
	bool isBallKickable() const;

// همان تابع بالا است با این تفاوت که می توانیم توپ را خودمان مشخص کنیم.
	bool isBallKickable(const Ball &theBall) const;

// مشخص می کند که توپ را می توان Catch کرد یا نه. یا در حقیقت توپ در ناحیه ی Catchable بازیکن ما قرار دارد یا نه.
	bool isBallCatchable() const;

// همان تابع بالا است با این تفاوت که می توانیم توپ را خودمان مشخص کنیم.
	bool isBallCatchable(const Ball &theBall) const;

	float getCatchProbability() const;

// مشخص می کند که آیا در kickable یکی از هم تیم ها قرار دارد یا نه.
	bool isBallInTmmKickable() const;

// همان تابع بالا است با این تفاوت که می توانیم توپ را خودمان مشخص کنیم.
	bool isBallInTmmKickable(const Ball &theBall) const;

// مشخص می کند که آیا در kickable یکی از بازیکنان حریف قرار دارد یا نه.
	bool isBallInOppKickable() const;

// همان تابع بالا است با این تفاوت که می توانیم توپ را خودمان مشخص کنیم.
	bool isBallInOppKickable(const Ball &theBall) const;

// مشخص می کند توپ درون زمین با اختلاف نیم متر از طول و عرض قرار دارد یا نه.
	bool isBallInField() const;

// همان تابع بالا است با این تفاوت که می توانیم توپ را خودمان مشخص کنیم.
	static bool isBallInField(const Ball &theBall);

// مشخص می کند که آیا بازیکن در نیمه ی زمین ما قرار دارد یا نه. بازیکن را به عنوان ورودی به تابع داده ایم.
	static bool isPlayerInOurField(const Player &player);

// مشخص می کند که آیا بازیکن در نیمه ی زمین ما قرار دارد یا نه. بازیکن را به عنوان ورودی به تابع داده ایم.
	static bool isPlayerInOppField(const Player &player);

// نمونه ای از جنس Formation برای ما بر می گرداند که اطلاعات مربوط به جایگیری بازیکن ها را از فایل PlayOnPositioning.conf خوانده و در اختیار ما می گذارد. برای دیدن توابع مربوط به Formation فایل Formation.h را ببینید.
	const Formation& getFormation() const;

// نمونه ای از جنس Formation برای ما بر می گرداند که اطلاعات مربوط به جایگیری بازیکن ها را از فایل NonPlayOnPositioning.conf خوانده و در اختیار ما می گذارد.
	const Formation& getBeforeKickOffFormation() const;

	const Formation& getNonPlayOnFormation() const;

// این تابع خط آفساید بازیکنان حریف را بر می گرداند. این تابع یک ورودی اختیاری از جنس bool دارد که اگر true باشد به این معنی است که در محاسبه ی خط آفساید توپ را در نظر بگیرد و اگر false باشد یعنی فقط بازیکنان حریف را در نظر بگیرد.
	float getOppOffsideLine(bool isBallFlag = true) const;

// همان تابع بالاست ولی برای بازیکنان تیم خودمان.
	float getTmmOffsideLine(bool isBallFlag = true) const;

	float getTmmDefenseLine() const;

// این تابع خط آفساید بازیکنانی از حریف را بر می گرداند که در اختلاف ۱۰ متری در راستای y و در اختلاف ۲۰ متری در راستای x توپ قرار دارند. مقداری هم که به عنوان ورودی می گیرد مشخص می کند که در هنگام محاسبه ی خط آفساید توپ را در نظر بگیرد یا نه.
	float getOppLocalOffLine(bool isBallFlag = true) const;

// این تابع فاصله ی یک نقطه خارج از زمین (که به عنوان ورودی به تابع داده ایم) تا محوطه ی زمین را خروجی می دهد.
	static float getOutDistance(const Point &point);

// این تابع نزدیک ترین بازیکن حریف به توپ را بر می گرداند که اگر هیچ حریفی در ناحیه ی دید ما وجود نداشت مقدار NULL را بر می گرداند. هنگام کار با این تابع باید همواره چک کنیم که اشاره گر NULL خروجی نداده باشد.
	const Player *getNearestOppToBall() const;

// همان تابع بالاست با این تفاوت که نزدیک ترین باریکن حریف را به نقطه ای که به عنوان ورودی به تابع داده ایم را بر می گرداند.
	const Player *getNearestOppToPoint(const Point & point) const;

	const Player *getNearestOppToPointFromNotList(const Point & point, const std::string& blackPlayers = "") const;

// نردیک ترین بازیکن بین هم تیمی ها به توپ را بر می گرداند. در اینجا لازم نیست چیزی را چک کنیم. چون اگر هیچ بازیکن هم تیمی را هم نبینیم خودمان به عنوان نزدیک ترین بازیکن انتخاب خواهیم شد.
	const Player &getNearestTmmToBall() const;

// همان تابع بالاست با این تفاوت که نزدیک ترین باریکن حریف را به نقطه ای که به عنوان ورودی به تابع داده ایم را بر می گرداند.
	const Player &getNearestTmmToPoint(const Point & point, bool checkMe = true) const;

// نزدیکترین بازیکن هم تیمی به یک نقطه را بر می گرداند. این بازیکن از یک لیست که به صورت string به عنوان ورودی به تابع داده ایم انتخاب می شود. نقطه مورد نظر را هم به عنوان ورودی به تابع می دهیم. باید توجه شود که ممکن است این تابع NULL بازگرداند چون ممکن است هیچ کدام از بازیکنانی که در لیست ما هستند موجود نباشند.
	const Player *getNearestTmmToPointFromList(const Point & point, const std::string& players = "") const;

// همان تابع بالا با این تفاوت که شرط این است که نزدیکترین بازیکن از مکمل لیست داده شده انتخاب می شود. یعنی اگر شما به عنوان ورودی لیست "45678" را بدهیم بازیکن را از لیست "1239AB" انتخاب می کند که منظور از A و B بازیکنان شماره ی ۱۰ و ۱۱ هستند.
	const Player *getNearestTmmToPointFromNotList(const Point & point, const std::string& blackPlayers = "") const;

	const Player *getNearestPlayerToPointFromNotList(const Point & point, const std::string& blackTeammates = "", const std::string& blackOpponents = "") const;

// دروازه بان تیم خودمان را می دهد. باید بررسی شود که NULL نباشد. اگر دروازه بان را نبیند NULL خروجی می دهد.
	const Player *getTmmGoalie() const;

// دروازه بان تیم حریف را می دهد. باید بررسی شود که NULL نباشد. اگر دروازه بان را نبیند NULL خروجی می دهد.
	const Player *getOppGoalie() const;

	float getSecurityStatus(const Player &player, float oval_a = .4, float oval_b = .5) const;
	float getPathSecurityStatus(const Player &srcPlayer, const Point trgPoint, float forwardRate) const;

	bool isOppBallShooted(const Ball &theBall) const;
	bool isOppBallShootedToTir(const Ball &theBall, float upYTirPoint,
								float downYTirPoint, Point &shootIntercept) const;
};

#endif // __WORLD_MODEL_UTILITIES_H
