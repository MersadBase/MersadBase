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
 *  This file is created by: Meisam Vosoughpour, Mohammad Salehe
 *  	and is modified by: Pooria Kaviani, Mohammad Zareie, Mehrdad Bakhtiari
 *
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>

#include <cstdlib>
#include <Degree.h>
#include <Basics.h>
#include <Defines.h>
#include <BasicTurn.h>
#include <Line.h>
#include <FullLine.h>
#include <Logger.h>
#include <BasicDash.h>

using namespace std;
using namespace Degree;

DashNormalGotoPoint::DashNormalGotoPoint(const Point target, float distance, const Body &body, float maxDashPower, float dir_thr, bool TOF): BasicAction()
{
	float deltaAngle, validDeltaAngle;
	Vector targetVector;
	targetVector.setByPoints(body.getPos(), target);
	deltaAngle = getDeltaAngle(targetVector.getDirection(), body.getBodyDir());
	distance = max((float) EPS, distance);
	validDeltaAngle = arcTan2(distance, targetVector.getMagnitude());

	mode = CT_EMPTY;
	command = NULL;

	float dashPower = 100;
	if (body.getStamina() < 2600)
		dashPower = 35;
	if (dashPower > maxDashPower)
		dashPower = maxDashPower;

	if (abs(deltaAngle) <= dir_thr or abs(deltaAngle) <= validDeltaAngle)
	{
		if (targetVector.getMagnitude() > distance)
		{
			command = new DashCommand(dashPower);
			mode = CT_DASH;
		}
		else
		{
			command = TurnToPoint(target, body).getCommand();
			mode = CT_TURN;
		}
	}
	else
	{
		command = TurnToPoint(target, body).getCommand();
		mode = CT_TURN;
	}
}

DashNormalGotoPoint::DashNormalGotoPoint(const Point target, float distance, const Body &body,
		float idealBodyDir, float maxDashPower): BasicAction()
{
	float deltaAngle, validDeltaAngle;
	Vector targetVector;
	targetVector.setByPoints(body.getPos(), target);
	deltaAngle = getDeltaAngle(targetVector.getDirection(), body.getBodyDir());
	distance = max((float) EPS, distance);
	validDeltaAngle = arcTan2(distance, targetVector.getMagnitude());

	mode = CT_EMPTY;
	command = NULL;

	float dashPower = 100;
	if (body.getStamina() < 2500)
		dashPower = 35;
	if (dashPower > maxDashPower)
		dashPower = maxDashPower;

	if (abs(deltaAngle) <= idealBodyDir or abs(deltaAngle) <= validDeltaAngle)
	{
		if (targetVector.getMagnitude() > distance or abs(deltaAngle) <= idealBodyDir)
		{
			command = new DashCommand(dashPower);
			mode = CT_DASH;
		}
		else
		{
			command = TurnToAngle(idealBodyDir, body).getCommand();
			mode = CT_TURN;
		}
	}
	else
	{
		command = TurnToPoint(target, body).getCommand();
		mode = CT_TURN;
	}

	if (maxDashPower == 0)
	{
		if (command == NULL)
			command = TurnToAngle(idealBodyDir, body).getCommand();
		else
		{
			delete command;
			command = TurnToAngle(idealBodyDir, body).getCommand();
		}
	}
}

Command *DashNormalGotoPoint::getCommand()
{
	return command;
}

CommandType DashNormalGotoPoint::getMode()
{
	return mode;
}

GoalieGotoPoint::GoalieGotoPoint(const Point target, const Body &body, float idealBodyDir): BasicAction()
{
	Vector targetVector(target - body.getPos());

	mode = CT_EMPTY;
	command = NULL;

	const float maxDashPower = 100.f;
	const float maxDistToLine = 0.5;
	const float maxDistToTarget = 3.f;
	const float maxYDistToTarget = 0.8;
	const float dist_thr = 0.2;
	const float dir_thr = 5;
	const float maxX_thr = 2.5;

	float dashPower = 100;
	if (body.getStamina() < 2500)
		dashPower = 35;
	if (dashPower > maxDashPower)
		dashPower = maxDashPower;

	if ( fabs(body.getPos().getY() - target.y) > maxDistToTarget and fabs( normalizeAngle(body.getBodyDir() - targetVector.getDirection()) ) < 10 )
	{
		LOG << "\tDashNormalGotoPoint(target)" << endl;
		DashNormalGotoPoint DNGotoPoint(target, dist_thr, body);
		command = DNGotoPoint.getCommand();
		mode = DNGotoPoint.getMode();
		return;
	}

	if ( fabs(body.getPos().getX() - target.x) > maxX_thr )
	{
		LOG << "x thr is BIG! DashNormalGotoPoint" << endl;
		DashNormalGotoPoint DNGotoPoint(target, dist_thr, body);
		command = DNGotoPoint.getCommand();
		mode = DNGotoPoint.getMode();
		return;
	}

	if ( fabs(body.getBodyDir() - idealBodyDir) > dir_thr )
	{
		LOG << "changing body dir" << endl;
		command = TurnToAngle(idealBodyDir, body).getCommand();
		mode = CT_TURN;
		return;
	}
	if ( fabs(target.y - body.getPos().getY()) > maxYDistToTarget )
	{
		LOG << "fix y distance" << endl;
		if ( fabs( normalizeAngle(body.getBodyDir() - targetVector.getDirection()) ) > 90.f )
			dashPower *= -1;
		command = new DashCommand(dashPower);
		mode = CT_DASH;
	}
	else
	{
		LOG << "fix x distance" << endl;
		LOG << "distToLine : " << fabs(target.x - body.getPos().getX()) << endl;
		if ( fabs(target.x - body.getPos().getX()) < maxDistToLine )
		{
			LOG << "distToLine is ideal" << endl;
			return;
		}

		float sideDashAngle = 90;
		if (target.x < body.getPos().getX())
			sideDashAngle = -idealBodyDir;
		else
			sideDashAngle = idealBodyDir;
		command = new DashCommand(dashPower, sideDashAngle);
		mode = CT_DASH;
	}
}

Command* GoalieGotoPoint::getCommand()
{
	return command;
}

CommandType GoalieGotoPoint::getMode()
{

	return mode;
}

DangerAreaGotoPoint::DangerAreaGotoPoint(const Point target, const Body &body, const Ball &ball, Point ballPos, float maxDashPower): BasicAction()
{
	//dash_power
	float dashPower = 100;
	if (body.getStamina() < 2500)
		dashPower = 35;
	if (dashPower > maxDashPower)
		dashPower = maxDashPower;
	//dash_power

	float MAX_NECK_DELTA_ANGLE = 120;
	float dir_thr = 10, dist_thr = 1;

	Point top_left(-53, 34),
			bottom_right(-36, -34);
	Rectangle DangerField(top_left, bottom_right);
//	if ( DangerField.isInRectangle(body.getPos()) )
//		dist_thr = 5;

//	if (ballPos.x < -47)
		dist_thr = 1.5;

	float deltaAngle;
	Vector targetVector, ballVector;
	targetVector.setByPoints(body.getPos(), target);
	ballVector.setByPoints(body.getPos(), ballPos);
	deltaAngle = getDeltaAngle(targetVector.getDirection(), ballVector.getDirection());
	float myBodyDir = body.getBodyDir();
	LOG << "my body direction : " << myBodyDir << endl;

	mode = CT_EMPTY;
	command = NULL;

	LOG << "dist_thr : " << dist_thr << endl;
	LOG << "targetVector.getMagnitude : " << targetVector.getMagnitude() << endl;
	LOG << "targetVector Dir : " << targetVector.getDirection() << endl;
	if (abs(deltaAngle) < MAX_NECK_DELTA_ANGLE /* normal GotoPoint*/)
	{
		LOG << "use normal dash" << endl;
		if ( (fabs(normalizeAngle(myBodyDir - targetVector.getDirection())) < dir_thr) and targetVector.getMagnitude() > dist_thr)
		{
			LOG << "do dash" << endl;
			command = new DashCommand(dashPower);
			mode = CT_DASH;
		}
		else
		{
			LOG << "do turn" << endl;
			command = TurnToPoint(target, body).getCommand();
			mode = CT_TURN;
		}
	}
	else/* backDash GotoPoint*/
	{
		LOG << "use back dash" << endl;
//		Vector backDashTargetVector = targetVector;
		Vector backDashTargetVector;
		backDashTargetVector.setAsPolar(targetVector.getMagnitude(), normalizeAngle(targetVector.getDirection() - 180));
//		backDashTargetVector.rotate(180);
//		backDashTargetVector.normalize();
		LOG << "backDashTargetVector.getMagnitude : " << backDashTargetVector.getMagnitude() << endl;
		LOG << "backDashTargetVector Dir : " << backDashTargetVector.getDirection() << endl;
		if (body.getStamina() < 2600)
			dashPower = 0;
		if ( fabs(normalizeAngle(backDashTargetVector.getDirection() - myBodyDir)) < dir_thr and backDashTargetVector.getMagnitude() > dist_thr)
		{
			LOG << "do dash" << endl;
			command = new DashCommand(dashPower, 180);
			mode = CT_DASH;
		}
		else
		{
			LOG << "do turn" << endl;
			command = TurnToAngle(backDashTargetVector.getDirection(), body).getCommand();
			mode = CT_TURN;
		}
	}
}

Command* DangerAreaGotoPoint::getCommand()
{
	return command;
}

CommandType DangerAreaGotoPoint::getMode()
{
	return mode;
}

PerfectGotoPoint::PerfectGotoPoint(const Point target, const Body& body, double dist_thr, double dashPower,
									double dir_thr, bool useBackDash, bool forceBackDash)
{
	LOG << "Entering PerfectGotoPoint" << endl;
	if (forceBackDash)
		useBackDash = true;
	else if (useBackDash and target.getDistance(body.getPos()) > 10)
		useBackDash = false;

	float deltaAngle;
	Vector targetVector;
	targetVector.setByPoints(body.getPos(), target);
	deltaAngle = getDeltaAngle(targetVector.getDirection(), body.getBodyDir());
	double thetaAngle = getDeltaAngle(targetVector.getDirection(),normalizeAngle(body.getBodyDir() - 180));

	LOG << "delta angle : " << deltaAngle << endl;
	LOG << "theta angle : " << thetaAngle << endl;

	mode = CT_EMPTY;
	command = NULL;

	double maxDashPower = 100;
	if (body.getStamina() < 2500 and dashPower > 35)
		dashPower = 35;
	if (dashPower > maxDashPower)
		dashPower = maxDashPower;

	if (body.getPos().getDistance(target) < dist_thr)
		dashPower = 0;

	double bestDashPower = body.getSpeedMax() * ( 1.0 - body.getDecay() );
	bestDashPower /= ( body.getEffort() * body.getDashPowerRate() );
	LOG << "dashPower is " << dashPower << endl;
	LOG << "bestDashPower is " << bestDashPower << endl;

	if (dashPower > bestDashPower)
	{
		dashPower = bestDashPower;
		LOG << "so dash power changed to " << bestDashPower << endl;
	}

	if
	(
		(
			abs(deltaAngle) <= dir_thr and !forceBackDash
		)
		or
		(
			useBackDash and abs(thetaAngle) <= dir_thr
		)
	)
	{
//		double direction = 0;
		if (useBackDash and abs(thetaAngle) <= dir_thr)
//			direction = 180;
			dashPower *= -1;

		command = new DashCommand(dashPower/*, direction*/);
		mode = CT_DASH;
	}
	else
	{
		float targetVectorDir = targetVector.getDirection();
		if
		(
			forceBackDash
			or
			(useBackDash and fabs(deltaAngle) > fabs(thetaAngle))
		)
			targetVectorDir = normalizeAngle(targetVectorDir - 180);

		command = TurnToAngle(targetVectorDir, body, 0).getCommand();
		mode = CT_TURN;
	}
}

Command*
PerfectGotoPoint::getCommand()
{
	return command;
}

CommandType
PerfectGotoPoint::getMode()
{
	return mode;
}

void ExactGotoPoint::setDashPower(const Body &body, const Point targetPoint,
		float defaultDashPower, float &dashPower)
{
	Body stoppedBody;
	float localDistance;

	stoppedBody = body;
	while (stoppedBody.getVel().getMagnitude() > 0.01)
		stoppedBody.simulateByDynamics();

	if (defaultDashPower >= 0)
	{
		localDistance = stoppedBody.getDistance(targetPoint);

		dashPower = ((localDistance * (1 - body.getDecay())) / body.getDashPowerRate()) / body.getEffort();

		if (dashPower > body.getMaxPower())
			dashPower = body.getMaxPower();

		if (dashPower > body.getStaminaIncMax() and body.getStamina() < 1350)
	 		dashPower = body.getStaminaIncMax();
	}
	else
	{
		localDistance = stoppedBody.getDistance(targetPoint);
		dashPower = (-localDistance * (1 - body.getDecay()) / body.getDashPowerRate()) / body.getEffort();;
		if (dashPower < body.getMinPower())
			dashPower = body.getMinPower();
		if (abs(dashPower) > (body.getStaminaIncMax() / 2) and body.getStamina() < 1400)
			dashPower = -(body.getStaminaIncMax() / 2);
	}
}

ExactGotoPoint::ExactGotoPoint(const Point targetPoint, float deltaDistance, const Body &body, bool negDashFlag): BasicAction()
{
	float pointDistance = body.getDistance(targetPoint);

	float deltaAngleFront, deltaAngleBack, angleFront, angleBack, cautionAngleBack, cautionAngleFront, dashPower;

	command = NULL;
	mode = CT_EMPTY;

	cautionAngleBack = 20;
	cautionAngleFront = 20;

	angleFront = body.getBodyDir();
	angleBack = body.getBodyDir() - 180;
	angleBack = Degree::normalizeAngle(angleBack);

	if (pointDistance >= deltaDistance)
	{
		Vector pointVector;
		pointVector.setByPoints(body.getPos(), targetPoint);

		deltaAngleFront = pointVector.getDirection() - angleFront;
		deltaAngleFront = Degree::normalizeAngle(deltaAngleFront);
		deltaAngleBack = pointVector.getDirection() - angleBack;
		deltaAngleBack = Degree::normalizeAngle(deltaAngleBack);

		if (abs(deltaAngleFront) < cautionAngleFront)
		{
			setDashPower(body, targetPoint, 100, dashPower);
		 	command = new DashCommand(dashPower);
			mode = CT_DASH;
		}
		else if (abs(deltaAngleBack) < cautionAngleBack and negDashFlag)
		{
			setDashPower(body, targetPoint, -100, dashPower);
		 	command = new DashCommand(dashPower);
			mode = CT_DASH;
		}
		else
		{
	 		float maxTurnAngle, deltaMaxTurnAngle;
			maxTurnAngle = BasicTurn::getTurnAngle(179, body);
			deltaMaxTurnAngle = abs(deltaAngleFront) - maxTurnAngle;

			if (deltaMaxTurnAngle < cautionAngleFront / 2.00 or not negDashFlag)
			{
				command = TurnAngle(deltaAngleFront, body).getCommand();
				mode = CT_TURN;
			}
			else
			{
				deltaMaxTurnAngle = deltaAngleBack - maxTurnAngle;

				if (deltaMaxTurnAngle < cautionAngleBack / 2.00)
				{
					command = TurnAngle(deltaAngleBack, body).getCommand();
					mode = CT_TURN;
				}
				else
				{
					command = TurnAngle(deltaAngleFront, body).getCommand();
					mode = CT_TURN;
				}
			}
		}
	}
}

Command *ExactGotoPoint::getCommand()
{
	return command;
}

CommandType ExactGotoPoint::getMode()
{
	return mode;
}

FreeKickGotoPoint::FreeKickGotoPoint(const Point point, float standDirection, float collisionDistance,
		const Body &body): BasicAction()
{
	Vector pointVector;
	Vector standVector;
	Vector targetVector;
	Vector goVector;
	Point targetPoint;
	float distance;
	bool intersectFlag = false;

	command = NULL;
	standVector.setAsPolar(body.getSize() + collisionDistance + 0.1, standDirection + 180);
	targetPoint = Vector(point) + standVector;
	pointVector.setByPoints(body.getPos(), point);
	targetVector.setByPoints(body.getPos(), targetPoint);

	Line targetLine;
	targetLine.setByPoints(body.getPos(), targetPoint);
	distance = targetLine.getPointDist(point);

	if (distance < body.getSize() + collisionDistance + 0.01 and targetVector.getMagnitude() > standVector.getMagnitude() and
		pointVector.getMagnitude() < targetVector.getMagnitude())
	{ // intersect
		float angleDiff;

		intersectFlag = true;
		angleDiff = normalizeAngle(absoluteAngle(standVector.getDirection()) - absoluteAngle(180 + pointVector.getDirection()));
		standVector.setAsPolar(body.getSize() + collisionDistance + 2.0, pointVector.getDirection() + (angleDiff / 2) + 180);
		targetPoint = Vector(point) + standVector;
		targetVector.setByPoints(body.getPos(), targetPoint);
	}
	goVector = targetVector;

	ExactGotoPoint doGotoPoint(targetPoint, 0.2, body);
	command = doGotoPoint.getCommand();

	if (intersectFlag or doGotoPoint.getMode() != CT_EMPTY)
		status = 1;
	else
		status = 0;
}

Command *FreeKickGotoPoint::getCommand()
{
	return command;
}

int FreeKickGotoPoint::getStatus() const
{
	return status;
}

ForbiddenAreaGotoPoint::ForbiddenAreaGotoPoint(const Point point, float rectX1, float rectY1, float rectY2,
		const Body &body): BasicAction()
{
	int sourcePlace = 0, goModel = 0;

	command = NULL;

	rectY1 = min(rectY1, rectY2);
	rectY2 = max(rectY1, rectY2);

	forbiddenSourceFlag = false;
	forbiddenTargetFlag = false;
	realTargetPlace = 1;
	realTargetPoint = point;
	goAroundFlag = false;

	if (point.x > rectX1 and point.y > rectY1 and point.y < rectY2)
	{
		float minDistance;
		forbiddenTargetFlag = true;

		realTargetPoint.x = rectX1;
		minDistance = point.x - rectX1;

		if (point.y - rectY1 < minDistance)
		{
			realTargetPoint.x = point.x;
			realTargetPoint.y = rectY1;
			minDistance = point.y - rectY1;
		}

		if (rectY2 - point.y < minDistance)
		{
			realTargetPoint.x = point.x;
			realTargetPoint.y = rectY2;
			minDistance = rectY2 - point.y;
		}
	}

	if (body.getPos().getX() > rectX1 + 0.8 and body.getPos().getY() > rectY1 + 0.8 and body.getPos().getY() < rectY2 - 0.8)
	{
		forbiddenSourceFlag = true;
		float minDistance;

		realTargetPoint.x = rectX1;
		minDistance = body.getPos().getX() - rectX1;

		if (body.getPos().getY() - rectY1 < minDistance)
		{
			realTargetPoint.x = body.getPos().getX();
			realTargetPoint.y = rectY1;
			minDistance = body.getPos().getY() - rectY1;
		}

		if (rectY2 - body.getPos().getY() < minDistance)
		{
			realTargetPoint.x = body.getPos().getX();
			realTargetPoint.y = rectY2;
			minDistance = rectY2 - body.getPos().getY();
		}
		DashNormalGotoPoint doGotoPoint(realTargetPoint, 1.2, body, 90);
		command = doGotoPoint.getCommand();
		if (doGotoPoint.getMode() == CT_EMPTY)
			status = 0;
		else
			status = 1;
		return;
	}

	if (realTargetPoint.x > rectX1 and realTargetPoint.y >= rectY2)
		realTargetPlace = 1;
	if (realTargetPoint.x <= rectX1 and realTargetPoint.y >= rectY2)
		realTargetPlace = 2;
	if (realTargetPoint.x <= rectX1 and realTargetPoint.y < rectY2 and
		realTargetPoint.y > rectY1)
		realTargetPlace = 3;
	if (realTargetPoint.x <= rectX1 and realTargetPoint.y <= rectY1)
		realTargetPlace = 4;
	if (realTargetPoint.x > rectX1 and realTargetPoint.y <= rectY1)
		realTargetPlace = 5;

	if (body.getPos().getX() > rectX1 and body.getPos().getY() >= rectY2)
		sourcePlace = 1;
	if (body.getPos().getX() <= rectX1 and body.getPos().getY() >= rectY2)
		sourcePlace = 2;
	if (body.getPos().getX() <= rectX1 and body.getPos().getY() < rectY2 and
		body.getPos().getY() > rectY1)
		sourcePlace = 3;
	if (body.getPos().getX() <= rectX1 and body.getPos().getY() <= rectY1)
		sourcePlace = 4;
	if (body.getPos().getX() > rectX1 and body.getPos().getY() <= rectY1)
		sourcePlace = 5;

	if (sourcePlace == realTargetPlace)
		goModel = 0;
	else if (abs(sourcePlace - realTargetPlace) == 1)
		goModel = 0;
	else if ((sourcePlace == 1 and realTargetPlace == 3) or (sourcePlace == 1 and realTargetPlace == 4) or (sourcePlace == 1 and realTargetPlace == 5))
		goModel = 1;
	else if (sourcePlace == 2 and realTargetPlace == 4)
		goModel = 0;
	else if (sourcePlace == 2 and realTargetPlace == 5)
		goModel = 2;
	else if (sourcePlace == 3 and realTargetPlace == 1)
		goModel = 1;
	else if (sourcePlace == 3 and realTargetPlace == 5)
		goModel = 2;
	else if (sourcePlace == 4 and realTargetPlace == 1)
		goModel = 1;
	else if (sourcePlace == 4 and realTargetPlace == 2)
		goModel = 0;
	else if ((sourcePlace == 5 and realTargetPlace == 1) or (sourcePlace == 5 and realTargetPlace == 2) or (sourcePlace == 5 and realTargetPlace == 3))
		goModel = 2;

	command = NULL;

	if (goModel == 0)
	{
		DashNormalGotoPoint doGotoPoint(realTargetPoint, 1.4, body, 90);
		command = doGotoPoint.getCommand();
		if (doGotoPoint.getMode() == CT_EMPTY)
			status = 0;
		else
			status = 1;
	}

	if (goModel == 1)
	{
		goAroundFlag = true;
		DashNormalGotoPoint doGotoPoint(Point(rectX1 - 1.5, rectY2 + 1.5), 1, body, 90);
		command = doGotoPoint.getCommand();
		if (doGotoPoint.getMode() == CT_EMPTY)
			status = 0;
		else
			status = 1;
	}

	if (goModel == 2)
	{
		goAroundFlag = true;
		DashNormalGotoPoint doGotoPoint(Point(rectX1 - 1.5, rectY1 - 1.5), 1, body, 90);
		command = doGotoPoint.getCommand();
		if (doGotoPoint.getMode() == CT_EMPTY)
			status = 0;
		else
			status = 1;
	}
}

ForbiddenAreaGotoPoint::ForbiddenAreaGotoPoint(const Point point, const Point rectNW, const Point rectSE,
		const Body &body): BasicAction()
{
	ForbiddenAreaGotoPoint(rectNW.x, rectNW.y, rectSE.y, body);
}

ForbiddenAreaGotoPoint::ForbiddenAreaGotoPoint(const Point point, const Point circleCenter, float circleRadius,
		const Body &body, bool forbiddenX): BasicAction()
{
	LOG << "Entering ForbiddenAreaGotoPoint" << endl;
	LOG << "circleRadius : " << circleRadius << endl;
	LOG << "circleCenter : " << circleCenter << endl;
	LOG << "point (final target) : " << point << endl;

	FullLine myRoute(body.getPos(), Vector(point));
	if (myRoute.getDistance(Vector(circleCenter)) > 9.0)
	{
		LOG << "Use PerfectGotoPoint" << endl;
		command = PerfectGotoPoint(point, body, 1.f, 100, 15, false).getCommand();
		return;
	}

	float firstDir, dirConfine, finalDir;
	firstDir = Vector(body.getPos() - circleCenter).getDirection();
	finalDir = Vector(point - circleCenter).getDirection();
	dirConfine = getDeltaAngle(finalDir, firstDir);
	LOG << "dirConfine : " << dirConfine << endl;

	float targetDir = firstDir + (dirConfine / fabs(dirConfine)) * 30;
	targetDir = normalizeAngle(targetDir);
	LOG << "firstDir : " << firstDir << endl;
	LOG << "finalDir : " << finalDir << endl;
	Vector target;
	float thr = 0.5;
	target.setAsPolar(circleRadius + thr, targetDir);
	target += circleCenter;
	LOG << "ForbiddenAreaGotoPoint target : " << target << endl;
	command = PerfectGotoPoint(target, body, 1.f, 100, 20, false).getCommand();
}

Command *ForbiddenAreaGotoPoint::getCommand()
{
	return command;
}

bool ForbiddenAreaGotoPoint::isForbiddenSource() const
{
	return forbiddenSourceFlag;
}

bool ForbiddenAreaGotoPoint::isForbiddenTarget() const
{
	return forbiddenTargetFlag;
}

int ForbiddenAreaGotoPoint::getRealTargetPlace() const
{
	return realTargetPlace;
}

Point ForbiddenAreaGotoPoint::getRealTargetPoint() const
{
	return realTargetPoint;
}

bool ForbiddenAreaGotoPoint::isClockwise() const
{
	return clockwiseFlag;
}

int ForbiddenAreaGotoPoint::getStatus() const
{
	return status;
}
