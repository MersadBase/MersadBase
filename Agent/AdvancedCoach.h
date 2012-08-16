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
 *  	and is modified by: Mehrdad Bakhtiari
 *
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __ADVANCED_COACH_H
#define __ADVANCED_COACH_H

#include <vector>

#include <TeamGraphic.h>
#include <SyncedCoach.h>
#include <CoachWorldModel.h>

class AdvancedCoach: public SyncedCoach
{
protected:
	int tasksNum;
	int numSentFreeForm;
	int lastDefenseMsgSentCycle;

	TeamGraphic M_team_graphic;

	static unsigned lastSubstituteCycle;
	static std::vector<unsigned> usedTypes;
	static unsigned remainSubstitutes;

	int sendChangeType;
	bool defenseMessageSent;
	int playersType[10];
	void setPlayersType();

	void sendTeamGraphic();
	bool doTeamGraphic( const int x,
				const int y,
				const TeamGraphic & team_graphic );

	// It substitute player number "uniformNum"
	void substitute(unsigned uniformNum);

public:
	AdvancedCoach(int argc, char **argv);
	virtual ~AdvancedCoach();

	virtual void changePlayerType(const SExpression &exp);
	virtual bool process();
};

#endif // __ADVANCED_COACH_H
