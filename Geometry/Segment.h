/*
 * Segment.h
 *
 *  Created on: 17/08/2012
 *      Author: amin
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <Types.h>
#include <algorithm>
#include <Line.h>
#include <Vector.h>

class Segment
{
protected:
	Point a,b;
public:
	Segment();
	Segment(const Point &_a,const Point &_b);
	Point getA()const;
	Point getB()const;
	void swapAB();
	Line asLine()const;
	bool sameSide(const Point &p1,const Point &p2)const;
	float getDist(const Point &p1)const;
	float getLength()const;

	virtual ~Segment();
};

#endif /* SEGMENT_H_ */
