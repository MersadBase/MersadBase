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

#include <cmath>
#include <Rectangle.h>

Rectangle::Rectangle()
{
	firstPoint = Point(0, 0);
	secondPoint = Point(0, 0);
	center = Point(0, 0);
}

Rectangle::Rectangle(Point a, Point b)
{
	firstPoint = a;
	secondPoint = b;
	length = getLength();
	width = getWidth();
}

Rectangle::Rectangle(double x1, double y1, double x2, double y2)
{
	firstPoint = Point(x1, y1);
	secondPoint = Point(x2 ,y2);
	length = getLength();
	width = getWidth();
}

Rectangle Rectangle::assignFromCenter(Point c, double len, double wid)
{
	return Rectangle(c.x - len*0.5, c.y - wid*0.5, len, wid );
}

Rectangle& Rectangle::operator+=(Rectangle rect)
{
	firstPoint += rect.getFirstPoint();
	secondPoint += rect.getSecondPoint();
	return *this;
}

Rectangle& Rectangle::operator-=(Rectangle rect)
{
	firstPoint -= rect.getFirstPoint();
	secondPoint -= rect.getSecondPoint();
	return *this;
}

bool Rectangle::isInRectangle(Point a) const
{
	float minX = std::min(firstPoint.x, secondPoint.x);
	float maxX = std::max(firstPoint.x, secondPoint.x);
	float minY = std::min(firstPoint.y, secondPoint.y);
	float maxY = std::max(firstPoint.y, secondPoint.y);

	if (a.x >= minX && a.x <= maxX && a.y <= maxY && a.y >= minY)
		return true;
	return false;
}

void Rectangle::setFirstPoint(Point a)
{
	firstPoint = a;
	length = getLength();
	width = getWidth();
}

void Rectangle::setSecondPoint(Point a)
{
	secondPoint = a;
	length = getLength();
	width = getWidth();
}

Point Rectangle::getFirstPoint() const
{
	return firstPoint;
}

Point Rectangle::getSecondPoint() const
{
	return secondPoint;
}

Point Rectangle::getCenter()
{
	center = Point((firstPoint.x + secondPoint.x) / 2, (firstPoint.y + secondPoint.y) / 2);
	return center;
}

double Rectangle::getWidth() const
{
	return secondPoint.y - firstPoint.y;
}

double Rectangle::getLength() const
{
	return secondPoint.x - firstPoint.x;
}

Line Rectangle::getLeftEdge() const
{
	Point leftVector = firstPoint;
	leftVector -= Point(0.0, width);
	Line leftEdge;
	leftEdge.setByPoints(firstPoint, leftVector);
	return leftEdge;
}

Line Rectangle::getRightEdge() const
{
	Point rightVector = secondPoint;
	rightVector += Point(0.0, width);
	Line rightEdge;
	rightEdge.setByPoints(secondPoint, rightVector);
	return rightEdge;
}

Point Rectangle::getTopRightPoint() const
{
	Point topRight(firstPoint);
	topRight += Point(length, 0.0);
	return topRight;
}

Point Rectangle::getBottomLeftPoint() const
{
	Point bottomLeft(secondPoint);
	bottomLeft -= Point(length, 0.0);
	return bottomLeft;
}

Point Rectangle::getTopLeftPoint() const
{
	return firstPoint;
}

Point Rectangle::getBottomRightPoint() const
{
	return secondPoint;
}
