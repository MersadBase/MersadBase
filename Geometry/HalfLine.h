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
 *  This file is created by: Mostafa Rokooey
 *  	and is modified by: Sassan Haradji, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __HALFLINE__H
#define __HALFLINE__H

#include <vector>
#include <Types.h>
#include <Line.h>

class Circle;

class HalfLine
{
protected:
	float a, b, c;
	float m;
	Point sourcePoint;
	float direction;

public:

	HalfLine(){}
	HalfLine(Point point, Point point2);

	float getA(void);
	float getB(void);
	float getC(void);
	Point getSource(void);
	float getDirection(void);

	unsigned getLineIntersect(Line line, Point &intersectPoint);
	unsigned getHalfLineIntersect(HalfLine halfLine, Point &intersectPoint);
	unsigned getCircleIntersect(Circle circle, std::vector<Point> &points);
	float getPointDist(Point point);
	bool checkPoint(Point point);

	void setByABC(float inpA, float inpB, float inpC);
	void setByPointDir(Point point, float Dir);

	bool isInRightDirection(Point p);
};

#endif // __HALFLINE_H
