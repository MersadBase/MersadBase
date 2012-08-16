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
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __OBJECT_H
#define __OBJECT_H

#include <Vector.h>
#include <FieldData.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

#define MAX_POS_DEV 20
#define MAX_VEL_DEV 5

class ConfigGroup;
class SExpression;
typedef ConfigGroup Param;

// در این فایل کلاس Object قرار دارد. کلاس Object پدر کلاس های Player و Ball و چند کلاس دیگر که با آن ها کاری نداریم هست. این کلاس برای خواص مشترکی که در این دو کلاس وجود داشت ساخته شده. خواص مشترکی که یک شیء را معرفی می کنند.
class Object
{
protected:
	bool readyToUpdateFlag;
	float posDeviation;
	float seePosDeviation;
	float velDeviation;
	float seeDistance;
	float seeDirection;
	float seeDistChange;
	float seeDirChange;
	unsigned seeTime;
	unsigned velSeeTime;

	Vector position;
	Vector velocity;
	Vector absVector;
	Vector bodyVector;
	Vector headVector;

	// Local serverparams for Updates and Simulates
	float accelMax;
	float decay;
	float rand;
	float size;
	float speedMax;
	float maxMoment;
	float minMoment;
	float maxPower;
	float minPower;
	float quantizeStep;
	float quantizeStepL;

public:
	Object();
	virtual ~Object();

	virtual void kill();

// این تابع مشخصات یک نمونه از کلاس Object را تنظیم می کند. مشخصاتی که از طرف سرور برای هر شیء تعیین می شوند. مثلا accelMax که بیان گر بیشینه ی شتاب یک شیء می باشد که در سرور مشخص است و برای بازیکنان فرستاده می شود و توسط این تابع آن اطلاعات برای شیء تنظیم می شوند. همان طور که می بینید اطلاعات سرور در کلاس Param ذخیره می شوند.
	void setServerParamVars(const Param &serverParam);

// این تابع از توابع Base است و شما با آن کار نخواهید داشت. این تابع عبارتی که سرور برای Base می فرستد را بررسی و تحلیل می کند و مشخصات Object را بر اساس آن اطلاعات تنظیم می کند. از جمله ی این مشخصات مکان یک شیء است که این مشخصات با مشخصاتی همچون بیشینه ی شتاب متفاوت هستند. این ویژگی ها در طول زمان بازی تغییر می کنند در حالی که ویژگی هایی همچون بیشینه ی شتاب در طول زمان بازی ثابت است.
	void parse(const SExpression &exp, unsigned curTime);

// این تابع فاصله ی یک شیء تا شیءی دیگر را بر می گرداند. شما باید شیء دیگر را به عنوان ورودی به این تابع بدهید.
	float getDistance(const Object &obj) const;
// این تابع فاصله ی یک شیء تا یک نقطه را بر می گرداند. شما باید نقطه را به عنوان ورودی به این تابع بدهید.
	float getDistance(const Point &point) const;

	// Getting functions
	bool isReadyToUpdate() const;
// میزان دقت یا خطای مکان یک شیء را بر می گرداند. مثلا ما توب را به چه دقتی در مکان فعلی اش می بینیم.
// این خطا در هنگام simulateByDynamics(که در کلاس ‌Body توضیحات مربوط به آن آمده) و هنگامی که یک شیء را نبینیم زیاد می شود.
	float getPosDeviation() const;

// همان تعریف بالا است با این تفاوت که دیگر در زمان simulateByDynamics این خطا زیاد نمی شود.
	float getSeePosDeviation() const;

// همانند getPosDeviation است با این تفاوت که خطا برای سرعت را مشخص می کند نه مکان.
	float getVelDeviation() const;

	float getSeeDistance() const;
	float getSeeDirection() const;
	float getSeeDistChange() const;
	float getSeeDirChange() const;
	unsigned getSeeTime() const;
	unsigned getVelSeeTime() const;

	float getAccelMax() const;
	float getDecay() const;
	float getRand() const;
	float getSize() const;
	float getSpeedMax() const;
	float getMaxMoment() const;
	float getMinMoment() const;
	float getMaxPower() const;
	float getMinPower() const;
	float getQuantizeStep() const;
	float getQuantizeStepL() const;

// این تابع مکان شیء را بر می گرداند.
	const Vector &getPos() const;

// این تابع سرعت شیء را بر می گرداند.
	const Vector &getVel() const;

	const Vector &getAbsVec() const;
	const Vector &getBodyVec() const;
	const Vector &getHeadVec() const;

	// Setting functions
	void setReadyToUpdate(bool readyToUpdateFlagArg);
	void setPosDeviation(float posDeviationArg);
	void setSeePosDeviation(float posDeviationArg);
	void setVelDeviation(float velDeviationArg);
	void setSeeDistance(float seeDistanceArg);
	void setSeeDirection(float seeDirectionArg);
	void setSeeDistChange(float seeDistChangeArg);
	void setSeeDirChange(float seeDirChangeArg);
	void setSeeTime(unsigned seeTimeArg);
	void setVelSeeTime(unsigned velSeeTimeArg);

	void setDecay(float decayArg);

// با این تابع می توان مکان شیء را تنظیم کرد.
	Vector &setPos();

// با این تابع می توان سرعت شیء را تنظیم کرد.
	Vector &setVel();
	Vector &setAbsVec();
	Vector &setBodyVec();
	Vector &setHeadVec();
};

class FieldFlag: public Object
{
protected:
	int id;

public:
	void update(const FieldData &fieldData);
	void parse(int idArg, const SExpression &exp, unsigned curTime);

	int getId() const;
};

class FieldLine: public Object
{
protected:
	int id;
	float prependOrient;

public:
	void update(const FieldData &fieldData);
	void parse(int idArg, const SExpression &exp, unsigned curTime);

	int getId() const;
	float getPrependOrient() const;
};

class FieldGoal: public Object
{
protected:
	int id;

public:
	void update(const FieldData &fieldData);
	void parse(int idArg, const SExpression &exp, unsigned curTime);
	void parseFullState(int idArg, const SExpression &exp, unsigned curTime);

	int getId() const;
};

#endif // __OBJECT_H
