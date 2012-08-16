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
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __GOALIE_H_
#define __GOALIE_H_

#include <WorldModel.h>
#include <Command.h>

class Goalie
{
private:
	float goalieLine;
	float goalieY;

	Command* command;
	const WorldModel* worldModel;

	static bool backPass;
	static unsigned lastTeammateKick;

public:
	Goalie(const WorldModel* worldModel);

	Command* getCommand();
	void decide(Form& form);

	bool GoalieIntercept(Form& form);
	bool CatchDecision();

	float getGoalieLine();
	float getGoalieY();

};

#endif // __GOALIE_H_
