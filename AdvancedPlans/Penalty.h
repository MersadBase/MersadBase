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
 *  Released on Sunday 9 July 2010, 17 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __PENALTY_H
#define __PENALTY_H

#include <WorldModel.h>
#include <DecideCommonTypes.h>
#include <vector>
#include <Command.h>
#include <WorldModel.h>
#include <Formation.h>

class Penalty
{

public:
	Penalty(const WorldModel* _worldModel):
			worldModel(_worldModel)
		{
		};
	
	void execute(Form& form);
	Command* getCommand();

private:
	Command * command;
	const WorldModel* worldModel;

	// decide for our kicker
	void doKicker(Form& form);

	// decide for goalie
	void obtainBall(Form& form);

	Point getPos(unsigned num);
};

#endif //__PENALTY_H
