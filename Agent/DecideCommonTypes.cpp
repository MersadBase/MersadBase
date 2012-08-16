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
 *  	and is modified by: Mohammad Salehe, Sassan Haradji, Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cassert>

#include <Logger.h>
#include <DecideCommonTypes.h>

using namespace std;


// DecideComment functions

DecideComment::DecideComment()
{
	reset();
}

void DecideComment::reset()
{
	time = 0;
	bodyCommandFailed = false;
	headCommandFailed = false;

	comments.clear();
}

void DecideComment::addComment(string name)
{
	map<string, vector<void *> >::iterator it = comments.find(name);
	if (it != comments.end())
		return; // It exists already.
	else
	{
		vector<void *> tmp;
		comments.insert(make_pair(name, tmp));
	}
}

bool DecideComment::exist(const string &name) const
{
	map<string, vector<void *> >::const_iterator it = comments.find(name);
	return (it != comments.end());
}

const vector<void *> &DecideComment::operator[](const string &name) const
{
	map<string, vector<void *> >::const_iterator it = comments.find(name);
	assert(it != comments.end());
	return it->second;
}

vector<void *> &DecideComment::operator[](const string &name)
{
	map<string, vector<void *> >::iterator it = comments.find(name);
	assert(it != comments.end());
	return it->second;
}

// Get & Set functions
bool DecideComment::isBodyCommandFailed() const
{
	return bodyCommandFailed;
}

void DecideComment::setBodyCommandFailed(bool bodyCommandFailedArg)
{
	bodyCommandFailed = bodyCommandFailedArg;
}

int DecideComment::getTime() const
{
	return time;
}

void DecideComment::setTime(int timeArg)
{
	time = timeArg;
}


// CommentsHistory functions

const DecideComment *CommentsHistory::getLastCycle(int curTime) const
{
	for (int i = decides.size() - 1; i >= 0; i--)
		if (decides[i].getTime() == curTime - 1)
			return &decides[i];

	return NULL;
}

DecideComment *CommentsHistory::getLastCycle(int curTime)
{
	for (int i = decides.size() - 1; i >= 0; i--)
		if (decides[i].getTime() == curTime - 1)
			return &decides[i];

	return NULL;
}

void CommentsHistory::setCurCycle(const DecideComment &curCycle, int curTime)
{
	DecideComment *ptr = NULL;
	for (int i = decides.size() - 1; i >= 0; i--)
		if (decides[i].getTime() == curTime)
		{
			ptr = &decides[i];
			break;
		}

	if (ptr == NULL)
		decides.push_back(curCycle);
	else
		*ptr = curCycle;
}

// SayDecisionForm functions

SayDecisionForm::SayDecisionForm()
{
	reset();
}

void SayDecisionForm::reset()
{
	defenseSay = false;
	defenseStr = "";

	dfbSystem = false;
	dfbStatus = 0;

	passSay = false;
	doRoutinSay = false;
	beforePassSay = false;
	suggestPassSay = false;
	planSay = false;
	freeKickSay = false;
	planHeader = "";

	passNum = 12;
	interceptNum = 0;
	attentionNum = 0;
	planStatus = 0;
	gwInterceptNum = 0;
	gwAttentionNum = 0;

	planSay = false;
	radarSay = false;
}

// HeadDecisionForm functions
void HeadDecisionForm::reset()
{
	tnMode = TNM_AUTO_PLAY_ON;
}

// Library fuctions

Library::Library(const WorldModel *worldModel)
{
}


// Form fuctions
Form::Form()
{
	reset();
}

void Form::reset()
{
	comment.reset();
	sayForm.reset();
	headForm.reset();

	setBreakExperiment = 0;
}
