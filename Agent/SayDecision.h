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
 *  	and is modified by: Mohammadreza Montazeri
 *
 *  Released on Monday 13 September 2010, 22 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/
#ifndef __SAY_DECISION_H
#define __SAY_DECISION_H

#include <DecideCommonTypes.h>

#define RELATED_PLAYER_X_RANGE 47.5
#define RELATED_PLAYER_Y_RANGE 30

class Command;
class WorldModel;


class SayDecision
{
protected:
	const WorldModel *worldModel;
	Command *sayCommand;
	Command *attentionCommand;
	CodeItem curCode;

public:
	SayDecision(const WorldModel *worldModel);
	~SayDecision();

	void decide(const Command *bodyCycleCommand,
			const SayDecisionForm &sayForm);

	Command *getSayCommand();
	Command *getAttentionCommand();
private:
	void addSayCode(CodeItem& code);
	void sayPass(const Command *bodyCycleCommand, const SayDecisionForm &sayForm);

};

#endif // __SAY_DECISION_H
