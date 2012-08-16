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
 *  	and is modified by: Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Monday 13 September 2010, 22 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <Logger.h>
#include <Basics.h>
#include <Degree.h>
#include <Defines.h>
#include <WorldModelUtilities.h>

#define OPP_OFFLINE_LOCAL_X 20
#define OPP_OFFLINE_LOCAL_Y 10

using namespace std;
using namespace Basics;
using namespace Degree;

Formation WorldModelUtilities::formation("");
Formation WorldModelUtilities::beforeKickOffFormation("");
Formation WorldModelUtilities::nonPlayOnFormation("");
bool WorldModelUtilities::formationSet(false);

int WorldModelUtilities::checkPlayModeForPenalty() const
{
	if (virtualPlayMode == VPM_PENALTY_ON_FIELD)
		return 1;// Rotate Field Flags
	if (virtualPlayMode == VPM_PENALTY_ON_FIELD_OPP)
		return 2;// Rotate Field Flags ONLY for goalie

	return 0;
}

float WorldModelUtilities::getTackleProb(bool foul) const
{
	float exponent = getServerParam()["tackle_exponent"].asFloat();
	if (foul)
		exponent = getServerParam()["foul_exponent"].asFloat();

	float succTackleProb = 0, failTackleProb = 1;
	Vector playerAndBall = getBall().getPos() - getBody().getPos();
	playerAndBall.rotate(-getBody().getBodyDir());
	float tackleDist;
	if (playerAndBall.getX() > 0)
		tackleDist = getServerParam()["tackle_dist"].asFloat();
	else
		tackleDist = getServerParam()["tackle_back_dist"].asFloat();

	if (tackleDist > 1.0e-5)
		failTackleProb = pow(abs(playerAndBall.getX()) / tackleDist,
		exponent) +
		pow(abs(playerAndBall.getY()) /
		getServerParam()["tackle_width"].asFloat(),
		exponent);

	if ( failTackleProb < 1 )
		succTackleProb = 1 - failTackleProb;
	return succTackleProb;
}

float WorldModelUtilities::getTackleProb(const Ball &theBall, const Body &body, bool foul) const
{
	float exponent = getServerParam()["tackle_exponent"].asFloat();
	if (foul)
		exponent = getServerParam()["foul_exponent"].asFloat();

	float succTackleProb = 0, failTackleProb = 1;
	Vector playerAndBall = theBall.getPos() - body.getPos();
	playerAndBall.rotate(-body.getBodyDir());
	float tackleDist;
	if (playerAndBall.getX() > 0)
		tackleDist = getServerParam()["tackle_dist"].asFloat();
	else
		tackleDist = getServerParam()["tackle_back_dist"].asFloat();

	if (tackleDist > 1.0e-5)
		failTackleProb = pow(abs(playerAndBall.getX()) / tackleDist,
		exponent) +
		pow(abs(playerAndBall.getY()) /
		getServerParam()["tackle_width"].asFloat(),
		exponent);

	if ( failTackleProb < 1 )
		succTackleProb = 1 - failTackleProb;
	return succTackleProb;
}

float WorldModelUtilities::getTackleProb(const Ball &theBall, const Player *player, bool foul) const
{
	float exponent = getServerParam()["tackle_exponent"].asFloat();
	if (foul)
		exponent = getServerParam()["foul_exponent"].asFloat();

	float succTackleProb = 0, failTackleProb = 1;
	Vector playerAndBall = theBall.getPos() - player->getPos();
	playerAndBall.rotate(-player->getBodyDir());
	float tackleDist;
	if (playerAndBall.getX() > 0)
		tackleDist = getServerParam()["tackle_dist"].asFloat();
	else
		tackleDist = getServerParam()["tackle_back_dist"].asFloat();

	if (tackleDist > 1.0e-5)
		failTackleProb = pow(abs(playerAndBall.getX()) / tackleDist,
		exponent) +
		pow(abs(playerAndBall.getY()) /
		getServerParam()["tackle_width"].asFloat(),
		exponent);

	if ( failTackleProb < 1 )
		succTackleProb = 1 - failTackleProb;
	return succTackleProb;
}

bool WorldModelUtilities::isBallKickable() const
{
	float kickableArea;
	kickableArea = getBall().getSize() + getBody().getKickableMargin() + getBody().getSize() - 0.055;

	if (getBall().getAbsVec().getMagnitude() <= kickableArea)
		return true;
	return false;
}

bool WorldModelUtilities::isBallKickable(const Ball &theBall) const
{
	float kickableArea;
	kickableArea = getBall().getSize() + getBody().getKickableMargin() +
		getBody().getSize() - 0.055;

	if (theBall.getAbsVec().getMagnitude() <= kickableArea)
		return true;
	return false;
}

bool WorldModelUtilities::isBallCatchable() const
{
	float catchableArea = std::sqrt( std::pow( serverParam["catchable_area_w"].asFloat() * 0.5, 2.0 ) + std::pow( serverParam["catchable_area_l"].asFloat(), 2.0 ) );

	if (getBall().getAbsVec().getMagnitude() < catchableArea)
		return true;
	return false;
}

bool WorldModelUtilities::isBallCatchable(const Ball &theBall) const
{
	float catchableArea = std::sqrt( std::pow( serverParam["catchable_area_w"].asFloat() * 0.5, 2.0 ) + std::pow( serverParam["catchable_area_l"].asFloat(), 2.0 ) );

	if (theBall.getAbsVec().getMagnitude() < catchableArea)
		return true;
	return false;
}

float WorldModelUtilities::getCatchProbability() const
{
	float catchableArea = std::sqrt( std::pow( serverParam["catchable_area_w"].asFloat() * 0.5, 2.0 ) + std::pow( serverParam["catchable_area_l"].asFloat(), 2.0 ) );
	float catch_prob = serverParam["catch_probability"].asFloat();
	float ballDist = getBall().getPos().getDistance(getBody().getPos());
	float stretch_l = serverParam["catchable_area_l"].asFloat() * getPlayerType(getBody().getUniNum())["catchable_area_l_stretch"].asFloat();
	float stretch_area = std::sqrt( std::pow( serverParam["catchable_area_w"].asFloat() * 0.5, 2 )
                                                   + std::pow( stretch_l, 2 ) );

	LOG << "my stretch_area : " << stretch_area << endl;

	if ( ballDist > stretch_area )
	{
		return 0.0;
	}

	if ( ballDist > catchableArea )
	{
		float x = ballDist * ( stretch_l / stretch_area );
		catch_prob = serverParam["catch_probability"].asFloat()
				- serverParam["catch_probability"].asFloat() * ( ( x - serverParam["catchable_area_l"].asFloat() )
				/ ( stretch_l - serverParam["catchable_area_l"].asFloat() ) );
	}
	return catch_prob;
}

bool WorldModelUtilities::isBallInTmmKickable() const
{
	register int i;
	float kickableArea;
	kickableArea = getBall().getSize() +
		getBody().getSize();

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_TEAMMATE, i).isValid())
			if(!getFullPlayer(TID_TEAMMATE, i).isBody())
				if(getFullPlayer(TID_TEAMMATE, i).getDistance(getBall())
				   <=	kickableArea + getFullPlayer(TID_TEAMMATE, i).getKickableMargin())
					return true;

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_TEAMMATE, i).isValid())
			if(!getHalfPlayer(TID_TEAMMATE, i).isBody())
				if(getHalfPlayer(TID_TEAMMATE, i).getDistance(getBall())
				   <=	kickableArea + getHalfPlayer(TID_TEAMMATE, i).getKickableMargin())
					return true;

	return false;
}

bool WorldModelUtilities::isBallInTmmKickable(const Ball &theBall) const
{
	register int i;
	float kickableArea;
	kickableArea = getBall().getSize() +
		getBody().getSize();

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_TEAMMATE, i).isValid())
			if(!getFullPlayer(TID_TEAMMATE, i).isBody())
				if(getFullPlayer(TID_TEAMMATE, i).getDistance(theBall)
				   <=	kickableArea + getFullPlayer(TID_TEAMMATE, i).getKickableMargin())
					return true;

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_TEAMMATE, i).isValid())
			if(!getHalfPlayer(TID_TEAMMATE, i).isBody())
				if(getHalfPlayer(TID_TEAMMATE, i).getDistance(theBall)
				   <=	kickableArea + getHalfPlayer(TID_TEAMMATE, i).getKickableMargin())
					return true;

	return false;
}

bool WorldModelUtilities::isBallInOppKickable() const
{
	register int i;
	float kickableArea;
	kickableArea = getBall().getSize() +
		getBody().getSize();

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_OPPONENT, i).isValid())
			if(getFullPlayer(TID_OPPONENT, i).getDistance(getBall())
			   <=	kickableArea + getFullPlayer(TID_OPPONENT, i).getKickableMargin())
				return true;

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_OPPONENT,i).isValid())
			if(getHalfPlayer(TID_OPPONENT,i).getDistance(getBall())
			   <=	kickableArea + getHalfPlayer(TID_OPPONENT, i).getKickableMargin())
				return true;

	return false;
}

bool WorldModelUtilities::isBallInOppKickable(const Ball &theBall) const
{
	register int i;
	float kickableArea;
	kickableArea = getBall().getSize() +
		getBody().getSize();

	for (i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_OPPONENT, i).isValid())
			if(getFullPlayer(TID_OPPONENT, i).getDistance(theBall)
			   <=	kickableArea + getFullPlayer(TID_OPPONENT, i).getKickableMargin())
				return true;

	for (i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_OPPONENT,i).isValid())
			if(getHalfPlayer(TID_OPPONENT,i).getDistance(theBall)
			   <=	kickableArea + getHalfPlayer(TID_OPPONENT, i).getKickableMargin())
				return true;

	return false;
}

bool WorldModelUtilities::isBallInField() const
{
	if (getBall().getPos().getX() < 52.5 + 0.5 &&
		getBall().getPos().getX() > -52.5 - 0.5 &&
		getBall().getPos().getY() < 34 + 0.5 &&
		getBall().getPos().getY() > -34 - 0.5)
		return true;
	return false;
}

bool WorldModelUtilities::isBallInField(const Ball &theBall)
{
	if (theBall.getPos().getX() < 52.5 + 0.5 &&
		theBall.getPos().getX() > -52.5 - 0.5 &&
		theBall.getPos().getY() < 34 + 0.5 &&
		theBall.getPos().getY() > -34 - 0.5)
		return true;
	return false;
}

float WorldModelUtilities::getOutDistance(const Point &point)
{
	float xDistance = 0;
	float yDistance = 0;

	if (point.x < -52.5)
		xDistance = -point.x - 52.5;
	else if (point.x > 52.5)
		xDistance = +point.x - 52.5;

	if (point.y < -34)
		yDistance = -point.y - 34;
	else if (point.y > 34)
		yDistance = +point.y - 34;

	return hypot(xDistance, yDistance);
}

bool WorldModelUtilities::isOppBallShooted(const Ball &theBall) const
{
	Point interceptGoalLine, interceptGoal;

	if (theBall.getVel().getX() == 0)
		return 0;

	float delatX = -52.5 - theBall.getPos().getX();
	float delatY = (theBall.getVel().getY() * delatX) / theBall.getVel().getX();

	float delatX2 = -52.5 - theBall.getPos().getX();
	float delatY2 = (theBall.getVel().getY() * delatX) / theBall.getVel().getX();

	interceptGoalLine.x = theBall.getPos().getX() + delatX;
	interceptGoalLine.y = theBall.getPos().getY() + delatY;

	interceptGoal.x = theBall.getPos().getX() + delatX2;
	interceptGoal.y = theBall.getPos().getY() + delatY2;

	if (abs(interceptGoal.y) > 8.5)
		return 0;

	Ball simBall;
	simBall = theBall;

	while (simBall.getVel().getMagnitude() > .4)
		simBall.simulateByDynamics(getBody());

	float standardX = -51;

	if (theBall.getPos().getX() < standardX)
		standardX = -52.5;

	if (simBall.getPos().getX() < standardX)
		return true;
	return false;
}

bool WorldModelUtilities::isOppBallShootedToTir(const Ball &theBall, float upYTirPoint,
									   float downYTirPoint, Point &shootIntercept) const
{
	Point interceptGoalLine, interceptGoal;

	if (theBall.getVel().getX() == 0)
		return 0;

	float deltaX = -52.5 - theBall.getPos().getX();
	float deltaY = (theBall.getVel().getY() * deltaX) / theBall.getVel().getX();

	float deltaX2 = -52.5 - theBall.getPos().getX();
	float deltaY2 = (theBall.getVel().getY() * deltaX) / theBall.getVel().getX();

	interceptGoalLine.x = theBall.getPos().getX() + deltaX;
	interceptGoalLine.y = theBall.getPos().getY() + deltaY;

	interceptGoal.x = theBall.getPos().getX() + deltaX2;
	interceptGoal.y = theBall.getPos().getY() + deltaY2;


	if (interceptGoal.y < (downYTirPoint - 1) || interceptGoal.y > (upYTirPoint + 1))
		return 0;

	Ball simBall;
	simBall = theBall;

	while(simBall.getVel().getMagnitude() > .4)
		simBall.simulateByDynamics(getBody());

	float standardX = -51.5;

	if (theBall.getPos().getX() < standardX)
		standardX = -52.5;

	if (simBall.getPos().getX() < standardX)
	{
		shootIntercept = interceptGoal;
		return 1;
	}
	return 0;
}

const Player &WorldModelUtilities::getNearestTmmToPoint(const Point & point, bool checkMe) const
{
	const Player *player = NULL;
	bool nearestPlayerIsValid = false;
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_TEAMMATE, i).isValid())
		{
			if (getFullPlayer(TID_TEAMMATE, i).getUniNum() == getBody().getUniNum() and checkMe == false)
				continue;
			if (!nearestPlayerIsValid)
			{
				player = &getFullPlayer(TID_TEAMMATE, i);
				nearestPlayerIsValid = true;
			}
			else if (getFullPlayer(TID_TEAMMATE, i).getDistance(point) < player->getDistance(point))
				player = &getFullPlayer(TID_TEAMMATE, i);
		}

	for (unsigned i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_TEAMMATE, i).isValid())
		{
			if (!nearestPlayerIsValid)
			{
				player = &getHalfPlayer(TID_TEAMMATE, i);
				nearestPlayerIsValid = true;
			}
			else if (getHalfPlayer(TID_TEAMMATE, i).getDistance(point) < player->getDistance(point))
				player = &getHalfPlayer(TID_TEAMMATE, i);
		}

	const Player &tmpPlayer = *player;
	return tmpPlayer;
}

const Player *WorldModelUtilities::getNearestTmmToPointFromList(const Point & point, const std::string& players) const
{
	int nearestPlayer = -1;

	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_TEAMMATE, i).isValid() and isInPlayers(getFullPlayer(TID_TEAMMATE, i).getUniNum(), players))
		{
			if (nearestPlayer == -1)
				nearestPlayer = i;
			else if (point.getDistance(getFullPlayer(TID_TEAMMATE, i).getPos().asPoint()) <
                                 point.getDistance(getFullPlayer(TID_TEAMMATE, nearestPlayer).getPos().asPoint()))
				nearestPlayer = i;
		}

	if (nearestPlayer == -1)
		return NULL;
	else
		return fullPlayers[TID_TEAMMATE][nearestPlayer];
}

const Player *WorldModelUtilities::getNearestOppToPointFromNotList(const Point & point, const std::string& blackPlayers) const
{
	int nearestPlayer = -1;

	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_OPPONENT, i).isValid() and !isInPlayers(getFullPlayer(TID_OPPONENT, i).getUniNum(), blackPlayers))
		{
			if (nearestPlayer == -1)
				nearestPlayer = i;
			else if (point.getDistance(getFullPlayer(TID_OPPONENT, i).getPos().asPoint()) <
					point.getDistance(getFullPlayer(TID_OPPONENT, nearestPlayer).getPos().asPoint()))
				nearestPlayer = i;
		}

	if (nearestPlayer == -1)
		return NULL;
	else
		return fullPlayers[TID_OPPONENT][nearestPlayer];
}

const Player *WorldModelUtilities::getNearestTmmToPointFromNotList(const Point & point, const std::string& blackPlayers) const
{
	int nearestPlayer = -1;

	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_TEAMMATE, i).isValid() and !isInPlayers(getFullPlayer(TID_TEAMMATE, i).getUniNum(), blackPlayers))
		{
			if (nearestPlayer == -1)
				nearestPlayer = i;
			else if (point.getDistance(getFullPlayer(TID_TEAMMATE, i).getPos().asPoint()) <
					point.getDistance(getFullPlayer(TID_TEAMMATE, nearestPlayer).getPos().asPoint()))
				nearestPlayer = i;
		}

	if (nearestPlayer == -1)
		return NULL;
	else
		return fullPlayers[TID_TEAMMATE][nearestPlayer];
}

const Player *WorldModelUtilities::getNearestPlayerToPointFromNotList(const Point & point, const std::string& blackTeammates, const std::string& blackOpponents) const
{
	int nearestPlayer = -1;
	TeamId nearestPlayerTeam;
	float minDist = 999.f;

	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_TEAMMATE, i).isValid() and !isInPlayers(getFullPlayer(TID_TEAMMATE, i).getUniNum(), blackTeammates))
		{
			if (point.getDistance(getFullPlayer(TID_TEAMMATE, i).getPos().asPoint()) < minDist)
			{
				nearestPlayer = i;
				minDist = point.getDistance(getFullPlayer(TID_TEAMMATE, i).getPos().asPoint());
				nearestPlayerTeam = TID_TEAMMATE;
			}
		}

	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_OPPONENT, i).isValid() and !isInPlayers(getFullPlayer(TID_OPPONENT, i).getUniNum(), blackOpponents))
		{
			if (point.getDistance(getFullPlayer(TID_OPPONENT, i).getPos().asPoint()) < minDist)
			{
				nearestPlayer = i;
				minDist = point.getDistance(getFullPlayer(TID_OPPONENT, i).getPos().asPoint());
				nearestPlayerTeam = TID_OPPONENT;
			}
		}

	if (nearestPlayer == -1)
		return NULL;
	else
		return fullPlayers[nearestPlayerTeam][nearestPlayer];
}

const Player &WorldModelUtilities::getNearestTmmToBall() const
{
	return getNearestTmmToPoint(getBall().getPos());
}

const Player *WorldModelUtilities::getNearestOppToBall() const
{
	return getNearestOppToPoint(getBall().getPos());
}

const Player *WorldModelUtilities::getNearestOppToPoint(const Point & point) const
{
	const Player *player = NULL;
	bool nearestPlayerIsValid = false;
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_OPPONENT, i).isValid())
		{
			if (!nearestPlayerIsValid)
			{
				player = &getFullPlayer(TID_OPPONENT, i);
				nearestPlayerIsValid = true;
			}
			else if (getFullPlayer(TID_OPPONENT, i).getDistance(point) < player->getDistance(point))
				player = &getFullPlayer(TID_OPPONENT, i);
		}

	for (unsigned i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_OPPONENT, i).isValid())
		{
			if (!nearestPlayerIsValid)
			{
				player = &getHalfPlayer(TID_OPPONENT, i);
				nearestPlayerIsValid = true;
			}
			else if (getHalfPlayer(TID_OPPONENT, i).getDistance(point) < player->getDistance(point))
				player = &getHalfPlayer(TID_OPPONENT, i);
		}

	return player;
}

bool WorldModelUtilities::isPlayerInOurField(const Player &player)
{
	if (player.getPos().getX() <= 0)
		return true;
	return false;
}

bool WorldModelUtilities::isPlayerInOppField(const Player &player)
{
	if (player.getPos().getX() > 0)
		return true;
	return false;
}

const Formation& WorldModelUtilities::getFormation() const
{
	return const_cast<const Formation&>(formation);
}

const Formation& WorldModelUtilities::getBeforeKickOffFormation() const
{
	return const_cast<const Formation&>(beforeKickOffFormation);
}

const Formation& WorldModelUtilities::getNonPlayOnFormation() const
{
	return const_cast<const Formation&>(nonPlayOnFormation);
}

float WorldModelUtilities::getTmmDefenseLine() const
{
	float min = 0xFFFF;

	for (unsigned i = 1; i <= 5; i++)
		if (fullPlayers[TID_TEAMMATE][i - 1]->isValid())
			min = fmin(min, fullPlayers[TID_TEAMMATE][i - 1]->getPos().getX());

	if (fullPlayers[TID_TEAMMATE][8 - 1]->isValid())
		min = fmin(min, fullPlayers[TID_TEAMMATE][8 - 1]->getPos().getX());

	if (min == 0xFFFF)
		return -52.5;
	return min;
}

float WorldModelUtilities::getOppOffsideLine(bool isBallFlag) const
{
	float offsideLine = -0xFFFF;
	bool goalieFound = false;//, firstHalf = true;

	for (unsigned int i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		if (getFullPlayer(TID_OPPONENT, i).isValid() and getFullPlayer(TID_OPPONENT, i).isGoalie())
			goalieFound = true;
		if (getFullPlayer(TID_OPPONENT, i).isValid() and not getFullPlayer(TID_OPPONENT, i).isGoalie()
				and getFullPlayer(TID_OPPONENT, i).getPos().getX() > offsideLine)
		{
			offsideLine = getFullPlayer(TID_OPPONENT, i).getPos().getX();
			LOG << "Full Player " << i + 1 << " => " << offsideLine << endl;
		}
	}

	float halfMin = offsideLine;
	for (unsigned int i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_OPPONENT, i).isValid() and not getHalfPlayer(TID_OPPONENT, i).isGoalie()
				and getHalfPlayer(TID_OPPONENT, i).getPos().getX() > offsideLine)
	{
			/*if ((not goalieFound) and firstHalf)
				firstHalf = false;
			else*/
			if (not goalieFound)
			{
				offsideLine = halfMin;
				halfMin = getHalfPlayer(TID_OPPONENT, i).getPos().getX();
				LOG << "#Half Player " << i + 1 << " => Stored Value = " << halfMin
				    << " Offside Line = " << offsideLine << endl;
			}
			else
			{
				offsideLine = getHalfPlayer(TID_OPPONENT, i).getPos().getX();
				LOG << "Half Player " << i + 1 << " => " << offsideLine << endl;
			}
	}

	////
	/*unsigned maxSimCounter = 0;
	for (unsigned int i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		if (getFullPlayer(TID_OPPONENT, i).isValid() and (not getFullPlayer(TID_OPPONENT, i).isGoalie())
			and offsideLine - getFullPlayer(TID_OPPONENT, i).getPos().getX() < 7)
		{
			if (maxSimCounter < getFullPlayer(TID_OPPONENT, i).getSimCounter())
				maxSimCounter = getFullPlayer(TID_OPPONENT, i).getSimCounter();
		}
	}
	for (unsigned int i = 0; i < HALF_PLAYERS_NUM; i++)
	{
		if (getHalfPlayer(TID_OPPONENT, i).isValid() and (not getHalfPlayer(TID_OPPONENT, i).isGoalie())
			and offsideLine - getHalfPlayer(TID_OPPONENT, i).getPos().getX() < 7)
		{
			if (maxSimCounter < getHalfPlayer(TID_OPPONENT, i).getSimCounter())
				maxSimCounter = getHalfPlayer(TID_OPPONENT, i).getSimCounter();
		}
	}
	const WorldModelHear * thisHear = dynamic_cast <const WorldModelHear *> (this);
	if (curCycle - thisHear->getHeardOffsideCycle() < 3 and maxSimCounter > curCycle - thisHear->getHeardOffsideCycle())
	{
		LOG << "Change offside line by hear." << offsideLine << " -> " << thisHear->getHeardOffsideLine() << endl;
		offsideLine = thisHear->getHeardOffsideLine();
	}*/
	////

	if (isBallFlag)
		if (getBall().getPos().getX() > offsideLine)
		{
			offsideLine = getBall().getPos().getX();
			LOG << "Ball => " << offsideLine << endl;
		}

	if (offsideLine < 0)
		offsideLine = 0;

	return offsideLine;
}

float WorldModelUtilities::getOppLocalOffLine(bool isBallFlag) const
{
	float offsideLine = -0xFFFF;
	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_OPPONENT, i).isValid() and not getFullPlayer(TID_OPPONENT, i).isGoalie() and
				getFullPlayer(TID_OPPONENT, i).getPos().getX() > offsideLine and
				(abs(getFullPlayer(TID_OPPONENT, i).getPos().getY() - getBall().getPos().getY()) < OPP_OFFLINE_LOCAL_Y and
				 abs(getFullPlayer(TID_OPPONENT, i).getPos().getX() - getBall().getPos().getX()) < OPP_OFFLINE_LOCAL_X))
			offsideLine = getFullPlayer(TID_OPPONENT, i).getPos().getX();

	for (int i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_OPPONENT, i).isValid() and not getHalfPlayer(TID_OPPONENT, i).isGoalie()  and
				getHalfPlayer(TID_OPPONENT, i).getPos().getX() > offsideLine and
				(abs(getHalfPlayer(TID_OPPONENT, i).getPos().getY() - getBall().getPos().getY()) < OPP_OFFLINE_LOCAL_Y and
			 	abs(getHalfPlayer(TID_OPPONENT, i).getPos().getX() - getBall().getPos().getX()) < OPP_OFFLINE_LOCAL_X))
			offsideLine = getHalfPlayer(TID_OPPONENT, i).getPos().getX();

	if (isBallFlag)
		if (getBall().getPos().getX() > offsideLine)
			offsideLine = getBall().getPos().getX();

	if (offsideLine < 0)
		offsideLine = 0;

	return offsideLine;
}

float WorldModelUtilities::getTmmOffsideLine(bool isBallFlag) const
{
	float offsideLine = 0xFFFF;

	for (unsigned int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_TEAMMATE, i).isValid() and not getFullPlayer(TID_TEAMMATE, i).isGoalie()
				and getFullPlayer(TID_TEAMMATE, i).getPos().getX() < offsideLine)
			offsideLine = getFullPlayer(TID_TEAMMATE, i).getPos().getX();

	for (unsigned int i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_TEAMMATE, i).isValid() and not getHalfPlayer(TID_TEAMMATE, i).isGoalie()
				and getHalfPlayer(TID_TEAMMATE, i).getPos().getX() < offsideLine)
			offsideLine = getHalfPlayer(TID_TEAMMATE, i).getPos().getX();

	if (isBallFlag)
		if (getBall().getPos().getX() < offsideLine)
			offsideLine = getBall().getPos().getX();

	if (offsideLine > 0)
		offsideLine = 0;

	return offsideLine;
}

int WorldModelUtilities::getOppPlayersBeforeLine(float lineX) const
{
	int counter = 0;

	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_OPPONENT, i).isValid() && getFullPlayer(TID_OPPONENT, i).getPos().getX() <= lineX)
			counter++;

	for (int i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_OPPONENT,i).isValid() && getHalfPlayer(TID_OPPONENT, i).getPos().getX() <= lineX)
			counter++;

	return counter;
}

int WorldModelUtilities::getOppPlayersBetweenLines(float line1X, float line2X) const
{
	return getOppPlayersBeforeLine(line2X) - getOppPlayersBeforeLine(line1X);
}

const Player *WorldModelUtilities::getTmmGoalie() const
{
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_TEAMMATE, i).isValid() &&
			getFullPlayer(TID_TEAMMATE, i).isGoalie())
			return fullPlayers[TID_TEAMMATE][i];

	for (unsigned i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_TEAMMATE, i).isValid() &&
			getHalfPlayer(TID_TEAMMATE, i).isGoalie())
			return halfPlayers[TID_TEAMMATE][i];

	return NULL;
}

const Player *WorldModelUtilities::getOppGoalie() const
{
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
		if (getFullPlayer(TID_OPPONENT, i).isValid() &&
			getFullPlayer(TID_OPPONENT, i).isGoalie())
			return fullPlayers[TID_OPPONENT][i];

	for (unsigned i = 0; i < HALF_PLAYERS_NUM; i++)
		if (getHalfPlayer(TID_OPPONENT, i).isValid() &&
			getHalfPlayer(TID_OPPONENT, i).isGoalie())
			return halfPlayers[TID_OPPONENT][i];

	return NULL;
}

float WorldModelUtilities::getSecurityStatus(const Player &player, float oval_a, float oval_b) const
{
	float securityStatus = 100000;
	float tmpSecurityStatus = 0;
	if (!player.isValid()) return 0;
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_OPPONENT][i]->isValid())
		{
			float oppPlayerX = fullPlayers[TID_OPPONENT][i]->getPos().getX() -
					player.getPos().getX();
			float oppPlayerY = fullPlayers[TID_OPPONENT][i]->getPos().getY() -
					player.getPos().getY();
			tmpSecurityStatus = abs(2 * oval_b *
					oppPlayerX * oppPlayerY *	sqrt(abs(
					1 / (oval_a * oppPlayerY) + 1 / (oval_b * oppPlayerX))));
			if (abs(oppPlayerX) < .05)
				tmpSecurityStatus = abs(2 * (oval_b / oval_a) * oppPlayerY);
			if (abs(oppPlayerY) < .05)
				tmpSecurityStatus = abs(2 * oppPlayerX);
			if (tmpSecurityStatus < securityStatus)
				securityStatus = tmpSecurityStatus;
		}

	for (unsigned i = 0; i < HALF_PLAYERS_NUM; i++)
		if (halfPlayers[TID_OPPONENT][i]->isValid())
		{
			float oppPlayerX = halfPlayers[TID_OPPONENT][i]->getPos().getX() -
					player.getPos().getX();
			float oppPlayerY = halfPlayers[TID_OPPONENT][i]->getPos().getY() -
					player.getPos().getY();
			tmpSecurityStatus = abs(2 * oval_b * oppPlayerX * oppPlayerY *
					sqrt(1 / (oval_a * oppPlayerY) +
					1 / (oval_a * oppPlayerX)));
			if (abs(oppPlayerX) < .05)
				tmpSecurityStatus = abs(2 * (oval_b / oval_a) * oppPlayerY);
			if (abs(oppPlayerY) < .05)
				tmpSecurityStatus = abs(2 * oppPlayerX);
			if (tmpSecurityStatus < securityStatus)
				securityStatus = tmpSecurityStatus;
		}

	return securityStatus;
}

float WorldModelUtilities::getPathSecurityStatus(const Player &srcPlayer, const Point trgPoint, float forwardRate) const
{
	float minAngleSecurity = 100000;
	Vector meToTRGPoint;
	meToTRGPoint.setByPoints(srcPlayer.getPos(), trgPoint);
	float trgDirection = absoluteAngle(meToTRGPoint.getDirection());
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
		if (fullPlayers[TID_OPPONENT][i]->isValid())
		{
			Vector meToTmpOpp;
			meToTmpOpp.setByPoints(srcPlayer.getPos(),
					fullPlayers[TID_OPPONENT][i]->getPos());
			if (meToTmpOpp.getMagnitude() <= meToTRGPoint.getMagnitude() + 1)
			{
				float tmpAngle = absoluteAngle(abs(trgDirection -
						absoluteAngle(meToTmpOpp.getDirection())));
				float tmpAngleSecurity = DEG2RAD * tmpAngle;
				if ((trgDirection > meToTmpOpp.getDirection() &&
						normalizeAngle(trgDirection) >= 0) ||
						(trgDirection < meToTmpOpp.getDirection() &&
						normalizeAngle(trgDirection) <= 0))
					tmpAngleSecurity = DEG2RAD * tmpAngle + (forwardRate - 1) *
							DEG2RAD * tmpAngle * (sin(DEG2RAD * tmpAngle) + 1);
				if (tmpAngleSecurity < minAngleSecurity)
					minAngleSecurity = tmpAngleSecurity;
			}
		}

	for (unsigned i = 0; i < HALF_PLAYERS_NUM; i++)
		if (halfPlayers[TID_OPPONENT][i]->isValid())
		{
			Vector meToTmpOpp;
			meToTmpOpp.setByPoints(srcPlayer.getPos(),
					halfPlayers[TID_OPPONENT][i]->getPos());
			if (meToTmpOpp.getMagnitude() >= meToTRGPoint.getMagnitude() + 4)
			{
				float tmpAngle = absoluteAngle(trgDirection - meToTmpOpp.getDirection());
				float tmpAngleSecurity = DEG2RAD * tmpAngle;
				if ((trgDirection > meToTmpOpp.getDirection() &&
						normalizeAngle(trgDirection) >= 0) ||
						(trgDirection < meToTmpOpp.getDirection() &&
						normalizeAngle(trgDirection) <= 0))
					tmpAngleSecurity = DEG2RAD * tmpAngle + (forwardRate - 1) *
							DEG2RAD * tmpAngle * (sin(DEG2RAD * tmpAngle) + 1);
				if (tmpAngleSecurity < minAngleSecurity)
					minAngleSecurity = tmpAngleSecurity;
			}
		}

	return minAngleSecurity;
}

float WorldModelUtilities::getTackleEffectivePower(float Direction) const
{
	float target_rel_angle = Degree::getDeltaAngle(Direction, getBody().getBodyDir());
	float ball_rel_angle = Degree::getDeltaAngle(Vector(getBall().getPos() - getBody().getPos()).getDirection(), getBody().getBodyDir());
	double eff_power
		= MAX_BACK_TACKLE_POWER
		+ ( ( MAX_TACKLE_POWER - MAX_BACK_TACKLE_POWER )
		* ( 1.0 - fabs(target_rel_angle) / 180.0 ) );
	eff_power *= getBall().getTacklePowerRate();
	eff_power *= 1.0 - 0.5*( fabs(ball_rel_angle) / 180.0 );
	return eff_power;
}

