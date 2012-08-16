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

#ifndef __TYPES_H
#define __TYPES_H

#ifndef NULL
#	define NULL 0
#endif // NULL

// در این کلاس انواع enum ها و Type های مختلف قرار دارند.

#include <iostream>

class Player;

// structures

enum CardModel
{
	CM_NONE = 0,
	CM_YELLOW = 1,
	CM_RED = 2
};

enum PlayerPowerModel
{
	APowerModel = 0,
	BPowerModel = 1,
	CPowerModel = 2
};

struct ValueModel
{
	float minValue;
	float maxValue;
	float minWeight;
	float maxWeight;

	bool use;

	float getWeight(float value);
	void set(float min_value, float max_value, float min_weight, float max_weight, bool use_it = true);
};

struct Point
{
	float x;
	float y;

	Point(float x = 0.0, float y = 0.0): x(x), y(y) {}
	float getDistance(const Point &point) const;
	float getX() const { return x; };
	float getY() const { return y; };

	Point& operator+=(const Point point);
	Point& operator-=(const Point point);
};

std::ostream &operator<<(std::ostream &stream, const Point &point);

// enumerators

// این enum مشخص کننده ی تیم یک بازیکن خواهد بود. TID_TEAMMATE مشخص کننده ی یک بازیکن هم تیمی و TID_OPPONENT مشخص کننده ی یک بازیکن حریف و TID_UNKNOWN بیان گر این است که مشخص نیست آن بازیکن برای کدام تیم است.
enum TeamId
{
	TID_TEAMMATE = 0, TID_OPPONENT = 1, TID_UNKNOWN
};

enum ViewModeQuality
{
	VMQ_LOW, VMQ_HIGH
};

enum ViewModeWidth
{
	VMW_NARROW, VMW_NORMAL, VMW_WIDE
};

// مشخص کننده ی این است که یک تیم در سمت چپ زمین بازی می کند یا راست.
enum Side
{
	SI_LEFT, SI_RIGHT
};

// انواع حالت های مختلفی که در طول یک بازی پیش می‌ آید در دو enum زیر قرار دارند.
enum PlayMode
{
	PM_TIME_UP_WITHOUT_A_TEAM,
	PM_TIME_UP,
	PM_BEFORE_KICK_OFF,
	PM_PLAY_ON,
	PM_KICK_OFF,
	PM_KICK_OFF_OPP,
	PM_FREE_KICK,
	PM_FREE_KICK_OPP,
	PM_KICK_IN,
	PM_KICK_IN_OPP,
	PM_OFFSIDE,
	PM_OFFSIDE_OPP,
	PM_GOAL,
	PM_GOAL_OPP,
	PM_GOAL_KICK,
	PM_GOAL_KICK_OPP,
	PM_CORNER_KICK,
	PM_CORNER_KICK_OPP,
	PM_FREE_KICK_FAULT,
	PM_FREE_KICK_FAULT_OPP,
	PM_CATCH_FAULT,
	PM_CATCH_FAULT_OPP,
	PM_BACK_PASS,
	PM_BACK_PASS_OPP,
	PM_FOUL_CHARGE,
	PM_FOUL_CHARGE_OPP,

	PM_PENALTY_SETUP,
	PM_PENALTY_SETUP_OPP,
	PM_PENALTY_READY,
	PM_PENALTY_READY_OPP,
};

enum VirtualPlayMode
{
	VPM_NONE,
	VPM_DROP_BALL,
	VPM_HALF_TIME,
	VPM_TIME_EXTENDED,
	VPM_PENALTY_DRAW,
	VPM_GOALIE_CATCH_BALL,
	VPM_GOALIE_CATCH_BALL_OPP,
	VPM_GOAL_KICK_TAKEN,
	VPM_FOUL,
	VPM_FOUL_OPP,
	VPM_PENALTY_ON_FIELD,
	VPM_PENALTY_ON_FIELD_OPP,
	VPM_PENALTY_MISS,
	VPM_PENALTY_MISS_OPP,
	VPM_PENALTY_SCORE,
	VPM_PENALTY_SCORE_OPP,
	VPM_PENALTY_WINNER,
	VPM_PENALTY_WINNER_OPP,
	VPM_PENALTY_FOUL,
	VPM_PENALTY_FOUL_OPP,
	VPM_PENALTY_TAKEN,
	VPM_PENALTY_TAKEN_OPP,
	VPM_INDIRECT_FREE_KICK,
	VPM_INDIRECT_FREE_KICK_OPP
};

enum BallStatus
{
	BS_NONE,
	BS_KICKABLE_TMM,
	BS_KICKABLE_OPP,
	BS_KICKABLE_BODY,
	BS_KICKABLE_TMM_OPP,
	BS_KICKABLE_BODY_TMM,
	BS_KICKABLE_BODY_OPP,
	BS_FREE_BALL,
	BS_FREE_BALL_TMM,
	BS_FREE_BALL_OPP,
	BS_FREE_BALL_BODY,
	BS_FREE_BALL_TMM_OPP,
	BS_FREE_BALL_BODY_TMM,
	BS_FREE_BALL_BODY_OPP
};

enum TriAnswer
{
	TA_YES,
	TA_NO,
	TA_DONT_KNOW
};

std::ostream &operator<<(std::ostream &stream, const BallStatus &var);
std::ostream &operator<<(std::ostream &stream, const PlayMode &var);
std::ostream &operator<<(std::ostream &stream, const TeamId &var);
std::ostream &operator<<(std::ostream &stream, const Player &var);

bool operator != (Point a, Point b);
bool operator == (Point a, Point b);
bool operator < (Point a, Point b);

class FastestPlayer;
class Player;

bool operator == (FastestPlayer a, const Player& b);
bool operator != (FastestPlayer a, const Player& b);

#endif // __TYPES_H
