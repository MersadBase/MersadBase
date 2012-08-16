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

#ifndef __FORMATION_H
#define __FORMATION_H

#include <Types.h>
#include <string>
#include <vector>
#include <utility>

// در این فایل کلاس Formation قرار دارد که مربوط به خواند اطلاعات جایگیری از فایل های مربوط به آنهاست.

class Formation
{
private:
	std::vector <std::pair <Point, Point> > positions[12];

	float getNum(std::string line, int &lastPos);
	Point getPoint(std::string line, int &lastPos);

public:
	Formation(std::string fileName);
	~Formation();

	void swap(float &a, float &b) const;
	void swap(Point &a, Point &b) const;

// این تابع جای یک بازیکن را بر اساس جای توپ بر می گرداند. ورودی اول شماره ی بازیکن و ورودی دوم مکان توپ است. این تابع ابتدا از اطلاعات Positioning دو نقطه را که به مکان توپ نزدیکتر هستند را پیدا می کند. سپس نقطه ای را روی خط بین آن دو نقطه با توجه به اینکه توپ به کدام یک از دونقطه نزدیک تر است بر می گرداند.
	const Point getPosition(int number, Point ballPos, float powArg = 1.f) const;

// این تابع همانند تابع بالاست با این تفاوت که دیگر نقطه ی وسط آن خط را نمی دهد بلکه نزدیکترین نقطه به مکان توپ را پیدا می کند و جای بازیکن را در آن حالت بر می گرداند.
	const Point getExactPosition(int number, Point ballPos) const;
};

#endif // __FORMATION_H
