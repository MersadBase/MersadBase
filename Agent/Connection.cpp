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

#include <Connection.h>
#include <iostream>

using namespace std;

Connection::~Connection()
{
}

void Connection::send(const string &message)
{
	send(message.c_str());
}

void Connection::send(Command *command)
{
	send(command->toString());
}
