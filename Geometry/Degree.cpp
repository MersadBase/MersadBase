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
 *  This file is created by: Ahmad Boorghany
 *  	and is modified by: Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>

#include <Line.h>
#include <Degree.h>
#include <Defines.h>
#include <Logger.h>
#include <Vector.h>

using namespace std;
// namespace Degree

namespace Degree
{
	float sin(float direction)
	{
		return ::sin(direction * DEG2RAD);
	}

	float cos(float direction)
	{
		return ::cos(direction * DEG2RAD);
	}

	float tan(float direction)
	{
		return ::tan(direction * DEG2RAD);
	}

	float cot(float direction)
	{
		return 1 / ::tan(direction * DEG2RAD);
	}

	float arcSin(float value)
	{
		return ::asin(value) * RAD2DEG;
	}

	float arcCos(float value)
	{
		return ::acos(value) * RAD2DEG;
	}

	float arcTan(float value)
	{
		return ::atan(value) * RAD2DEG;
	}

	float arcCot(float value)
	{
		return ::atan(1 / value) * RAD2DEG;
	}

	float arcTan2(float y, float x)
	{
		return ::atan2(y,x) * RAD2DEG;
	}

	float arcCot2(float y, float x)
	{
		return arcTan2(y,x); // Reversing arguments because of Cot.
	}

	float normalizeAngle(float angle)
	{
		while (angle < -180) angle += 360;
		while (angle > 180) angle -= 360;
		return angle;
	}

	float absoluteAngle(float angle)
	{
		while (angle < 0) angle += 360;
		while (angle >= 360) angle -= 360;
		return angle;
	}


	bool isBetween(float angle1, float angle2, float checkAngle)
	{
		angle1 = absoluteAngle(angle1);
		angle2 = absoluteAngle(angle2);
		checkAngle = absoluteAngle(checkAngle);

		if (angle1 == angle2)
		{
			if (checkAngle == angle1)
				return true;
			else
				return false;
		}
		else if (angle1 < angle2)
		{
			if (checkAngle >= angle1 && checkAngle <= angle2)
				return true;
			else
				return false;
		}
		else
		{
			if (checkAngle <= angle2 || checkAngle >= angle1)
				return true;
			else
				return false;
		}
	}

	float getDeltaAngle(float angle1,float angle2)
	{
		angle1 = absoluteAngle(angle1);
		angle2 = absoluteAngle(angle2);
		return normalizeAngle(angle1 - angle2);
	}

	float getDegree(Point pos1, Point pos2, Point pos3)
	{
		Vector v1 = pos1 - pos2, v2 = pos3 - pos2;
		return normalizeAngle(v1.getDirection() - v2.getDirection());
	}

	Point turn(Point point, float deg)
	{
		float x = point.x, y = point.y;
		point.x = x * cos(deg) - y * sin(deg);
		point.y = x * sin(deg) + y * cos(deg);
		return point;
	}
}
