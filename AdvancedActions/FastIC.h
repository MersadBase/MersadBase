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
 *  This file is created by: MohammadReza Montazeri Shatoori
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __FAST_IC_H_
#define __FAST_IC_H_

#include <ctime>
#include <vector>
#include <Vector.h>
#include <Player.h>

#define EXTRA_DASH_RATE 0.10
#define TYPESCOUNT 18
#define EXHAUSTION_RATE 0.45
#define TURN_VELCOUNT 10
#define TURN_ANGLECOUNT 18

using namespace std;

class WorldModel;
class Player;

struct TurnModel
{
	int cyclesToTurn[TURN_VELCOUNT][TURN_ANGLECOUNT];
	float maxVelocity;
	float maxAngleChange;

	int getCyclesToTurn( float velocity , float angleChange );

	TurnModel()
	{
		maxVelocity = 1.f;
		maxAngleChange = 180.f;
	}
};

struct FastType
{
	TurnModel Turn;
	float Distance[150];
	float KickableArea;
	float CatchableArea;
	float Decay;
};

struct FastPlayer
{
	Point pos;
	Point vel;
	int delay;
	int extraDashCycles;
	float dashMulti;
	float controlMulti;
	int exhaustionTime;
	bool calculated;
	const Player * playerPtr;
	bool hasDir;
	bool plusVel;

	FastPlayer();
	FastPlayer(const WorldModel * worldModel, int _delay, float _controlBuff ,float _dashMulti ,const Player * _playerPtr, int _extraDashCycles = 0, bool _plusVel = false);

	int getExhaustionTime(const WorldModel * worldModel);
};

struct FastBall
{
	Point pos;
	Point vel;
	float decay;
	int delay;
	FastBall() : pos(0.f, 0.f), vel(0.f, 0.f), decay(0), delay(0)
	{}
};

struct FastestPlayer
{
	int unum;
	bool isTeammate;
	int reachCycle;
	Point receivePoint;
	const Player * playerPtr;
	FastestPlayer( ) :  unum(-1),
						isTeammate(false),
						reachCycle(9999),
						receivePoint(),
						playerPtr(NULL)
	{
	}
	FastestPlayer( int _unum, bool _isTeammate, int _reachCycle, Point _receivePoint, const Player * _playerPtr = NULL) :  unum(_unum),
																							isTeammate(_isTeammate),
																							reachCycle(_reachCycle),
																							receivePoint(_receivePoint),
																							playerPtr(_playerPtr)
	{
	}
};

class FastIC
{
	static bool isSet;
	static FastType types[TYPESCOUNT];

	FastBall ball;
	//Ball ball;

	vector <FastPlayer> fastPlayers;
	vector <FastestPlayer> fastestPlayers;

	const WorldModel * worldModel;
	unsigned maxAll, maxTmm, maxOpp, maxCyclesAfter, maxCycles;
	bool checkQuarters;

	bool calculateSelf;

	float unknownExtraKickable;

	bool outFlag;
	unsigned outCycle;

public:
	FastIC();
	FastIC(const WorldModel * world, bool quarter = false);
	~FastIC();

	bool log;

	void addPlayer(const Player * _playerPtr, int _delay = 0, float _controlBuff = 1.0, float _dashMulti = 1.0, int extraDashCycle = 0, bool _plusVel = false);
	vector<FastPlayer> & players();

	void refresh();
	void updateBody(unsigned updateCycle);
	void calculate();
	void setByWorldModel();

	const std::vector<FastestPlayer> & getFastestPlayers() const;

	void setBall(Point ballPos, Point ballVel, int ballDelay = 0, float ballDecay = -9999);
	void setBall(Ball _ball, unsigned delay);

	bool isFastestQuarter() const;
	bool isSelfFastestPlayer() const;
	bool isSelfFastestTeammate() const;
	bool isOurTeamBallPossessor() const;
	int getFastestTeammateReachCycle(bool withMe = true) const;
	Point getFastestTeammateReachPoint(bool withMe = true) const;
	int getFastestOpponentReachCycle() const;
	Point getFastestOpponentReachPoint() const;
	int getFastestPlayerReachCycle() const;
	int getSelfReachCycle() const;
	const Player * getFastestTeammate(bool withMe = true) const;
	const Player * getFastestOpponent() const;
	const Player * getFastestPlayer() const;

	void setMaxCount(unsigned max);
	void setMaxTeammateCount(unsigned max);
	void setMaxOpponentCount(unsigned max);
	void setMaxCycleAfterFirstFastestPlayer(unsigned max);
	void setMaxCycles(unsigned max);
	void setCalculateSelf(bool cal);

	void setUnknownExtraKickable(float exKick);

	bool isOut();
	unsigned getOutCycle();

	void deletePointers();
	void logFastests() const;
};

#endif // __FAST_IC_H_
