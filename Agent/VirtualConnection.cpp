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

#include <VirtualConnection.h>

using namespace std;

VirtualConnection::VirtualConnection()
{
}

VirtualConnection::~VirtualConnection()
{
}

bool VirtualConnection::init()
{
	return true;
}

void VirtualConnection::send(const string &message)
{
}

int VirtualConnection::receive(string &message)
{
	if (messages.size() == 0)
		return 0;

	message = messages.front();
	messages.pop_front();
	return 1;
}

void VirtualConnection::addMessage(const string &message)
{
	messages.push_back(message);
}
