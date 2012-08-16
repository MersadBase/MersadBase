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

#include <cmath>

#include <HalfLine.h>
#include <Vector.h>
#include <Defines.h>
#include <Line.h>
#include <Circle.h>
#include <Degree.h>
#include <Logger.h>

using namespace std;
using namespace Degree;

HalfLine::HalfLine(Point point, Point point2)
{
	Vector tempVector(Point(point2 - point));
	HalfLine temhLine;
	temhLine.setByPointDir(point, tempVector.getMagnitude());

//	double dir = temhLine.getDirection();
	double dir = tempVector.getDirection();

	float cs, sn, n;
	if (((dir + 90) / 180) == (int)((dir + 90) / 180))
	{
		b = 0;
		a = 1;
		c = 0 - point.x;
		m = 0xFFFFFF;
	}
	else
	{
		m = Degree::tan(dir);
		sn = Degree::sin(dir);
		cs = Degree::cos(dir);
		n = point.y - (m * point.x);
		a = 0 - sn;
		b = cs;
		c = 0 - (cs * n);
	}
	direction = dir;
	sourcePoint = point;
}

float HalfLine::getA(void)
{
	return a;
}

float HalfLine::getB(void)
{
	return b;
}

float HalfLine::getC(void)
{
	return c;
}

Point HalfLine::getSource(void)
{
	return sourcePoint;
}

float HalfLine::getDirection(void)
{
	return direction;
}

unsigned HalfLine::getHalfLineIntersect(HalfLine halfLine, Point &intersectPoint)
{
	float a1 = a, b1 = b, c1 = c;
	float a2 = halfLine.getA(), b2 = halfLine.getB(), c2 = halfLine.getC();

	intersectPoint.x = ((b1 * c2) - (b2 * c1)) / ((a1 * b2) - (a2 * b1));
	intersectPoint.y = ((a2 * c1) - (a1 * c2)) / ((a1 * b2) - (a2 * b1));

	if (checkPoint(intersectPoint) && halfLine.checkPoint(intersectPoint))
		return 1;
	return 0;
}

unsigned HalfLine::getLineIntersect(Line line, Point &intersectPoint)
{
	float a1 = a, b1 = b, c1 = c;
	float a2 = line.getA(), b2 = line.getB(), c2 = line.getC();

	intersectPoint.x = ((b1 * c2) - (b2 * c1)) / ((a1 * b2) - (a2 * b1));
	intersectPoint.y = ((a2 * c1) - (a1 * c2)) / ((a1 * b2) - (a2 * b1));

	if (checkPoint(intersectPoint))
		return 1;
	return 0;
}

unsigned HalfLine::getCircleIntersect(Circle circle, vector <Point> &points)
{
	Line line;
	line.setBySourceDir(sourcePoint, direction);
	vector <Point> p;
	int tedad = line.getCircleIntersect(circle, p);

	//LOG << "tedad = " << tedad << endl;

	if (tedad == 0)
	{
		return 0;
	}

	/*if (tedad > 0)
		LOG << "p1 = " << p[0] << " is In Right = " << isInRightDirection(p[0]) << endl;
	if (tedad > 1)
		LOG << "p1 = " << p[1] << " is In Right = " << isInRightDirection(p[1]) << endl;*/

	if (tedad == 1)
	{
		if (isInRightDirection(p[0]))
		{
			points.push_back(p[0]);
			return 1;
		}
	}

	if (tedad == 2)
	{
		if (isInRightDirection(p[0]) and isInRightDirection(p[1]))
		{
			points.push_back(p[0]);
			points.push_back(p[1]);
			return 2;
		}

		if (not isInRightDirection(p[0]) and isInRightDirection(p[1]))
		{
			points.push_back(p[1]);
			return 1;
		}

		if (isInRightDirection(p[0]) and not isInRightDirection(p[1]))
		{
			points.push_back(p[0]);
			return 1;
		}
	}

	return 0;
	/*float x0 = circle.getCenter().x, y0 = circle.getCenter().y;
	float r = circle.getRadius();
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

	LOG << "p1 = " << isInRightDirection(ansPoint1) << endl;
	LOG << "p2 = " << isInRightDirection(ansPoint2) << endl;

	if (delta == 1)
	{
		if (isInRightDirection(ansPoint1))
		{
			points.push_back(ansPoint1);
			return 1;
		}
	}

	if (isInRightDirection(ansPoint1) and isInRightDirection(ansPoint2))
	{
		points.push_back(ansPoint1);
		points.push_back(ansPoint2);
		return 2;
	}

	if (not isInRightDirection(ansPoint1) and isInRightDirection(ansPoint2))
	{
		points.push_back(ansPoint2);
		return 1;
	}

	if (isInRightDirection(ansPoint1) and not isInRightDirection(ansPoint2))
	{
		points.push_back(ansPoint1);
		return 1;
	}
*/
	/*if (checkPoint(ansPoint1) && checkPoint(ansPoint2))
	{
		points.push_back(ansPoint1);
		points.push_back(ansPoint2);
		return 2;
	}
	if (checkPoint(ansPoint1) && !checkPoint(ansPoint2))
	{
		points.push_back(ansPoint1);
		return 1;
	}
	if (!checkPoint(ansPoint1) && checkPoint(ansPoint2))
	{
		points.push_back(ansPoint2);
		return 1;
	}*/
	return 0;
}

bool HalfLine::checkPoint(Point point)
{
	if (((a * point.x) + (b * point.y) + c) == 0)
	{
		Vector pointToSource;
		pointToSource.setByPoints(point, sourcePoint);
		if (Degree::normalizeAngle(pointToSource.getDirection()) == direction ||
			Degree::normalizeAngle(pointToSource.getDirection()) ==
			Degree::normalizeAngle(direction + 180))
			return true;
	}
	return false;
}

void HalfLine::setByPointDir(Point point, float dir)
{
	float cs, sn, n;
	if (((dir + 90) / 180) == (int)((dir + 90) / 180))
	{
		b = 0;
		a = 1;
		c = 0 - point.x;
		m = 0xFFFFFF;
	}
	else
	{
		m = Degree::tan(dir);
		sn = Degree::sin(dir);
		cs = Degree::cos(dir);
		n = point.y - (m * point.x);
		a = 0 - sn;
		b = cs;
		c = 0 - (cs * n);
	}
	direction = dir;
	sourcePoint = point;
}

float HalfLine::getPointDist(Point point)
{
	Line tempLine;
	Point interPoint;
	Vector tempVector;

	tempLine.setBySourceDir(point, Degree::normalizeAngle(direction + 90));
	if (getLineIntersect(tempLine, interPoint) != 1)
		return NOVALUE;
	tempVector.setByPoints(interPoint,point);

	return tempVector.getMagnitude();
}

bool HalfLine::isInRightDirection(Point p)
{
	Vector point;
	point.setByPoints(sourcePoint, p);
	//LOG << "Direction = " << direction << " point to source = "  << point.getDirection() << endl;
	//LOG << "Delta = " << getDeltaAngle(point.getDirection(), direction) << endl;
	//LOG << "Menha = " << fabs(point.getDirection() - direction) << endl;
	return fabs(getDeltaAngle(point.getDirection(), direction)) < 90.0;
}
