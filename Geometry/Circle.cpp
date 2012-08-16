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

#include <cmath>

#include <Circle.h>
#include <Line.h>
#include <HalfLine.h>

using namespace std;

Point Circle::getCenter(void)
{
	return centeralPoint;
}

float Circle::getRadius(void)
{
	return radius;
}

void Circle::setByCenterRadius(Point inpCenteralPoint, float inpRadius)
{
	centeralPoint = inpCenteralPoint;
	radius = inpRadius;
}

void Circle::setByThreePoints(Point point1, Point point2, Point point3)
{
}

unsigned Circle::getLineIntersect(Line line, vector <Point> &points)
{
	float a = line.getA(), b = line.getB(), c = line.getC();
	float x0 = centeralPoint.x, y0 = centeralPoint.y;
	float r = radius;
	float formulaA, formulaB, formulaC;
	float delta;
	Point ansPoint1, ansPoint2;

	cout << "Flag 1" << endl;

/*
	delta = pow(((b / a) * (c / a + centeralPoint.x) - centeralPoint.y) - ((b * b) / (a * a) + 1) * (pow((centeralPoint.x + c / a), 2) + centeralPoint.y * centeralPoint.y - radius * radius), 2);

	if(delta < 0)
		return 0;

	ansPoint1.y = (-((b / a) * (c / a + centeralPoint.x) - centeralPoint.y ) + sqrt(delta)) / ( (b * b) / (a * a) + 1);
	ansPoint2.y = (-((b / a) * (c / a + centeralPoint.x) - centeralPoint.y ) - sqrt(delta)) / ( (b * b) / (a * a) + 1);
	ansPoint1.x = -(b / a) * ansPoint1.y - c / a;
	ansPoint2.x = -(b / a) * ansPoint2.y - c / a;
*/

	formulaA = (pow(a, 2) + pow(b, 2)) / pow(a, 2);
	formulaB = ((2 * a * b * x0) - (2 * b * c) - (2 * pow(a, 2) * y0)) /
		pow(a, 2);
	formulaC = (pow(a * x0, 2) + pow(a * y0, 2) - pow(a * r, 2) + pow(c, 2) +
		(2 * a * c * x0)) / pow(a, 2);
	delta = pow(formulaB, 2) - (4 * formulaA * formulaC);

	cout << "Flag 2" << endl;

	if (delta < 0)
		return 0;

	ansPoint1.y = ((0 - formulaB) + sqrt(delta)) / (2 * formulaA);
	ansPoint2.y = ((0 - formulaB) - sqrt(delta)) / (2 * formulaA);

	ansPoint1.x = (0 - ((b * ansPoint1.y) + c)) / a;
	ansPoint2.x = (0 - ((b * ansPoint2.y) + c)) / a;

	cout << "Flag 3" << endl;

	if (delta == 0)
	{
		points.push_back(ansPoint1);
		return 1;
	}

	cout << "Flag 4" << endl;

	cout << "ansPoint1 = " << ansPoint1 << endl;
	cout << "ansPoint2 = " << ansPoint2 << endl;

	points.push_back(ansPoint1);
	points.push_back(ansPoint2);

	cout << "Flag 5" << endl;

	return 2;
}

unsigned Circle::getHalfLineIntersect(HalfLine halfLine, vector <Point> &points)
{
	float a = halfLine.getA(), b = halfLine.getB(), c = halfLine.getC();
	float x0 = centeralPoint.x, y0 = centeralPoint.y;
	float r = radius;
	float formulaA, formulaB, formulaC;
	float delta;
	Point ansPoint1, ansPoint2;


	formulaA = (pow(a, 2) + pow(b, 2)) / pow(a, 2);
	formulaB = ((2 * a * b * x0) - (2 * b * c) - (2 * pow(a, 2) * y0)) /
		pow(a, 2);
	formulaC = (pow(a * x0, 2) + pow(a * y0, 2) - pow(a * r, 2) + pow(c, 2) +
		(2 * a * c * x0)) / pow(a, 2);
	delta = pow(formulaB, 2) - (4 * formulaA * formulaC);

	if (delta < 0)
		return 0;

	ansPoint1.y = ((0 - formulaB) + sqrt(delta)) / (2 * formulaA);
	ansPoint2.y = ((0 - formulaB) - sqrt(delta)) / (2 * formulaA);

	ansPoint1.x = (0 - ((b * ansPoint1.y) + c)) / a;
	ansPoint2.x = (0 - ((b * ansPoint2.y) + c)) / a;

	if (halfLine.checkPoint(ansPoint1) && halfLine.checkPoint(ansPoint2))
	{
		points.push_back(ansPoint1);
		points.push_back(ansPoint2);
		return 2;
	}
	if (halfLine.checkPoint(ansPoint1) && !halfLine.checkPoint(ansPoint2))
	{
		points.push_back(ansPoint1);
		return 1;
	}
	if (!halfLine.checkPoint(ansPoint1) && halfLine.checkPoint(ansPoint2))
	{
		points.push_back(ansPoint2);
		return 1;
	}
	return 0;
}

unsigned Circle::getCircleIntersect(Circle circle, vector <Point> &points)
{
	return 0;
}

bool Circle::checkPoint(Point point)
{
	float centerToPointDist = sqrt((pow((centeralPoint.x - point.x), 2) +
		pow((centeralPoint.y - point.y), 2)));
	if (fabs(centerToPointDist - radius) <= .001)
		return true;
	return false;
}

bool Circle::checkPointInArea(Point point)
{
	float centerToPointDist = sqrt((pow((centeralPoint.x - point.x), 2) +
		pow((centeralPoint.y - point.y), 2)));
	if (centerToPointDist < radius)
		return true;
	return false;
}
