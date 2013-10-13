/*
 * Triangle.h
 *
 *  Created on: 17/08/2012
 *      Author: amin
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <Types.h>
#include <Segment.h>
#include <cmath>
#include <Degree.h>
class Triangle
{
private:
	Point a,b,c;
public:
	Triangle();
	Triangle *ta,*tb,*tc;
	Triangle(Point _a,Point _b,Point _c);

	Point getA()const;
	Point getB()const;
	Point getC()const;
	Segment getAB()const;
	Segment getAC()const;
	Segment getBC()const;
	float getADeg()const;
	float getBDeg()const;
	float getCDeg()const;
	float getArea()const;
	bool isInTriangle(const Point &p)const;


	virtual ~Triangle();
};

#endif /* TRIANGLE_H_ */
