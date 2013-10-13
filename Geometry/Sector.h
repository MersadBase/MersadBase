/*
 * Sector.h
 *
 *  Created on: Oct 13, 2013
 *      Author: amin
 */

#ifndef SECTOR_H_
#define SECTOR_H_
#include <Types.h>

class Sector {
protected:
	Point center;
	float radius;
	float minDir;
	float maxDir;
public:
	Sector(const Point& center,float radius,float minDir,float maxDir);
	const Point& getCenter() const;
	void setCenter(const Point& center);
	float getRadius() const;
	void setRadius(float radius);
	bool contain(const Point& point);
};

#endif /* SECTOR_H_ */
