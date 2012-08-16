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

#ifndef __LINE__H
#define __LINE__H

#include <vector>
#include <Types.h>
#include <Defines.h>

class HalfLine;
class Circle;

class Line
{
protected:
	float a, b, c;
	float m;
	float direction;

public:
	Line(Point a, Point b);
	Line(){}

	float getA(void);
	float getB(void);
	float getC(void);
	float getDirection(void);

	double getY(double x);

	Point getLineIntersect(Line line);
	unsigned getLineIntersect(Line line, Point &intersectPoint);
	unsigned getHalfLineIntersect(HalfLine halfLine, Point &intersectPoint);
	unsigned getCircleIntersect(Circle circle, std::vector<Point> &points);
	float getPointDist(Point point);
	bool checkPoint(Point point);

	void setByABC(float inpA, float inpB, float inpC);
	void setBySourceDir(Point point, float Dir);
	void setByPoints(Point point1, Point point2);
};

float getShib(Point pos1, Point pos2);
float getDist(Point pos1,Point pos2);
bool getdistance(Point pos1, Point pos2, Point pos3, Point pos4);

#endif // __LINE_H
