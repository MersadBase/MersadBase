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
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __CONNECTION_H
#define __CONNECTION_H

#include <string>
#include <Command.h>


#ifndef NULL
#	define NULL 0
#endif // NULL

#define MAX_MSG 4096


class Connection
{
protected:
	virtual bool init() = 0;

public:
	virtual ~Connection();

	virtual void send(Command *command);
	virtual void send(const std::string &message) = 0;
	virtual int receive(std::string &message) = 0;
};

#endif // __CONNECTION_H
