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
 *  	and is modified by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __POINT_TO_DECISION_H
#define __POINT_TO_DECISION_H


#include <WorldModel.h>


class Command;

class PointToDecision
{
	const WorldModel *worldModel;
	Command *pointToCommand;

public:
	PointToDecision(const WorldModel *worldModel);
	~PointToDecision();

	void decide(const Command *headCycleCommand);

	Command *getPointToCommand();
};

#endif // __POINT_TO_DECISION_H

