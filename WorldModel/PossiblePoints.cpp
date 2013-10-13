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
 *  This file is created by: Mohammad Salehe
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>

#include <PossiblePoints.h>
#include <Defines.h>
#include <Degree.h>
#include <Config.h>

#include <Body.h>
#include <Ball.h>
#include <Player.h>

#define POINTS_TABLE_PPDX 0.20f
#define POINTS_TABLE_PPDY 0.20f

#define POINTS_TABLE_BPDX 0.18f
#define POINTS_TABLE_BPDY 0.18f

#define POINTS_TABLE_VDX 0.06f
#define POINTS_TABLE_VDY 0.06f

#define SQRT2 M_SQRT2

#define GOOD_TABLE_POINTS 4

#define VELOCITY_SAFE_AREA 0.14f

#define VALID_POS_DEVIATION 5

using namespace std;

PossiblePoints possiblePoints;

PossiblePoints::PossiblePoints()
{
	data = NULL;
	pointsTablePW = 0;
	pointsTablePH = 0;
}

PossiblePoints::~PossiblePoints()
{
	destroy();
}

void PossiblePoints::view()
{
}


void PossiblePoints::initTableByMiddle(float middleX, float middleY,
	float width, float heigth)
{
	destroy();

	left = middleX - (width / 2);
	down = middleY - (heigth / 2);
	pointsTablePW = (int) (floor((width / pointsTablePDX) + 1));
	pointsTablePH = (int) (floor((heigth / pointsTablePDY) + 1));
	dX = width / pointsTablePW;
	dY = heigth / pointsTablePH;
	xAverage = middleX;
	yAverage = middleY;

	data = new int [pointsTablePW * pointsTablePH];

	reset();
}


void PossiblePoints::initTableByNorthEast(float leftArg, float downArg,
	float width, float heigth)
{
	destroy();

	left = leftArg;
	down = downArg;
	pointsTablePW = (int) (floor((width / pointsTablePDX) + 1));
	pointsTablePH = (int) (floor((heigth / pointsTablePDY) + 1));
	dX = width / pointsTablePW;
	dY = heigth / pointsTablePH;
	xAverage = (left + (width / 2));
	yAverage = (down + (heigth / 2));

	data = new int [pointsTablePW * pointsTablePH];

	reset();
}


void PossiblePoints::initTableForVelocityByNorthEast(float leftArg,
	float downArg, float width, float heigth)
{
	destroy();

	left = leftArg;
	down = downArg;
	pointsTablePW = (int) (floor((width / POINTS_TABLE_VDX) + 1));
	pointsTablePH = (int) (floor((heigth / POINTS_TABLE_VDY) + 1));
	dX = width / pointsTablePW;
	dY = heigth / pointsTablePH;
	xAverage = (left + (width / 2));
	yAverage = (down + (heigth / 2));

	data = new int [pointsTablePW * pointsTablePH];

	reset();
}


void PossiblePoints::reset()
{
	for (int i = 0; i < pointsTablePW; i++)
		for (int j = 0; j < pointsTablePH; j++)
			data[i + pointsTablePW * j] = 0;

	maxData = 0;
}

void PossiblePoints::destroy()
{
	if (data && pointsTablePW && pointsTablePH)
	{
		delete[] data;
		data = NULL;
	}
}


void PossiblePoints::setByFlags(const FieldFlag **flags, int nFlags,
	float headAngle, float quantizeStep)
{
	int i;
	float a, b, c, d, e;
	float distMin, distMax, dirMin, dirMax;

	Vector flagPosition;
	Vector ray;

	if (nFlags == 0)
		return;

	// Initialize Table With Nearest flag
	const FieldFlag* nearestFlag;
	nearestFlag = flags[0];
	for (i = 0; i < nFlags; i++)
		if (flags[i]->getSeeDistance() < nearestFlag->getSeeDistance())
			nearestFlag = flags[i];

	a = nearestFlag->getSeeDistance();
	b = log(a);
	c = quantize(b, quantizeStep);
	quantizeRange(c, quantizeStep, d, e);
	distMin = exp(d);
	distMax = exp(e);


	if (distMin > distMax)
	{
		a = distMax;
		distMax = distMin;
		distMin = a;
	}

	a = nearestFlag->getSeeDirection();
	a += headAngle;

	quantizeRange(a, 1, dirMin, dirMax);

	flagPosition = (nearestFlag->getPos());

	ray.setAsPolar(distMin, dirMin);
	ray = flagPosition - ray;
	a = ray.getX(); // xmin
	b = ray.getY(); // ymin
	c = ray.getX(); // xmax
	d = ray.getY(); // ymax

	ray.setAsPolar(distMin, dirMax);
	ray = flagPosition - ray;
	if (ray.getX() < a)
		a = ray.getX();
	if (ray.getX() > c)
		c = ray.getX();
	if (ray.getY() < b)
		b = ray.getY();
	if (ray.getY() > d)
		d = ray.getY();

	ray.setAsPolar(distMax, dirMin);
	ray = flagPosition - ray;
	if (ray.getX() < a)
		a = ray.getX();
	if (ray.getX() > c)
		c = ray.getX();
	if (ray.getY() < b)
		b = ray.getY();
	if (ray.getY() > d)
		d = ray.getY();

	ray.setAsPolar(distMax, dirMax);
	ray = flagPosition - ray;
	if (ray.getX() < a)
		a = ray.getX();
	if (ray.getX() > c)
		c = ray.getX();
	if (ray.getY() < b)
		b = ray.getY();
	if (ray.getY() > d)
		d = ray.getY();

	dirMin = Degree::absoluteAngle(dirMin);
	dirMax = Degree::absoluteAngle(dirMax);

	if (Degree::isBetween(dirMin, dirMax, 90))
	{
		ray.setAsPolar(distMax, 90);
		ray = flagPosition - ray;
		if (ray.getY() < b)
			b = ray.getY();
		if (ray.getY() > d)
			d = ray.getY();
	}

	if (Degree::isBetween(dirMin, dirMax, 270))
	{
		ray.setAsPolar(distMax, -90);
		ray = flagPosition - ray;
		if (ray.getY() < b)
			b = ray.getY();
		if (ray.getY() > d)
			d = ray.getY();
	}

	if (Degree::isBetween(dirMin, dirMax, 0))
	{
		ray.setAsPolar(distMax, 0);
		ray = flagPosition - ray;
		if (ray.getX() < a)
			a = ray.getX();
		if (ray.getX() > c)
			c = ray.getX();
	}

	if (Degree::isBetween(dirMin, dirMax, 180))
	{
		ray.setAsPolar(distMax, 180);
		ray = flagPosition - ray;
		if (ray.getX() < a)
			a = ray.getX();
		if (ray.getX() > c)
			c = ray.getX();
	}

	initTableByNorthEast(a, b, c - a, d - b);

	int n = 0;
	int t = 0;

	int tempArray1[60];
	float tempArray2[60];
	int tempValue1 = 0;
	float tempValue2 = 0;
	int t2 = 0;

	for (t = 0; t < nFlags; t++)
	{
		tempArray1[t] = t;
		tempArray2[t] = flags[t]->getSeeDistance();
	}

	for (t = 0; t < nFlags; t++)
	{
		for (t2 = t; t2 < nFlags; t2++)
		{
			if (tempArray2[t] > tempArray2[t2])
			{
				tempValue1 = tempArray1[t]; tempArray1[t] = tempArray1[t2]; tempArray1[t2] = tempValue1;
				tempValue2 = tempArray2[t]; tempArray2[t] = tempArray2[t2]; tempArray2[t2] = tempValue2;
			}
		}
	}

	for (t = 0; t < nFlags; t++)
	{
		n = addFlag(flags[tempArray1[t]], headAngle, quantizeStep);

		if (n <= GOOD_TABLE_POINTS && n > 0)
			break; // if is is a good table ,don't check the other flags to IMPROVE SPEED
	}
}

int PossiblePoints::addFlag(const FieldFlag* flag, float headAngle, float quantizeStep)
{
	int i, j;
	int n = 0;
	float a, b, c, d, e;
	float distMin, distMax, dirMin, dirMax;
	float flagX = 0, flagY = 0;

	a = flag->getSeeDistance();
	b = log(a);
	c = quantize(b, quantizeStep);
	quantizeRange(c, quantizeStep, d, e);
	distMin = exp(d);
	distMax = exp(e);

	if (distMin > distMax)
	{
		a = distMax;
		distMax = distMin;
		distMin = a;
	}


	a = flag->getSeeDirection();
	a += headAngle;

	quantizeRange(a, 1, dirMin, dirMax);

	flagX = flag->getPos().getX();
	flagY = flag->getPos().getY();

	Degree::absoluteAngle(dirMin);
	Degree::absoluteAngle(dirMax);

	for (i = 0; i < pointsTablePW; i++)
		for (j = 0; j < pointsTablePH; j++)
			if (data[i + pointsTablePW * j] == maxData)
			{
				a = flagX - (((i + 0.5f) * dX) + left);
				b = flagY - (((j + 0.5f) * dY) + down);
				c = hypot(a, b);
				d = Degree::arcTan2(b, a);
				Degree::absoluteAngle(d);
				if (c > distMin && c < distMax)
					if (Degree::isBetween(dirMin, dirMax, d)) // IsBeTwEeN cAn Be FaStEr BeCaUsE fIrSt TwO aRgUmEnTs ArE cOnStAnT mAnY tImEs
					{
						n++;
						(data[i + pointsTablePW * j])++;
					}
			}

	if (n > 0)
		maxData++;

	return n;
}


void PossiblePoints::calculate()
{
	int i, j;

	if (maxData == 0)
		return;

	float sumX = 0;
	float sumY = 0;
	float sumDeviationX = 0;
	float sumDeviationY = 0;
	float N = 0;

	view();

	for (i = 0; i < pointsTablePW; i++)
		for (j = 0; j < pointsTablePH; j++)
			if (data[i + pointsTablePW * j] == maxData)
			{
				sumX += (((i + 0.5f) * dX) + left);
				sumDeviationX += ((((i + 0.5f) * dX) + left) * (((i + 0.5f) * dX) +
					left));
				sumY += (((j + 0.5f) * dY) + down);
				sumDeviationY += ((((j + 0.5f) * dY) + down) * (((j + 0.5f) * dY) +
					down));
				N += 1;
			}

	xAverage = (sumX / N);
	yAverage = (sumY / N);
	xDeviation = (sumDeviationX / N) - (xAverage * xAverage);
	yDeviation = (sumDeviationY / N) - (yAverage * yAverage);
	xDeviation = sqrt(abs(xDeviation));
	yDeviation = sqrt(abs(yDeviation));
}


int PossiblePoints::addPositionSimulate(float x, float y, float velX, float velY, float rand, float posDeviation, float velDeviation)
{
	float xRandMax, yRandMax;
	int n = 0;

	xRandMax = velX * rand;
	yRandMax = velY * rand;

	xRandMax += (posDeviation + velDeviation) / SQRT2;
	yRandMax += (posDeviation + velDeviation) / SQRT2;

	float iteration;
	int posSimX, posSimY;

	Vector absVelVector, relVelVector;
	absVelVector.setAsCartesian(velX, velY);

	iteration = dX > dY ? dY : dX;
	if (iteration < pointsTablePDX / 2)
		iteration = pointsTablePDX / 2;
	if (iteration < pointsTablePDY / 2)
		iteration = pointsTablePDY / 2;

	for (float i = -xRandMax; i <= xRandMax; i += iteration)
		for (float j = -yRandMax; j <= yRandMax; j += iteration)
		{
			relVelVector.setAsCartesian(i, j);
			relVelVector.rotate(-(absVelVector.getDirection()));
			posSimX = (int) ((relVelVector.getX() + x - left) / dX);
			posSimY = (int) ((relVelVector.getY() + y - down) / dY);
			if (posSimX >= 0 &&
				posSimX < pointsTablePW &&
				posSimY >= 0 &&
				posSimY < pointsTablePH)
				if (data[posSimX + pointsTablePW * posSimY] == maxData)
				{
					data[posSimX + pointsTablePW * posSimY]++;
					n++;
				}
		}

	if (n > 0)
		maxData++;

	return n;
}

void PossiblePoints::setBySee(float distance, float direction, float headAngle, const Vector &agentPosition, float quantizeStep)
{
	float a, b, c, d, e;
	float distMin, distMax, dirMin, dirMax;

	Vector ray;

	a = distance;
	b = log(a);
	c = quantize(b, quantizeStep);
	d = 0;
	e = 0;
	quantizeRange(c, quantizeStep, d, e);
	distMin = exp(d);
	distMax = exp(e);

	if (distMin > distMax)
	{
		a = distMax;
		distMax = distMin;
		distMin = a;
	}

	a = direction;
	a += headAngle;

	quantizeRange(a, 1, dirMin, dirMax);

	ray.setAsPolar(distMin, dirMin);
	ray = ray + agentPosition;
	a = ray.getX(); // xmin
	b = ray.getY(); // ymin
	c = ray.getX(); // xmax
	d = ray.getY(); // ymax

	ray.setAsPolar(distMin, dirMax);
	ray = ray + agentPosition;
	if (ray.getX() < a)
		a = ray.getX();
	if (ray.getX() > c)
		c = ray.getX();
	if (ray.getY() < b)
		b = ray.getY();
	if (ray.getY() > d)
		d = ray.getY();

	ray.setAsPolar(distMax, dirMin);
	ray = ray + agentPosition;
	if (ray.getX() < a)
		a = ray.getX();
	if (ray.getX() > c)
		c = ray.getX();
	if (ray.getY() < b)
		b = ray.getY();
	if (ray.getY() > d)
		d = ray.getY();

	ray.setAsPolar(distMax, dirMax);
	ray = ray + agentPosition;
	if (ray.getX() < a)
		a = ray.getX();
	if (ray.getX() > c)
		c = ray.getX();
	if (ray.getY() < b)
		b = ray.getY();
	if (ray.getY() > d)
		d = ray.getY();

	Degree::absoluteAngle(dirMin);
	Degree::absoluteAngle(dirMax);

	if (Degree::isBetween(dirMin, dirMax, 90))
	{
		ray.setAsPolar(distMax, 90);
		ray = ray + agentPosition;
		if (ray.getY() < b)
			b = ray.getY();
		if (ray.getY() > d)
			d = ray.getY();
	}

	if (Degree::isBetween(dirMin, dirMax, 270))
	{
		ray.setAsPolar(distMax, -90);
		ray = ray + agentPosition;
		if (ray.getY() < b)
			b = ray.getY();
		if (ray.getY() > d)
			d = ray.getY();
	}

	if (Degree::isBetween(dirMin, dirMax, 0))
	{
		ray.setAsPolar(distMax, 0);
		ray = ray + agentPosition;
		if (ray.getX() < a)
			a = ray.getX();
		if (ray.getX() > c)
			c = ray.getX();
	}

	if (Degree::isBetween(dirMin, dirMax, 180))
	{
		ray.setAsPolar(distMax, 180);
		ray = ray + agentPosition;
		if (ray.getX() < a)
			a = ray.getX();
		if (ray.getX() > c)
			c = ray.getX();
	}

	initTableByNorthEast(a, b, c - a, d - b);

//	int n;
//	n = addSee(distance, direction, headAngle, agentPosition, quantizeStep);
}


int PossiblePoints::addSee(float distance, float direction, float headAngle,
	const Vector &agentPosition, float quantizeStep)
{
	int i, j;
	int n = 0;
	float a, b, c, d, e;
	float distMin, distMax, dirMin, dirMax;

	a = distance;
	b = log(a);
	c = quantize(b, quantizeStep);
	d = 0;
	e = 0;
	quantizeRange(c, quantizeStep, d, e);
	distMin = exp(d);
	distMax = exp(e);

	if (distMin > distMax)
	{
		a = distMax;
		distMax = distMin;
		distMin = a;
	}

	a = direction;
	a += headAngle;

	quantizeRange(a, 1, dirMin, dirMax);

	Degree::absoluteAngle(dirMin);
	Degree::absoluteAngle(dirMax);

	for (i = 0; i < pointsTablePW; i++)
		for (j = 0; j < pointsTablePH; j++)
			if (data[i + pointsTablePW * j] == maxData)
			{
				a = (((i + 0.5f) * dX) + left) - agentPosition.getX();
				b = (((j + 0.5f) * dY) + down) - agentPosition.getY();
				c = hypot(a, b);
				d = Degree::arcTan2(b, a);
				Degree::absoluteAngle(d);
				if (c > distMin && c < distMax)
					if (Degree::isBetween(dirMin, dirMax, d)) // IsBeTwEeN cAn Be FaStEr BeCaUsE fIrSt TwO aRgUmEnTs ArE cOnStAnT mAnY tImEs
					{
						n++;
						(data[i + pointsTablePW * j])++;
					}
			}

	if (n > 0)
		maxData++;

	return n;
}

void PossiblePoints::setByVelocitySimulate(float velX, float velY, float headAngle, float rand, float velDeviation)
{
	float a, b, c, d;
	float xRandMax, yRandMax;

	xRandMax = velX * rand;
	yRandMax = velY * rand;

	xRandMax += (velDeviation) / SQRT2;
	yRandMax += (velDeviation) / SQRT2;


	Vector relVelVector, absVelVector;
	absVelVector.setAsCartesian(velX, velY);

	relVelVector.setAsCartesian(-xRandMax, -yRandMax);
	relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() + absVelVector.getDirection());
	relVelVector = relVelVector + absVelVector;
	relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() - headAngle);
	a = relVelVector.getX(); // xmin
	b = relVelVector.getY(); // ymin
	c = relVelVector.getX(); // xmax
	d = relVelVector.getY(); // ymax

	relVelVector.setAsCartesian(-xRandMax, yRandMax);
	relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() + absVelVector.getDirection());
	relVelVector = relVelVector + absVelVector;
	relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() - headAngle);
	if (relVelVector.getX() < a)
		a = relVelVector.getX();
	if (relVelVector.getX() > c)
		c = relVelVector.getX();
	if (relVelVector.getY() < b)
		b = relVelVector.getY();
	if (relVelVector.getY() > d)
		d = relVelVector.getY();

	relVelVector.setAsCartesian(xRandMax, -yRandMax);
	relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() + absVelVector.getDirection());
	relVelVector = relVelVector + absVelVector;
	relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() - headAngle);
	if (relVelVector.getX() < a)
		a = relVelVector.getX();
	if (relVelVector.getX() > c)
		c = relVelVector.getX();
	if (relVelVector.getY() < b)
		b = relVelVector.getY();
	if (relVelVector.getY() > d)
		d = relVelVector.getY();

	relVelVector.setAsCartesian(xRandMax, yRandMax);
	relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() + absVelVector.getDirection());
	relVelVector = relVelVector + absVelVector;
	relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() - headAngle);
	if (relVelVector.getX() < a)
		a = relVelVector.getX();
	if (relVelVector.getX() > c)
		c = relVelVector.getX();
	if (relVelVector.getY() < b)
		b = relVelVector.getY();
	if (relVelVector.getY() > d)
		d = relVelVector.getY();

	a = a - VELOCITY_SAFE_AREA;
	b = b - VELOCITY_SAFE_AREA;
	c = c + VELOCITY_SAFE_AREA;
	d = d + VELOCITY_SAFE_AREA;

	initTableForVelocityByNorthEast(a, b, c - a, d - b);
	view();
}

int PossiblePoints::addVelocitySimulate(float velX, float velY, float headAngle, float rand, float velDeviation)
{
	float xRandMax, yRandMax;
	int n = 0;

	xRandMax = velX * rand;
	yRandMax = velY * rand;

	xRandMax += (velDeviation) / SQRT2;
	yRandMax += (velDeviation) / SQRT2;

	float iteration;
	int posSimX, posSimY;

	Vector absVelVector, relVelVector;
	absVelVector.setAsCartesian(velX, velY);

	iteration = dX > dY ? dY : dX;

	for (float i = -xRandMax; i <= xRandMax; i += iteration)
		for (float j = -yRandMax; j <= yRandMax; j += iteration)
		{
			relVelVector.setAsCartesian(i, j);
			relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() + absVelVector.getDirection());
			relVelVector = relVelVector + absVelVector;
			relVelVector.setAsPolar(relVelVector.getMagnitude(), relVelVector.getDirection() - headAngle);
			posSimX = (int) ((relVelVector.getX() - left) / dX);
			posSimY = (int) ((relVelVector.getY() - down) / dY);
			if (posSimX >= 0 && posSimX < pointsTablePW && posSimY >= 0 && posSimY < pointsTablePH)
				if (data[posSimX + pointsTablePW * posSimY] == maxData)
				{
					data[posSimX + pointsTablePW * posSimY]++;
					n++;
				}
		}

	if (n > 0)
		maxData++;

	view();

	return n;
}

void PossiblePoints::addVelocityDistDirChange(const Vector &headVector, float distChng, float dirChng, float quantizeStep)
{
	float dirChngMin, dirChngMax;
	float distChngMin, distChngMax;
	float a, b, c, d, e, f;
	int i, j, n;
	n = 0;

	quantizeRange(-dirChng, quantizeStep, dirChngMin, dirChngMax);
	dirChngMin -= 0.2;
	dirChngMax += 0.2;

	float distanceMin, distanceMax;

	a = headVector.getMagnitude();
	b = log(a);
	c = quantize(b, quantizeStep);
	d = 0;
	e = 0;
	quantizeRange(c, quantizeStep, d, e);
	distanceMin = exp(d);
	distanceMax = exp(e);

	if (distanceMin > distanceMax)
	{
		a = distanceMax;
		distanceMax = distanceMin;
		distanceMin = a;
	}

	distChngMin = headVector.getMagnitude() * quantize(((distChng / distanceMax)), 0.02) - 0.02;
	distChngMax = headVector.getMagnitude() * quantize(((distChng / distanceMin)), 0.02) + 0.02;

	for (i = 0; i < pointsTablePW; i++)
		for (j = 0; j < pointsTablePH; j++)
			if (data[i + pointsTablePW * j] == maxData)
			{
				a = (((i + 0.5f) * dX) + left);
				b = (((j + 0.5f) * dY) + down);
				c = headVector.getX() / headVector.getMagnitude();
				d = headVector.getY() / headVector.getMagnitude();
				e = (a * c) + (b * d);
				f = ((-(a * d) + (b * c)) / headVector.getMagnitude()) * (180 / PI);

				if (f >= dirChngMin && f <= dirChngMax && e >= distChngMin && e <= distChngMax)
				{
					if (data[i + pointsTablePW * j] == maxData)
					{
						n++;
						(data[i + pointsTablePW * j])++;
					}
				}
			}
	if (n > 0)
		maxData++;

	view();
}

void PossiblePoints::quantizeRange(float v, float q, float &min, float &max)
{
	min = v - (q / 2);
	max = v + (q / 2);
}

float PossiblePoints::quantize(float v, float q)
{
	return (rint(v / q)) * q;
}

void PossiblePoints::setMode(int mode)
{
	if (mode == 1)
	{
		pointsTablePDX = POINTS_TABLE_PPDX;
		pointsTablePDY = POINTS_TABLE_PPDY;
	}
	else if (mode == 2)
	{
		pointsTablePDX = POINTS_TABLE_BPDX;
		pointsTablePDY = POINTS_TABLE_BPDY;
	}
	else
	{
		pointsTablePDX = POINTS_TABLE_BPDX;
		pointsTablePDY = POINTS_TABLE_BPDY;
	}
}

Vector PossiblePoints::calculateBodyPosition(const Body &body, const FieldFlag **flags, int nFlags, float &retPosDeviation, float quantizeStep)
{
	Vector pos;
	setMode(1);

	setByFlags(flags, nFlags, body.getHeadDir(), quantizeStep);
	if (body.getPosDeviation() < VALID_POS_DEVIATION)
		addPositionSimulate(body.getPos().getX(), body.getPos().getY(), body.getVel().getX(), body.getVel().getY(), body.getRand(),
			body.getPosDeviation(), 0);

	calculate();
	pos.setAsCartesian(xAverage, yAverage);
	retPosDeviation = sqrt(((xDeviation * xDeviation) + (yDeviation * yDeviation))) / 2;
	destroy();

	return pos;
}


Vector PossiblePoints::calculatePlayerPosition(const Player &player, const Body &body, float &retPosDeviation, float quantizeStep)
{
	Vector pos;
	setMode(1);

	setBySee(player.getSeeDistance(), player.getSeeDirection(),
		body.getHeadDir(), body.getPos(), quantizeStep);
	if (player.getPosDeviation() < VALID_POS_DEVIATION)
		addPositionSimulate(player.getPos().getX(), player.getPos().getY(), player.getVel().getX(), player.getVel().getY(), player.getRand(),
			player.getPosDeviation(), 0);

	calculate();
	pos.setAsCartesian(xAverage, yAverage);
	retPosDeviation = sqrt(((xDeviation * xDeviation) + (yDeviation * yDeviation))) / 2;
	destroy();

	return pos;
}

Vector PossiblePoints::calculateBallPosition(const Ball &ball, const Body &body, float &retPosDeviation, float quantizeStep)
{
	Vector pos;
	setMode(2);

	setBySee(ball.getSeeDistance(), ball.getSeeDirection(), body.getHeadDir(), body.getPos(), quantizeStep);
	if (ball.getPosDeviation() < VALID_POS_DEVIATION)
		addPositionSimulate(ball.getPos().getX(), ball.getPos().getY(), ball.getVel().getX(), ball.getVel().getY(), ball.getRand(),
			ball.getPosDeviation(), 0);

	calculate();
	pos.setAsCartesian(xAverage, yAverage);
	retPosDeviation = sqrt(((xDeviation * xDeviation) + (yDeviation * yDeviation))) / 2;
	destroy();

	return pos;
}
