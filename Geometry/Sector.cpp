/*
 * Sector.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: amin
 */

#include <Sector.h>
#include <Vector.h>
#include <Degree.h>

Sector::Sector(const Point& center, float radius, float minDir, float maxDir) :
		center(center), radius(radius), minDir(minDir), maxDir(maxDir) {

}

const Point& Sector::getCenter() const {
	return center;
}

void Sector::setCenter(const Point& center) {
	this->center = center;
}

float Sector::getRadius() const {
	return radius;
}

void Sector::setRadius(float radius) {
	this->radius = radius;
}

bool Sector::contain(const Point& point) {
	Vector v = point - center;
	if (v.getMagnitude() < radius
			&& Degree::isBetween(minDir, maxDir, v.getDirection()))
		return true;
	return false;
}
