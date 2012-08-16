/*
 *  Copyright 2002-2009, Mersad Team, Allameh Helli High School (NODET).
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

#ifndef __POSSIBLEPOINTS_H
#define __POSSIBLEPOINTS_H

#include <cmath>
#include <Types.h>
#include <vector>
#include <Object.h>

class ConfigGroup;
typedef Param ConfigGroup;
class Body;
class Player;
class Ball;

class PossiblePoints
{
public:
	//	int data [pointsTablePW] [pointsTablePH];

	PossiblePoints();
	~PossiblePoints();

	int* data;

	// Debug Function
	void view();

	void initTableByMiddle(float middleX, float middleY, float width,
		float heigth);
	void initTableForVelocityByNorthEast(float left, float down, float width,
		float heigth);
	void initTableByNorthEast(float left, float down, float width,
		float heigth);

	void reset();

	void destroy();

	// Agent Position
	void setByFlags(const FieldFlag **flags, int nFlags, float HeadAnle,
		float quantizeStep);
	int addFlag(const FieldFlag* flag, float headAngle, float quantizeStep);

	// Object Position
	void setBySee(float distance, float direction, float headAngle,
		const Vector &agentPosition, float quantizeStep);
	int addSee(float distance, float direction, float headAngle,
		const Vector &agentPosition, float quantizeStep);

	// Agent Position + Object Position
	int addPositionSimulate(float x, float y, float velX, float velY,
		float rand, float posDeviation, float velDeviation);

	//Object Velocity
	void setByVelocitySimulate(float velX, float velY, float headAngle,
		float rand, float velDeviation);
	int addVelocitySimulate(float velX, float velY, float headAngle,
		float rand, float velDeviation);
	void addVelocityDistDirChange(const Vector &headVector, float distChng,
		float dirChng, float quantizeStep);

	// calculate ...
	void calculate();

	// Outputs
	float xAverage, yAverage;
	float xDeviation, yDeviation;

	// mode
	void setMode(int mode); // 1 *= player 2 *= ball

	Vector calculateBodyPosition(const Body &body, const FieldFlag **flags,
		int nFlags, float &retPosDeviation, float quantizeStep);
	Vector calculatePlayerPosition(const Player &player, const Body &body,
		float &retPosDeviation, float quantizeStep);
	Vector calculateBallPosition(const Ball &ball, const Body &body,
		float &retPosDeviation, float quantizeStep);

private:
	float left, down;
	float dX, dY;
	int maxData;
	int pointsTablePW, pointsTablePH;

	void quantizeRange(float v, float q, float &min, float &max);
	float quantize(float v, float q);

	float pointsTablePDX;
	float pointsTablePDY;
};

extern PossiblePoints possiblePoints;

#endif // __POSSIBLEPOINTS_H
