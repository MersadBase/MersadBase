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
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __CIRCLE__H
#define __CIRCLE__H

#include <vector>
#include <Types.h>
#include <Line.h>

class HalfLine;

class Circle
{
protected:
	Point centeralPoint;
	float radius;

public:
	Point getCenter(void);
	float getRadius(void);

	void setByCenterRadius(Point inpCenteralPoint, float inpRadius);
	void setByThreePoints(Point point1, Point point2, Point point3);

	unsigned getLineIntersect(Line line, std::vector<Point> &points);
	unsigned getHalfLineIntersect(HalfLine halfLine, std::vector<Point> &points);
	unsigned getCircleIntersect(Circle circle, std::vector<Point> &points);
	bool checkPoint(Point point);
	bool checkPointInArea(Point point);

};

#endif // __CIRCLE_H
