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

#ifndef __TIMER_H
#define __TIMER_H

#include <sys/time.h>
#include <iostream>

#ifndef NULL
#	define NULL 0
#endif // NULL

class Timer
{
protected:
	long cycle;

public:
	Timer();
	virtual ~Timer();

	virtual long now() const = 0;
	virtual long getCycle() const;
	virtual void resetCycle(long cycle);
};

class OnlineTimer: public Timer
{
protected:
	timeval beginning;

public:
	OnlineTimer();
	virtual ~OnlineTimer();

	virtual long now() const;
	virtual void resetCycle(long cycle);
};

class VirtualTimer: public Timer
{
protected:
	long curMS;

public:
	VirtualTimer();
	virtual ~VirtualTimer();

	void setCurMS(long curMSArg);

	virtual long now() const;
	virtual void resetCycle(long cycle);
};

#endif // __TIMER_H
