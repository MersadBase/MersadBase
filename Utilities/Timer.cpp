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

#include <Timer.h>
#include <iostream>

using namespace std;


// class Timer

Timer::Timer(): cycle(-1)
{
	// resetCycle(); // logically, should be here!
}

Timer::~Timer()
{
}

void Timer::resetCycle(long cycle)
{
	this->cycle = cycle;
}

long Timer::getCycle() const
{
	return cycle;
}


// class OnlineTimer

OnlineTimer::OnlineTimer()
{
	resetCycle(-1);
}

OnlineTimer::~OnlineTimer()
{
}

void OnlineTimer::resetCycle(long cycle)
{
	Timer::resetCycle(cycle);
	gettimeofday(&beginning, NULL);
}

long OnlineTimer::now() const
{
	timeval now;
	gettimeofday(&now, NULL);
	return (long) (((now.tv_usec - beginning.tv_usec) / 1000.0 +
		(now.tv_sec - beginning.tv_sec) * 1000) + 0.5);
}


// class VirtualTimer

VirtualTimer::VirtualTimer()
{
}

VirtualTimer::~VirtualTimer()
{
}

void VirtualTimer::resetCycle(long cycle)
{
	Timer::resetCycle(cycle);
	curMS = 0;
}

long VirtualTimer::now() const
{
	return curMS;
}

void VirtualTimer::setCurMS(long curMSArg)
{
	curMS = curMSArg;
}

