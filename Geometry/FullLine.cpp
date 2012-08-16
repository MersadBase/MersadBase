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

#include <FullLine.h>
#include <cmath>

FullLine::FullLine()
{
	firstPoint = Point(0, 0);
	secondPoint = Point(0, 0);
}

FullLine::FullLine(Point a, Point b)
{
	firstPoint = a;
	secondPoint = b;
}

FullLine::FullLine(float x1, float y1, float x2, float y2)
{
	firstPoint = Point(x1, y1);
	secondPoint = Point(x2 ,y2);
}

FullLine& FullLine::operator+=(FullLine fl)
{
	firstPoint += fl.getFirstPoint();
	secondPoint += fl.getSecondPoint();
	return *this;
}

FullLine& FullLine::operator-=(FullLine fl)
{
	firstPoint -= fl.getFirstPoint();
	secondPoint -= fl.getSecondPoint();
	return *this;
}

void FullLine::setFirstPoint(Point a)
{
	firstPoint = a;
}

void FullLine::setSecondPoint(Point a)
{
	secondPoint = a;
}

Point FullLine::getFirstPoint()
{
	return firstPoint;
}

Point FullLine::getSecondPoint()
{
	return secondPoint;
}

float FullLine::getMagnitude()
{
	return hypot(firstPoint.x - secondPoint.x, firstPoint.y - secondPoint.y);
}

float FullLine::getDistance(Point p)
{
	double xu = p.x - firstPoint.x;
	double yu = p.y - firstPoint.y;
	double xv = secondPoint.x - firstPoint.x;
	double yv = secondPoint.y - firstPoint.y;
	if (xu * xv + yu * yv < 0)
		return sqrt( xu * xu + yu * yu );

	xu = p.x - secondPoint.x;
	yu = p.y - secondPoint.y;
	xv = -xv;
	yv = -yv;
	if (xu * xv + yu * yv < 0)
		return sqrt( xu * xu + yu * yu );

	return fabs( ( p.x * ( firstPoint.y - secondPoint.y ) + p.y * ( secondPoint.x - firstPoint.x ) + ( firstPoint.x * secondPoint.y - secondPoint.x * firstPoint.y ) ) / sqrt( ( secondPoint.x - firstPoint.x ) * ( secondPoint.x - firstPoint.x ) + ( secondPoint.y - firstPoint.y ) * ( secondPoint.y - firstPoint.y ) ) );
}

bool operator < (FullLine fl1, FullLine fl2)
{
	if(fl1.getMagnitude() < fl2.getMagnitude())
		return true;
	return false;
}

bool operator > (FullLine fl1, FullLine fl2)
{
	if(fl1.getMagnitude() < fl2.getMagnitude())
		return false;
	return true;
}
