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
 *  	and is modified by: Mehrdad Bakhtiari, MohammadReza Montazeri Shatoori
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __HEAD_DECISION_H
#define __HEAD_DECISION_H

#include <WorldModel.h>
#include <DecideCommonTypes.h>

#define TURN_NECK_RESOLUTION 5

class Command;
class AdvancedAgent;

class HeadDecision
{
protected:
	float viewPartWeights[VIEW_PARTS_NUM];
	unsigned viewPartObjectsNums[VIEW_PARTS_NUM];

	Body nextBody;
	Ball nextBall;
	Command *turnNeckCommand;
	Command *changeViewCommand;

	// Weighting rates
	float haveBallRate;
	float havePlayerRate;
	float objectsNumRate;
	float offenseRate;

	// Giving vars
	AdvancedAgent *notConstOwner;
	const AdvancedAgent *owner;
	const WorldModel *worldModel;

	void decideForLookCarefullyToBall();
	void decideForLookCarefullyToOppGoalie(TurnNeckMode &tnMode);
	void decideForLookNormallyToOurGoalie(TurnNeckMode &tnMode);
	void decideForLookCarefullyToOppPlayer(TurnNeckMode &tnMode, unsigned attentionNum);
	void decideForTurnNeckCommand(float nextViewAngle);
	void decideForLookNormallyToBall(float nextViewAngle, TurnNeckMode &tnMode);
	void decideForLookCarefullyToOppAndBall(TurnNeckMode &tnMode, unsigned attentionNum);
	void decideForOffensiveIntercept(TurnNeckMode &tnMode);
	void decideForLookToReceiver(TurnNeckMode &tnMode, unsigned attentionNum);
	void decideForOffensiveLook(TurnNeckMode &tnMode);
	void decideForLookToDir(TurnNeckMode &tnMode, float direction);
	void decideForLookToTmmAndBall(TurnNeckMode &tnMode, unsigned attentionNum);

	void logVariables();
	void updateViewPartObjectsNum();
	void setWeightsForLookNormally();
	void setWeightsForLookCarefully();
	void setWeightsForLookOffensive();

	float getViewPartWeight(unsigned viewPartNum);

public:
	HeadDecision(AdvancedAgent *owner);
	~HeadDecision();

	void resetInfo();
	void decide(const Command *bodyCycleCommand,
			const HeadDecisionForm &headForm);

	void updateAfterSee();
	void updateAfterSenseBody();

	Command *getTurnNeckCommand();
	Command *getChangeViewCommand();
};

#endif // __HEAD_DECISION_H
