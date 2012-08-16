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
 *  This file is created by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Tackle.h>
#include <Intercept.h>

#include <cmath>
#include <iostream>

#include <WorldModel.h>
#include <Logger.h>
#include <Ball.h>
#include <Degree.h>
#include <Command.h>
#include <BasicKick.h>
#include <BasicDash.h>
#include <BasicTurn.h>
#include <BasicTackle.h>

#include <Rectangle.h>

using namespace std;
using namespace Degree;

Tackle::Tackle(const WorldModel *wm):worldModel(wm)
{
	blockTackleMinProb = 0.8;
	Point top_left(-53, 10),
			bottom_right(-36, -10);
	Rectangle DangerField(top_left, bottom_right);
	if (DangerField.isInRectangle(worldModel->getBall().getPos()))
		blockTackleMinProb = 0.7;
}

Tackle::~Tackle()
{
}

bool Tackle::execute(Form &form)
{
	float tackleProb = worldModel->getTackleProb();
	LOG << "tackle Probability = " << tackleProb << endl;

	if (worldModel->getGlobalFastIC().isOurTeamBallPossessor() or worldModel->isBallInTmmKickable() or worldModel->isBallKickable())
		return false;

	Ball finalBall = worldModel->getBall();
	for (int i = 0; i < 30; i++)
		finalBall.simulateByDynamics(worldModel->getBody());
	Line ballRoute(worldModel->getBall().getPos(), finalBall.getPos());

	bool oppPossessor = false;
	if ((!worldModel->getGlobalFastIC().getFastestPlayers().size()) or worldModel->getGlobalFastIC().getFastestPlayers()[0].receivePoint.x < -52.f)
		oppPossessor = true;
	LOG << "finalBall : " << finalBall.getPos() << endl;
	LOG << "Ball Route = " << fabs(ballRoute.getY(-52.5)) << endl;

	if (tackleProb > 0.01 and fabs(ballRoute.getY(-52.5)) < 8.f and oppPossessor)
	{
		LOG << "Goaltackle value is true, tackling" << endl;
		float deg = 80;
		if (ballRoute.getY(-52.5) < 0)
			deg = -80;
		LOG << "ballRoute.getY(-52.5) : " << ballRoute.getY(-52.5) << endl;
		LOG << "tackle direction : " << deg << endl;
		command = TackleToDir(worldModel, deg, worldModel->getBody()).getCommand();
		return true;
	}

	LOG << "tackle value is false" << endl;
	return false;
}

float Tackle::getBlockTackleMinProb()
{
	return blockTackleMinProb;
}

Command* Tackle::getCommand()
{
	return command;
}

