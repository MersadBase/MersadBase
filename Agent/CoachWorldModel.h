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
 *  This file is created by: Pooria Kaviani
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __COACH_WORLD_MODEL_H
#define __COACH_WORLD_MODEL_H

#include <vector>

#include <Ball.h>
#include <Timer.h>
#include <Config.h>
#include <Object.h>
#include <Player.h>
#include <FieldData.h>
#include <WorldModel.h>
#include <SExpression.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

#define COACH_WORLD_MODEL_HISTORY_SIZE 100

class CoachWorldModel: public WorldModel
{
protected:
	// special to trainer
	std::string leftTeam;
	std::string rightTeam;
	bool playingRight;
	/// end trainer

public:
	CoachWorldModel();
	CoachWorldModel(const CoachWorldModel &worldModelCopy);
	virtual ~CoachWorldModel();

	virtual void parseSeeGlobal(const SExpression &exp);
	virtual void parseGoal(const SExpression &exp);
	virtual void parseBall(const SExpression &exp);
	virtual void parsePlayer(const SExpression &exp);

	virtual void parseHear(const SExpression &exp);

	virtual void logObjects();

	void setLeftTeam(const std::string & teamName);
	void setRightTeam(const std::string & teamName);
	const std::string & getLeftTeam() const;
	const std::string & getRightTeam() const;

};

class CoachWorldModelHistory
{
protected:
	CoachWorldModel *history[COACH_WORLD_MODEL_HISTORY_SIZE];
	int size;
	int curTime;
	int startTime;

	void moreNew();
public:
	CoachWorldModelHistory();
	~CoachWorldModelHistory();

// get
//	const CoachWorldModel **gethistory() const;
	int getSize() const;
	int getCurTime() const;
	int getStartTime() const;

// set
	CoachWorldModel **sethistory();
	void setSize(int sizeArg);
	void setCurTime(int curTimeArg);
	void setStartTime(int startTimeArg);

	void saveWorldModel(CoachWorldModel *worldModel, unsigned time);
	const CoachWorldModel *operator[](int time) const;
};

#endif // __COACH_WORLD_MODEL_H
