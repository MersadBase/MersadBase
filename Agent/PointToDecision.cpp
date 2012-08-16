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
 *  	and is modified by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Degree.h>
#include <Logger.h>
#include <Basics.h>
#include <Command.h>
#include <PointToDecision.h>

using namespace std;

// class PointToDecision

PointToDecision::PointToDecision(const WorldModel *worldModel):
		worldModel(worldModel)
{
}

PointToDecision::~PointToDecision()
{
}

void PointToDecision::decide(const Command *headCycleCommand)
{
	LOG << "PointToDecision::decide" << endl;

	float maxSenseStamina;
	if (worldModel->getPlayMode() == PM_PLAY_ON)
		maxSenseStamina = 6000;
	else
		maxSenseStamina = 7000;

	if (worldModel->getBody().getArmMovable() != 0)
	{
		pointToCommand = new EmptyCommand();
		return;
	}

	if (worldModel->getBody().getStamina() > maxSenseStamina)
	{
		if (worldModel->getBody().getArmExpires())
			pointToCommand = new PointToCommand(false);
		else
			pointToCommand = new EmptyCommand();
	}
	else
	{
		float dist = 1000;
		float dir = Basics::reRate(worldModel->getBody().getStamina(),
				2400, maxSenseStamina, 0, 270, RRM_DIRECT);

		dir = Degree::getDeltaAngle(dir, worldModel->getBody().getHeadDir());
		pointToCommand = new PointToCommand(dist, dir);
	}
}

Command *PointToDecision::getPointToCommand()
{
	return pointToCommand;
}

