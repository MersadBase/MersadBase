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
 *  	and is modified by: Mohammad Salehe, Sassan Haradji, Pooria Kaviani
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <cassert>

#include <Share.h>
#include <Degree.h>
#include <Logger.h>
#include <Command.h>
#include <Intercept.h>
#include <AdvancedAgent.h>
#include <Config.h>
#include <BodyDecision.h>

using namespace std;


// AdvancedAgent fuctions
AdvancedAgent::AdvancedAgent(int argc, char **argv): SyncedAgent(argc, argv),
		sayDecision(worldModel),
		headDecision(this), pointToDecision(worldModel),
		library(worldModel)
{
}

AdvancedAgent::~AdvancedAgent()
{
}

void AdvancedAgent::senseBody(const SExpression &exp)
{
	SyncedAgent::senseBody(exp);

	worldModel->setBall().setUpdateByHearPermitted(true);

	worldModel->updateFastIC();

	form.reset();
	if (library.commentsHis.getLastCycle(worldModel->getCurTime()))
		library.commentsHis.getLastCycle(worldModel->getCurTime())->
				setBodyCommandFailed(isBodyCycleCommandLost());
}

void AdvancedAgent::see(const SExpression &exp)
{
	SyncedAgent::see(exp);

	worldModel->updateFastIC();
}

void AdvancedAgent::hear(const SExpression &exp)
{
	SyncedAgent::hear(exp);

	SExpAtomic *at;

	at = dynamic_cast<SExpAtomic *>(exp[2]);
	assert(at);
	if (at->toString() == "referee")
	{
		if (worldModel->checkPlayModeForPenalty() == 1)
		{
			LOG << "AdvancedAgent::hear : Reversing Field" << endl;
			if (!worldModel->getBody().isGoalie())
			{
				LOG << "Reverse. You are player" << endl;
				worldModel->setFieldData().setPenaltySideRate(-1);
			}
			else
				LOG << "NO! Don't Reverse. You are goalie" << endl;
		}
		else if (worldModel->checkPlayModeForPenalty() == 2)
		{
			LOG << "AdvancedAgent::hear : -1 Reversing Field" << endl;
			if (worldModel->getBody().isGoalie())
			{
				LOG << "Reverse. You are goalie" << endl;
				worldModel->setFieldData().setPenaltySideRate(-1);
			}
			else
				LOG << "NO! Don't Reverse. You are player" << endl;
		}
	}
	else
		worldModel->updateFastIC();


	if ((worldModel->getHearedHeader() == 'A' ||
		 worldModel->getHearedHeader() == 'B') &&
		worldModel->getHearedSender() == 1 &&
		worldModel->getHearedInterceptNum() != 1 &&
	    worldModel->getPlayMode() == PM_GOAL_KICK)
	{
		if (worldModel->getPlayModeRemainCycle() < 199)
		{
			LOG << "GOAL_KICK --> GOAL_KICK_TAKEN" << endl;
			worldModel->setVirtualPlayMode(VPM_GOAL_KICK_TAKEN);
		}
		else
			LOG << "GOAL_KICK --> NO CHANGE TO PLAY MODE" << endl;
	}
}

bool AdvancedAgent::bodyDecide(int decideLevel)
{
	LOG << "AdvancedAgent::bodyDecide" << endl;

	// Main Code Here
	bodyCycleCommand = NULL;

	BodyDecision decision = BodyDecision();
	decision.decide(worldModel, form);
	bodyCycleCommand = decision.getCommand();
	// Main Code Finished

	if (bodyCycleCommand == NULL)
		bodyCycleCommand = new EmptyCommand();

	library.commentsHis.setCurCycle(form.comment, worldModel->getCurTime());

	if (form.setBreakExperiment)
		library.breakExperiment = form.setBreakExperiment;

	if (worldModel->getBall().getPosDeviation() > 10 ||
		worldModel->getBall().getAbsVec().getMagnitude() < 7.5 ||
		form.comment.exist("InterceptAdvAct") ||
		form.comment.exist("DefensePlan")) // CODE HAII KE MIKHAIM DOBARE DAR SOORATE TAVANAI SEDA ZADE SHAVAND
		return true;

	return false;
}

void AdvancedAgent::sayDecide()
{
	sayDecision.decide(bodyCycleCommand, form.sayForm);

	sayCycleCommand = sayDecision.getSayCommand();
	attentionCycleCommand = sayDecision.getAttentionCommand();
}

void AdvancedAgent::pointToDecide()
{
	pointToDecision.decide(headCycleCommand);

	pointToCycleCommand = pointToDecision.getPointToCommand();
}

void AdvancedAgent::headDecide()
{
	LOG << "AdvancedAgent::headDecide" << endl;
	headDecision.decide(bodyCycleCommand, form.headForm);

	headCycleCommand = headDecision.getTurnNeckCommand();
	Share::lastCycleViewChanged = true;
	changeViewCycleCommand = headDecision.getChangeViewCommand();
}
