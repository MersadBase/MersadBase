/*
 * Segment.cpp
 *
 *  Created on: 17/08/2012
 *      Author: amin
 */

#include <Segment.h>
#include <Triangle.h>

Segment::Segment()
{
}

Segment::Segment(const Point &_a, const Point &_b)
{
	a = _a;
	b = _b;
}
Point Segment::getA() const
{
	return a;
}

Point Segment::getB() const
{
	return b;
}
void Segment::swapAB()
{
	std::swap(a, b);
}

Line Segment::asLine() const
{
	return Line(a, b);
}

bool Segment::sameSide(const Point &p1, const Point &p2) const
{
	if ( (Vector(a) - Vector(b)).crossProduct(Vector(p1) - Vector(b))
			* (Vector(a) - Vector(b)).crossProduct(Vector(p2) - Vector(b)) >= 0 )
		return true;
	return false;
}

float Segment::getDist(const Point &p) const
{
	if (getLength() == 0)
		return a.getDistance(p);

	float prod = Vector(b - a).innerProduct(p - a);
	if (0 <= prod && prod <= getLength() * getLength())
	{
		return (Triangle(a, b, p).getArea() * 2) / getLength();
	}
	return std::min(a.getDistance(p), b.getDistance(p));

}

float Segment::getLength() const
{
	return a.getDistance(b);
}

Segment::~Segment()
{
	// TODO Auto-generated destructor stub
}

