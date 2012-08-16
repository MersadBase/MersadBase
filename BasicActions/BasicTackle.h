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
 *  This file is created by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __BASIC_TACKLE_H
#define __BASIC_TACKLE_H

#include <BasicAction.h>
#include <WorldModel.h>
#include <Types.h>
#include <Config.h>
#include <Vector.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

class TackleToDir: public BasicAction
{
protected:
	Command* command;
	bool possible;
	Vector velocity;
	float effPower;
public:
	TackleToDir(const WorldModel *worldModel, float direction,const Body& body, bool foul = false);
	Vector getVelocity();
	float getEffectivePower();
	bool isPossible();
	Command* getCommand();
};

class TackleToPoint: public BasicAction
{
protected:
	Command* command;
	bool possible;
	Vector velocity;
	float effPower;
public:
	TackleToPoint(const WorldModel *worldModel, Point target,const Body& body, bool foul = false);
	Vector getVelocity();
	float getEffectivePower();
	bool isPossible();
	Command* getCommand();
};

#endif// __BASIC_TACKLE_H
