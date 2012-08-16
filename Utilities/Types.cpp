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
 *  	and is modified by: Sassan Haradji, Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>

#include <Types.h>
#include <sstream>
#include <cassert>
#include <Player.h>
#include <FastIC.h>

using namespace std;

Point& Point::operator+=(const Point point)
{
	x += point.x;
	y += point.y;
	return *this;
}

Point& Point::operator-=(const Point point)
{
	x -= point.x;
	y -= point.y;
	return *this;
}

ostream &operator<<(ostream &stream, const TeamId &var)
{
	switch (var)
	{
		case TID_TEAMMATE:
			stream << "TID_TEAMMATE";
			break;
		case TID_OPPONENT:
			stream << "TID_OPPONENT";
			break;
		case TID_UNKNOWN:
			stream << "TID_UNKNOWN";
			break;
	}

	return stream;
}

ostream &operator<<(ostream &stream, const Player &var)
{
	stream << "Player Number: " << var.getUniNum() << " Team: " << var.getTeamId();
	return stream;
}

ostream &operator<<(ostream &stream, const Point &point)
{
	stream << "(" << point.x << "," << point.y << ")";
	return stream;
}

ostream &operator<<(ostream &stream, const PlayMode &var)
{
	switch (var)
	{
	case PM_TIME_UP_WITHOUT_A_TEAM:
		stream << "PM_TIME_UP_WITHOUT_A_TEAM";
		break;
	case PM_TIME_UP:
		stream << "PM_TIME_UP,";
		break;
	case PM_BEFORE_KICK_OFF:
		stream << "PM_BEFORE_KICK_OFF";
		break;
	case PM_PLAY_ON:
		stream << "PM_PLAY_ON";
		break;
	case PM_KICK_OFF:
		stream << "PM_KICK_OFF";
		break;
	case PM_KICK_OFF_OPP:
		stream << "PM_KICK_OFF_OPP";
		break;
	case PM_FREE_KICK:
		stream << "PM_FREE_KICK";
		break;
	case PM_FREE_KICK_OPP:
		stream << "PM_FREE_KICK_OPP";
		break;
	case PM_KICK_IN:
		stream << "PM_KICK_IN";
		break;
	case PM_KICK_IN_OPP:
		stream << "PM_KICK_IN_OPP";
		break;
	case PM_OFFSIDE:
		stream << "PM_OFFSIDE";
		break;
	case PM_OFFSIDE_OPP:
		stream << "PM_OFFSIDE_OPP";
		break;
	case PM_GOAL:
		stream << "PM_GOAL";
		break;
	case PM_GOAL_OPP:
		stream << "PM_GOAL_OPP";
		break;
	case PM_GOAL_KICK:
		stream << "PM_GOAL_KICK";
		break;
	case PM_GOAL_KICK_OPP:
		stream << "PM_GOAL_KICK_OPP";
		break;
	case PM_CORNER_KICK:
		stream << "PM_CORNER_KICK";
		break;
	case PM_CORNER_KICK_OPP:
		stream << "PM_CORNER_KICK_OPP";
		break;
	case PM_FREE_KICK_FAULT:
		stream << "PM_FREE_KICK_FAULT";
		break;
	case PM_FREE_KICK_FAULT_OPP:
		stream << "PM_FREE_KICK_FAULT_OPP";
		break;
	case PM_CATCH_FAULT:
		stream << "PM_CATCH_FAULT";
		break;
	case PM_CATCH_FAULT_OPP:
		stream << "PM_CATCH_FAULT_OPP";
		break;
	case PM_BACK_PASS:
		stream << "PM_BACK_PASS";
		break;
	case PM_BACK_PASS_OPP:
		stream << "PM_BACK_PASS_OPP";
		break;
	case PM_FOUL_CHARGE:
		stream << "PM_FOUL_CHARGE";
		break;
	case PM_FOUL_CHARGE_OPP:
		stream << "PM_FOUL_CHARGE_OPP";
		break;
	case PM_PENALTY_SETUP:
		stream << "PM_PENALTY_SETUP";
		break;
	case PM_PENALTY_SETUP_OPP:
		stream << "PM_PENALTY_SETUP_OPP";
		break;
	case PM_PENALTY_READY:
		stream << "PM_PENALTY_READY";
		break;
	case PM_PENALTY_READY_OPP:
		stream << "PM_PENALTY_READY_OPP";
		break;
	}
	return stream;
}

ostream &operator<<(ostream &stream, const BallStatus &var)
{
	switch (var)
	{
	case BS_NONE:
		stream << "BS_NONE";
		break;

	case BS_KICKABLE_TMM:
		stream << "BS_KICKABLE_TMM";
		break;

	case BS_KICKABLE_OPP:
		stream << "BS_KICKABLE_OPP";
		break;

	case BS_KICKABLE_BODY:
		stream << "BS_KICKABLE_BODY";
		break;

	case BS_KICKABLE_BODY_TMM:
		stream << "BS_KICKABLE_BODY_TMM";
		break;

	case BS_KICKABLE_BODY_OPP:
		stream << "BS_KICKABLE_BODY_OPP";
		break;

	case BS_KICKABLE_TMM_OPP:
		stream << "BS_KICKABLE_TMM_OPP";
		break;

	case BS_FREE_BALL:
		stream << "BS_FreeBall";
		break;

	case BS_FREE_BALL_TMM:
		stream << "BS_FREE_BALL_TMM";
		break;

	case BS_FREE_BALL_OPP:
		stream << "BS_FREE_BALL_OPP";
		break;

	case BS_FREE_BALL_BODY:
		stream << "BS_FREE_BALL_BODY";
		break;

	case BS_FREE_BALL_TMM_OPP:
		stream << "BS_FREE_BALL_TMM_OPP";
		break;

	case BS_FREE_BALL_BODY_TMM:
		stream << "BS_FREE_BALL_BODY_TMM";
		break;

	case BS_FREE_BALL_BODY_OPP:
		stream << "BS_FREE_BALL_BODY_OPP";
		break;

	}

	return stream;
}

float Point::getDistance(const Point &point) const
{
	return hypot(x - point.x, y - point.y);
}

float ValueModel::getWeight(float value)
{
	if (not use)
	    return 0.f;
	value = min(max(value, minValue), maxValue);
	value -= minValue;
	float valLength = maxValue - minValue;
	if (valLength == 0.0)
		return minWeight;
	float pos = value / valLength;
	return (pos * (maxWeight - minWeight)) + minWeight;
}
void ValueModel::set(float min_value, float max_value, float min_weight, float max_weight, bool use_it)
{
	minValue = min_value;
	maxValue = max_value;
	minWeight = min_weight;
	maxWeight = max_weight;
	use = use_it;
}

bool operator != (Point a, Point b)
{
	if(a.x == b.x && a.y == b.y)
		return false;
	return true;
}

bool operator == (Point a, Point b)
{
	if(a.x == b.x && a.y == b.y)
		return true;
	return false;
}

bool operator < (Point a, Point b)
{
	if(a.x < b.x)
		return true;
	else if(a.x == b.x)
		if (a.y < b.y)
			return true;
	return false;
}

bool operator == (FastestPlayer a, const Player& b)
{
	if (a.playerPtr->getTeamId() == b.getTeamId() and a.playerPtr->getUniNum() == b.getUniNum())
		return true;
	return false;
}

bool operator != (FastestPlayer a, const Player& b)
{
	return not (a == b);
}
