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
 *  This file is created by: Pooria Kaviani
 *  	and is modified by: Mohammad Zareie, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#ifndef __NON_PLAY_ON_PLAN_H
#define __NON_PLAY_ON_PLAN_H

#include <Defines.h>
#include <Formation.h>

class NonPlayOnDecision
{
private:
	Command *command;
	const WorldModel *wm;

protected:
	bool areAllPlayersReady();
	bool isStaminaEnough();
	bool isAllPlayersReady();
	Vector positions[11];

public:
	NonPlayOnDecision(const WorldModel *wm);
	~NonPlayOnDecision();

	void decide(Form &form);
	void usualKick(Form &form, int forceKicker = -1);
	void usualPos(Form &form);
	Command *getCommand();
};

#endif // __NON_PLAY_ON_PLAN_H
