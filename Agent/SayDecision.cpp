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

#include <cmath>
#include <string>
#include <cassert>

#include <WorldModel.h>
#include <Logger.h>
#include <Command.h>
#include <Defines.h>
#include <SayEncoding.h>
#include <SayDecision.h>

using namespace std;

// class SayDecision

SayDecision::SayDecision(const WorldModel *worldModel): worldModel(worldModel)
{
}

SayDecision::~SayDecision()
{
}

void SayDecision::decide(const Command *bodyCycleCommand,
		const SayDecisionForm &sayForm)
{
	LOG << "SayDecision::decide" << endl;

	unsigned attentionNum;

	if (sayForm.attentionNum)
		attentionNum = sayForm.attentionNum;
	else
	{
		float min = 0xFFFF;
		unsigned miner = NOVALUE;

		for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
			if (worldModel->getFullPlayer(TID_TEAMMATE, i).isValid() &&
				!worldModel->getFullPlayer(TID_TEAMMATE, i).isBody())
				if (miner == NOVALUE ||
					worldModel->getFullPlayer(TID_TEAMMATE, i).getDistance(
						worldModel->getBall()) < min)
				{
					miner = i;
					min = worldModel->getFullPlayer(TID_TEAMMATE, i).getDistance(
							worldModel->getBall());
				}

		if (miner == NOVALUE)
			attentionNum = 9;
		else
			attentionNum = miner + 1;
	}

	attentionCommand = new AttentionToCommand(TID_TEAMMATE, attentionNum);

	int permitLevel;

	if (sayForm.defenseSay)
		sayDefense(sayForm);
	if (sayForm.dfbSystem)
		sayDFB(sayForm);
	else if (sayForm.passSay)
	{
		unsigned playerNum = sayForm.passNum;
		if (playerNum > 11)
			playerNum -= 11;
		sayBeforePass(bodyCycleCommand, sayForm);
	}
	else if (sayForm.defenseSay)
		sayDefense(sayForm);
	else if (sayForm.radarSay)
		sayRadar(sayForm);
	else if (sayForm.freeKickSay)
		sayFreeKick(sayForm);
	else if (sayForm.beforePassSay)
	{
		unsigned playerNum = sayForm.interceptNum;
		if (playerNum > 11)
			playerNum -= 11;

			sayBeforePass(bodyCycleCommand, sayForm);
	}
	else if ((permitLevel = getSayRoutinBallPermission(sayForm)))
	{
		if (permitLevel == 1)
			sayRoutinBallWithoutVel(bodyCycleCommand, sayForm);
		else if (permitLevel == 2)
			sayRoutinBall(bodyCycleCommand, sayForm);
		else
			assert(0);
	}
	else
		sayCommand = new EmptyCommand();
}

void SayDecision::sayBeforePass(const Command *bodyCycleCommand, const SayDecisionForm &sayForm)
{
	string str = "A"; // BeforePass say flag

	Body nextBody(worldModel->getBody());
	nextBody.simulateByAction(bodyCycleCommand);
	nextBody.simulateByDynamics();

	Ball nextBall(worldModel->getBall());
	nextBall.simulateByAction(worldModel->getBody(), bodyCycleCommand);
	nextBall.simulateByDynamics(nextBody, 1);

	SayEncoding::encodeObjectPosition(nextBall.getPos().getX(), nextBall.getPos().getY(), str);
	SayEncoding::encodeObjectVelocity(nextBall.getVel().getMagnitude(), nextBall.getVel().getDirection(), str);

	str += SayEncoding::sayCodeKey[sayForm.passNum];

	sayCommand = new SayCommand(str);
}

void SayDecision::sayRoutinBall(const Command *bodyCycleCommand, const SayDecisionForm &sayForm)
{
	string str = "Z"; // RoutinBall say flag

	int sayingIntNum;
	if (sayForm.interceptNum)
		sayingIntNum = sayForm.interceptNum;
	else
		sayingIntNum = sayForm.gwInterceptNum;

	Body nextBody(worldModel->getBody());
	nextBody.simulateByAction(bodyCycleCommand);
	Body speedBody(nextBody); // I need this copy.
	nextBody.simulateByDynamics();

	Ball nextBall(worldModel->getBall());
	nextBall.simulateByAction(worldModel->getBody(), bodyCycleCommand);
	nextBall.simulateByDynamics(nextBody, 1);

	SayEncoding::encodeObjectPosition(nextBall.getPos().getX(),
			nextBall.getPos().getY(), str);

	if (worldModel->isBallKickable() && sayingIntNum != 0 &&
		worldModel->getBall().getPosDeviation() < 5)
	{
		SayEncoding::encodeObjectVelocity(
				speedBody.getVel().getMagnitude(),
				speedBody.getVel().getDirection(), str); // When I have the ball, it's velocity has no meaning.
		str += SayEncoding::sayCodeKey[sayingIntNum +
				SAY_UPPER_CASE_NUM]; // It means "Khow that I have the ball."

		str += SayEncoding::sayCodeKey[sayForm.planStatus];
		str += SayEncoding::sayCodeKey[0];
	}
	else
	{
		// When the ball is in the kickable of somebody, its velocity has no meaning.
		const Player *player = NULL;

		if (worldModel->isBallInTmmKickable())
		{
			player = &worldModel->getNearestTmmToBall();
			SayEncoding::encodeObjectVelocity(player->getVel().getMagnitude(),
					player->getVel().getDirection(), str);
		}
		else if (worldModel->isBallInOppKickable())
		{
			player = worldModel->getNearestOppToBall();
			SayEncoding::encodeObjectVelocity(player->getVel().getMagnitude(),
					player->getVel().getDirection(), str);
		}
		else
			SayEncoding::encodeObjectVelocity(nextBall.getVel().getMagnitude(),
					nextBall.getVel().getDirection(), str);

		str += SayEncoding::sayCodeKey[sayingIntNum];
		str += SayEncoding::sayCodeKey[sayForm.planStatus];

		if (player == NULL ||
			player->getModel() != PLM_FULL ||
			player->getPosDeviation() > 4)
			str += SayEncoding::sayCodeKey[0];
		else
		{
			if (player->getTeamId() == TID_TEAMMATE)
				str += SayEncoding::sayCodeKey[player->getUniNum()];
			else if (player->getTeamId() == TID_OPPONENT)
				str += SayEncoding::sayCodeKey[player->getUniNum() + 11];
			else
			{
				assert(0);
				str += SayEncoding::sayCodeKey[0];
			}
		}
	}

	sayCommand = new SayCommand(str);
}

void SayDecision::sayRoutinBallWithoutVel(const Command *bodyCycleCommand, const SayDecisionForm &sayForm)
{
	string str = "Y"; // RoutinBallWithoutVel say flag

	int sayingIntNum;
	if (sayForm.interceptNum)
		sayingIntNum = sayForm.interceptNum;
	else
		sayingIntNum = sayForm.gwInterceptNum;

	Body nextBody(worldModel->getBody());
	nextBody.simulateByAction(bodyCycleCommand);
	nextBody.simulateByDynamics();

	Ball nextBall(worldModel->getBall());
	nextBall.simulateByAction(worldModel->getBody(), bodyCycleCommand);
	nextBall.simulateByDynamics(nextBody, 1);

	SayEncoding::encodeObjectPosition(nextBall.getPos().getX(),
			nextBall.getPos().getY(), str);

	str += SayEncoding::sayCodeKey[sayingIntNum];
	str += SayEncoding::sayCodeKey[sayForm.planStatus];

	sayCommand = new SayCommand(str);
}

int SayDecision::getSayRoutinBallPermission(const SayDecisionForm &sayForm)
{
	if (worldModel->isBallKickable() &&
		 worldModel->getBall().getPosDeviation() < 5)
		return 2;

	if (sayForm.doRoutinSay && worldModel->getBall().getPosDeviation() < 5)
	{
		if (worldModel->getBall().getVelSeeTime() == worldModel->getCurTime())
			return 2;
		else
			return 1;
	}

	if (worldModel->getBall().getSeeTime() == worldModel->getCurTime() &&
		worldModel->getBall().getVelSeeTime() == worldModel->getCurTime())
		return 2;

	return 0;
}

void SayDecision::sayFreeKick(const SayDecisionForm &sayForm)
{
	string str = "F"; // freeKick say flag

	str += SayEncoding::sayCodeKey[sayForm.freeKickAdviceNumber];

	sayCommand = new SayCommand(str);
}

void SayDecision::sayRadar(const SayDecisionForm &sayForm)
{
	LOG << "SayDecision::sayRadar" << endl;

	string str = "R"; // Radar say flag

	for (int i = 0; i < sayForm.radarPlayersNum; i++)
	{
		LOG << "\tPlayer " << i << " ID: " << sayForm.ids[i]
			<< " Mag: " << sayForm.radarPlayers[i].getMagnitude()
			<< " Dir: " << sayForm.radarPlayers[i].getDirection() << endl;

		SayEncoding::encodeRadarObject(sayForm.ids[i],
			sayForm.radarPlayers[i].getMagnitude(),
			sayForm.radarPlayers[i].getDirection(),  str);
	}

	sayCommand = new SayCommand(str);
}

void SayDecision::sayDefense(const SayDecisionForm &sayForm)
{
	LOG << "SayDecision::sayDefense" << endl;

	string str = "D"; // Defense say flag

	str += sayForm.defenseStr;

	sayCommand = new SayCommand(str);
}

void SayDecision::sayDFB(const SayDecisionForm &sayForm)
{
	LOG << "SayDecision::sayDFB" << endl;

	string str = "X"; // DFB say flag

	str += (char(sayForm.dfbStatus) + '0');

	sayCommand = new SayCommand(str);
}

void SayDecision::sayPass(const SayDecisionForm &sayForm)
{
	LOG << "SayDecision::sayPass" << endl;

	string str = "P"; // Pass say flag

	SayEncoding::encodeObjectVelocity(sayForm.passSpeed, sayForm.passDir, str);
	SayEncoding::encodeWeight(/* Pass Weight */100, -50, 150, str);
	str += SayEncoding::sayCodeKey[sayForm.passNum];

	sayCommand = new SayCommand(str);
}

Command *SayDecision::getSayCommand()
{
	return sayCommand;
}

Command *SayDecision::getAttentionCommand()
{
	return attentionCommand;
}
