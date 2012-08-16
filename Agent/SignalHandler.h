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

#ifndef __SIGNAL_HANDLER_H
#define __SIGNAL_HANDLER_H

#include <SignalHandled.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

class SignalHandler
{
private:
	static SignalHandled *sh;
	static bool finished;

	static void sigAlrmHandler(int/* signum*/);
	static void sigIntHandler(int/* signum*/);
	static void sigIOHandler(int/*signum*/);
	static void sigKillHandler(int/* signum*/);
	static void sigQuitHandler(int/* signum*/);
	static void sigSegVHandler(int/* signum*/);
	static void sigTermHandler(int/* signum*/);
	static void sigAbrtHandler(int/* signum*/);


public:
	static void run();
	static void finalSignalHandlers();
	static void initSignalHandlers(SignalHandled *sh);

};

#endif // __SIGNAL_HANDLER_H
