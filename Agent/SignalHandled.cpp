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
 *  This file is created by: Mohsen Izadi
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <SignalHandled.h>
#include <SignalHandler.h>


SignalHandled::~SignalHandled()
{
}

bool SignalHandled::sigAlrmHandler()
{
	return true;
}

bool SignalHandled::sigIntHandler()
{
	return false;
}

bool SignalHandled::sigIOHandler()
{
	return true;
}

bool SignalHandled::sigKillHandler()
{
	return false;
}

bool SignalHandled::sigQuitHandler()
{
	return false;
}

bool SignalHandled::sigSegVHandler()
{
	return false;
}

bool SignalHandled::sigTermHandler()
{
	return false;
}

bool SignalHandled::sigAbrtHandler()
{
	return false;
}

void SignalHandled::sigAlrmTiming(long &begin, long &interval)
{
	begin = interval = 10;
}

void SignalHandled::run()
{
	SignalHandler::initSignalHandlers(this);
	SignalHandler::run();
	SignalHandler::finalSignalHandlers();
}
