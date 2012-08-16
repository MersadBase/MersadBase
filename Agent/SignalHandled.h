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

#ifndef __SIGNAL_HANDLED_H
#define __SIGNAL_HANDLED_H


#include <SignalsMask.h>


class SignalHandled
{
protected:
public:
	virtual ~SignalHandled();

	virtual bool sigAlrmHandler();
	virtual bool sigIntHandler();
	virtual bool sigIOHandler();
	virtual bool sigKillHandler();
	virtual bool sigQuitHandler();
	virtual bool sigSegVHandler();
	virtual bool sigTermHandler();
	virtual bool sigAbrtHandler();

	virtual SignalsMask signalsMask() = 0;
	virtual void sigAlrmTiming(long &begin, long &interval);

	virtual void run();
};


#endif // __SIGNAL_HANDLED_H

