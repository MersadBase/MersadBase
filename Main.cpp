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
 *  	and is modified by: Ahmad Boorghany, Mohammad Salehe
 *
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <iostream>
#include <cstring>

#include <AdvancedAgent.h>
#include <AdvancedCoach.h>

int main(int argc, char **argv)
{
	int i = 1;
	while (++i <= argc)
		if (!strcmp(argv[i - 1],"--coach"))
		{
			AdvancedCoach *coachPtr = new AdvancedCoach(argc, argv);
			coachPtr->run();

			delete coachPtr;
			return 0;
		}

	AdvancedAgent *playerPtr = new AdvancedAgent(argc, argv);
	playerPtr->run();

	delete playerPtr;
	return 0;
}
