/*
 * Triangle.cpp
 *
 *  Created on: 17/08/2012
 *      Author: amin
 */

#include "Triangle.h"

Triangle::Triangle()
{

}

Triangle::Triangle(Point _a, Point _b, Point _c)
{
	a = _a;
	b = _b;
	c = _c;
}

Point Triangle::getA() const
{
	return a;
}

Point Triangle::getB() const
{
	return b;
}

Point Triangle::getC() const
{
	return c;
}

Segment Triangle::getAB() const
{
	return Segment(a, b);
}

Segment Triangle::getAC() const
{
	return Segment(a, c);
}

Segment Triangle::getBC() const
{
	return Segment(b, c);
}

float Triangle::getADeg() const
{
	float ab = Segment(a, b).getLength();
	float ac = Segment(a, c).getLength();
	float bc = Segment(b, c).getLength();
	if (!ab || !ac || !bc)
		return 0;
	return Degree::arcCos((bc * bc - ab * ab - ac * ac) / (-2.f * ab * ac));
}
float Triangle::getBDeg() const
{
	float ab = Segment(a, b).getLength();
	float ac = Segment(a, c).getLength();
	float bc = Segment(b, c).getLength();
	if (!ab || !ac || !bc)
		return 0;
	return Degree::arcCos((ac * ac - ab * ab - bc * bc) / (-2.f * ab * bc));
}
float Triangle::getCDeg() const
{
	float ab = Segment(a, b).getLength();
	float ac = Segment(a, c).getLength();
	float bc = Segment(b, c).getLength();
	if (!ab || !ac || !bc)
		return 0;
	return Degree::arcCos((ab * ab - bc * bc - ac * ac) / (-2.f * bc * ac));
}

float Triangle::getArea() const
{
	float ab = Segment(a, b).getLength();
	float ac = Segment(a, c).getLength();
	float bc = Segment(b, c).getLength();
	float p = (ab + ac + bc) / 2;
	return sqrt((p - ab) * (p - bc) * (p - ac) * p);
}

bool Triangle::isInTriangle(const Point &p) const
{
	Point avg = (a + b + c) / 3;
	if (getAB().sameSide(avg, p) && getAC().sameSide(avg, p)
			&& getBC().sameSide(avg, p))
		return true;
	return false;
}

Triangle::~Triangle()
{
	// TODO Auto-generated destructor stub
}

