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
 *  This file is created by: Mohammad Amin Khashkhashi Moghaddam
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#ifndef __INTERCEPT_H
#define __INTERCEPT_H

#include <DecideCommonTypes.h>
#include <Ball.h>
#include <Body.h>

class Form;
class WorldModel;
class Command;

enum InterceptType {
	WAIT_INTERCEPT, //vay mistam ta toop biad :D
	FORWARD_DASH_INTERCEPT,
	BACK_DASH_INTERCEPT,
	TURN_INTERCEPT,
	SIDE_DASH_INTERCEPT, //using side dashes ( 6 angles)
	STOP_TURN_INTERCEPT,
	ADJUSTED_DASH_INTERCEPT
};

std::ostream &operator<<(std::ostream &stream, const InterceptType &entry);

struct InterceptEntry {
	unsigned reachCycle;
	float inKickable; // kickableArea - dist2ball
//	float threshold;
//	Point reachPos;
//	unsigned stamina;
	Command* command;
	InterceptType type;
//	int type;
	bool operator<(const InterceptEntry& e) const //use stamina for evaluating
			{
		if (reachCycle != e.reachCycle)
			return (reachCycle < e.reachCycle);
		if (fabs(inKickable - e.inKickable) > 0.1)
			return (inKickable > e.inKickable);
		return (type < e.type);
	}
};

class Intercept {
protected:
	const WorldModel *wm;
	Command *command;
	Body firstBody;
	Ball firstBall;
	Point interceptPoint;
	unsigned interceptTime;

public:
	Intercept(const WorldModel *wm);
	void execute(Form &form);
	void calculate();
	void getValue();
	Point getInterceptPoint() const;
	unsigned getInterceptTime() const;
	Command *getCommand();
};

#endif // __INTERCEPT_H
