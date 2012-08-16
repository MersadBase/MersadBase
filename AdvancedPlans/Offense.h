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
 *  	and is modified by: Mohammad Zareie 
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __OFFENSE_H
#define __OFFENSE_H

#include <Command.h>
#include <WorldModel.h>
#include <Formation.h>

class Offense
{
private:
	Command *command;
	const WorldModel *worldModel;

protected:

public:
	Offense(const WorldModel *worldModel);
	~Offense();
	
	void positioning();
	void decide(Form &form);
	Command *getCommand();
};

#endif // __OFFENSE_H
