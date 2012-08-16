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

#ifndef __FULLLINE__H
#define __FULLLINE__H

#include <Types.h>

class FullLine
{
protected:
	Point firstPoint;
	Point secondPoint;

public:
	FullLine();
	FullLine(Point a, Point b);
	FullLine(float x1, float y1, float x2, float y2);

	FullLine& operator+=(FullLine fl);
	FullLine& operator-=(FullLine fl);

	void setFirstPoint(Point a);
	void setSecondPoint(Point a);

	Point getFirstPoint();
	Point getSecondPoint();

	float getMagnitude();

	float getDistance(Point p);
};

bool operator < (FullLine fl1, FullLine fl2);
bool operator > (FullLine fl1, FullLine fl2);

#endif // __FULLLINE_H
