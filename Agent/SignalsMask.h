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

#ifndef __SIGNALS_MASK_H
#define __SIGNALS_MASK_H

#ifndef NULL
#	define NULL 0
#endif // NULL

#define SIG_ALRM 1
#define SIG_INT  2
#define SIG_IO   4
#define SIG_KILL 8
#define SIG_QUIT 16
#define SIG_SEGV 32
#define SIG_TERM 64
#define SIG_ABRT 128


typedef int SignalsMask;

#endif // __SIGNALS_MASK_H
