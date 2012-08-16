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
 *  	and is modified by: Mohammad Salehe, Sassan Haradji,
 *				Pooria Kaviani, MohammadReza Montazeri Shatoori, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __DECIDE_COMMON_TYPES_H
#define __DECIDE_COMMON_TYPES_H

#include <map>
#include <string>
#include <vector>

#include <Vector.h>
#include <Command.h>

class WorldModel;

#ifndef NULL
#	define NULL 0
#endif // NULL

// Comment classes

class DecideComment
{
protected:
	std::map<std::string, std::vector<void *> > comments;

	int time;
	bool bodyCommandFailed;
	bool headCommandFailed;

public:
	DecideComment();

	void reset();
	void addComment(std::string name);
	bool exist(const std::string &name) const;
	std::vector<void *> &operator[](const std::string &name);
	const std::vector<void *> &operator[](const std::string &name) const;

	// Get & Set functions
	bool isBodyCommandFailed() const;
	void setBodyCommandFailed(bool bodyCommandFailedArg);

	int getTime() const;
	void setTime(int timeArg);
};

class CommentsHistory
{
protected:
	std::vector<DecideComment> decides;

public:

	DecideComment *getLastCycle(int curTime);
	const DecideComment *getLastCycle(int curTime) const;
	void setCurCycle(const DecideComment &curCycle, int curTime);
};


// Say Types

struct SayDecisionForm
{
	bool defenseSay;
	std::string defenseStr;

	bool dfbSystem;
	int dfbStatus;

	bool passSay;
	unsigned passNum;

	bool doRoutinSay;
	bool beforePassSay;

	bool suggestPassSay;
	float passDir;
	float passSpeed;
	float passWeight;
	unsigned passPlayerNum;

	unsigned interceptNum;
	unsigned attentionNum;

	bool freeKickSay;
	int freeKickAdviceNumber;

	unsigned planStatus;
	unsigned gwInterceptNum;
	unsigned gwAttentionNum;

	bool planSay;
	std::string planHeader;
	std::string planInfo;

	bool radarSay;
	int radarPlayersNum;
	int ids[3];
	Vector radarPlayers[3];

	SayDecisionForm();
	void reset();
};


// Head Types

enum TurnNeckMode {
	TNM_NONE = 0,
	TNM_AUTO_PLAY_ON = 1,
	TNM_LOOK_NORMALLY = 2,
	TNM_LOOK_CAREFULLY = 3,
	TNM_LOOK_NORMALLY_TO_BALL = 4,
	TNM_LOOK_CAREFULLY_TO_BALL = 5,
	TNM_LOOK_CAREFULLY_TO_OPP_GOALIE = 6,
	TNM_LOOK_CAREFULLY_TO_OPP_PLAYER = 7,
	TNM_LOOK_CAREFULLY_TO_OPP_AND_BALL = 8,
	TNM_LOOK_NORMALLY_TO_OUR_GOALIE = 9,
	TNM_LOOK_OFFENSIVE_INTERCEPT = 10,
	TNM_LOOK_OFFENSIVE = 11,
	TNM_LOOK_TO_RECEIVER = 12,
	TNM_SEARCH = 13,
	TNM_LOOK_TO_DIR = 14,
	TNM_LOOK_TO_TMM_AND_BALL = 15
};

struct HeadDecisionForm
{
	TurnNeckMode tnMode;
	unsigned attentionNum;
	float direction;

	void reset();
};


// Decide Types

class Form
{
public:
	DecideComment comment;
	SayDecisionForm sayForm;
	HeadDecisionForm headForm;

	float setBreakExperiment;

	Form();
	void reset();
};

class Library
{
protected:
public:
	int decideLevel;
	float breakExperiment;

	const Command *lastBodyCycleCommand;

	CommentsHistory commentsHis;

	Library(const WorldModel *worldModel);
};

#endif // __DECIDE_COMMON_TYPES_H

