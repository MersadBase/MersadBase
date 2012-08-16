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

#include <SignalHandler.h>
#include <SignalsMask.h>
#include <csignal>
#include <sys/time.h>

#include <iostream>

using namespace std;

#ifndef NULL
#	define NULL 0
#endif // NULL

SignalHandled *SignalHandler::sh = NULL;
bool SignalHandler::finished;

void SignalHandler::sigAlrmHandler(int/* signum*/)
{
	if (sh) finished = !sh->sigAlrmHandler();
}

void SignalHandler::sigIntHandler(int/* signum*/)
{
	if (sh) finished = !sh->sigIntHandler();
}

void SignalHandler::sigIOHandler(int/* signum*/)
{
	if (sh) finished = !sh->sigIOHandler();
}

void SignalHandler::sigKillHandler(int/* signum*/)
{
	if (sh) finished = !sh->sigKillHandler();
}

void SignalHandler::sigQuitHandler(int/* signum*/)
{
	if (sh) finished = !sh->sigQuitHandler();
}

void SignalHandler::sigSegVHandler(int/* signum*/)
{
	if (sh) finished = !sh->sigSegVHandler();
}

void SignalHandler::sigTermHandler(int/* signum*/)
{
	if (sh) finished = !sh->sigTermHandler();
}

void SignalHandler::sigAbrtHandler(int/* signum*/)
{
	if (sh) finished = !sh->sigAbrtHandler();
}

void SignalHandler::initSignalHandlers(SignalHandled *sh)
{
	if (!sh)
		return;

	SignalHandler::sh = sh;

	struct sigaction sa;

	SignalsMask mask = sh->signalsMask();

	if (mask & SIG_ALRM)
	{
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGALRM);
		sigaddset(&sa.sa_mask, SIGIO);
		sa.sa_handler = sigAlrmHandler;
		sigaction(SIGALRM, &sa, NULL);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);

		long begin, interval;
		sh->sigAlrmTiming(begin, interval);
		itimerval itv;
		itv.it_value.tv_sec = begin / 1000;
		itv.it_value.tv_usec = (begin % 1000) * 1000;
		itv.it_interval.tv_sec = interval / 1000;
		itv.it_interval.tv_usec = (interval % 1000) * 1000;
		setitimer(ITIMER_REAL, &itv, NULL);
	}

	if (mask & SIG_INT)
	{
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGINT);
		sa.sa_handler = sigIntHandler;
		sigaction(SIGINT, &sa, NULL);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
	}

	if (mask & SIG_IO)
	{
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGIO);
		sigaddset(&sa.sa_mask, SIGALRM);
		sa.sa_handler = sigIOHandler;
		sigaction(SIGIO, &sa, NULL);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
	}

	if (mask & SIG_KILL)
	{
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGKILL);
		sa.sa_handler = sigKillHandler;
		sigaction(SIGKILL, &sa, NULL);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
	}

	if (mask & SIG_QUIT)
	{
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGQUIT);
		sa.sa_handler = sigQuitHandler;
		sigaction(SIGQUIT, &sa, NULL);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
	}

	if (mask & SIG_SEGV)
	{
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGSEGV);
		sa.sa_handler = sigSegVHandler;
		sigaction(SIGSEGV, &sa, NULL);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
	}

	if (mask & SIG_TERM)
	{
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGTERM);
		sa.sa_handler = sigTermHandler;
		sigaction(SIGTERM, &sa, NULL);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
	}

	if (mask & SIG_ABRT)
	{
		sa.sa_flags = 0;
		sigemptyset(&sa.sa_mask);
		sigaddset(&sa.sa_mask, SIGABRT);
		sa.sa_handler = sigAbrtHandler;
		sigaction(SIGABRT, &sa, NULL);
		sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);
	}

	finished = false;
}

void SignalHandler::finalSignalHandlers()
{
	signal(SIGALRM, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGIO, SIG_DFL);
	signal(SIGKILL, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGABRT, SIG_DFL);
}

void SignalHandler::run()
{
	sigset_t ss;
	sigprocmask(SIG_BLOCK, NULL, &ss);
	while (!finished)
		sigsuspend(&ss);
}

