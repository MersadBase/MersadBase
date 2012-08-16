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
 *  	and is modified by: Pooria Kaviani
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __ADVANCED_AGENT_H
#define __ADVANCED_AGENT_H

#include <SyncedAgent.h>
#include <SayDecision.h>
#include <HeadDecision.h>
#include <PointToDecision.h>
#include <DecideCommonTypes.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

class AdvancedAgent: public SyncedAgent
{
public:
	AdvancedAgent(int argc, char **argv);
	virtual ~AdvancedAgent();

protected:
	SayDecision sayDecision;
	HeadDecision headDecision;
	PointToDecision pointToDecision;

	Form form;
	Library library;

	virtual void senseBody(const SExpression &exp);
	virtual void see(const SExpression &exp);
	virtual void hear(const SExpression &exp);

	virtual void sayDecide();
	virtual void headDecide();
	virtual void pointToDecide();
	virtual bool bodyDecide(int decideLevel);
};

#endif // __ADVANCED_AGENT_H
