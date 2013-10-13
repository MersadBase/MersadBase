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
 *  This file is created by: Mohammad Zareie
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#ifndef __PASS_H
#define __PASS_H

#include <Command.h>
#include <WorldModel.h>
#include <Formation.h>
#include <DecideCommonTypes.h>
#include <KickEntry.h>
#include <Rectangle.h>

class Pass
{
private:
	Command *command;
	const WorldModel *wm;
	Point target;

	string validPlayers;
	Rectangle validRect;
	Rectangle invalidRect;
	bool isValueCalculated;

	PassEntry bestEntry;
	PassEntry bestSafeEntry;

	int maxSteps;

	unsigned safeDeltaCycle;
	float safeDistance;
	bool safeSearch;

	vector<PassEntry> entries;

	void createEntries();
	void addSpeedEntry(PassEntry& passEntry);
	void evaluateEntries();
	void findBestEntry();
	void evaluate(PassEntry &tmp, FastIC &temp);
	bool isSafe(const PassEntry& entry) const;
	bool isCrowded(PassEntry &tmp,vector<pair<float, const Player*> > v) const;
	unsigned getOppCircleCount(vector<pair<float, const Player*> > v) const;
	float getInvalidOppValue(const PassEntry& entry) const;

protected:

public:

	Pass(const WorldModel *worldModel);
	void setValidPlayers(string);
	void addValidPlayer(int);
	bool isStillGood(PassEntry &entry);
	~Pass();
	bool isPassAvailable();
	bool isSafePassAvailable();
	float getValue();
	float getSafeValue();
	PassEntry getBestEntry() const;
	PassEntry getBestSafeEntry() const;
	void decide(Form &form);
	void safeDecide(Form &form);
	Command *getCommand();
	Rectangle& getValidRect();
	Rectangle& getinValidRect();
	void setMaxSteps(unsigned step);
	void setSafety(bool safety);
};

#endif // __PASS_H
