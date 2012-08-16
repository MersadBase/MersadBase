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

public:
	SayDecision(const WorldModel *worldModel);
	~SayDecision();

	int getSayRoutinBallPermission(const SayDecisionForm &sayForm);

	void sayDefense(const SayDecisionForm &sayForm);
	void sayDFB(const SayDecisionForm &sayForm);
	void sayPass(const SayDecisionForm &sayForm);
	void sayRadar(const SayDecisionForm &sayForm);
	void sayFreeKick(const SayDecisionForm &sayForm);
	void sayBeforePass(const Command *bodyCycleCommand,
			const SayDecisionForm &sayForm);
	void sayRoutinBall(const Command *bodyCycleCommand,
			const SayDecisionForm &sayForm);
	void sayRoutinBallWithoutVel(const Command *bodyCycleCommand,
			const SayDecisionForm &sayForm);

//	void saySuggestPass(const SayDecisionForm &sayForm);
//	void sayBeforePassRelated(const Command *bodyCycleCommand, const SayDecisionForm &sayForm);

	void decide(const Command *bodyCycleCommand,
			const SayDecisionForm &sayForm);

	Command *getSayCommand();
	Command *getAttentionCommand();
};

#endif // __SAY_DECISION_H

