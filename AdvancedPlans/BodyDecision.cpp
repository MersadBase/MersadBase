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
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

// برای توضیحات در مورد کار هر یک از تابع ها فایل BodeDecision.h را ببینید.
#include <BodyDecision.h>
#include <NonPlayOnDecision.h>
#include <Share.h>
#include <Goalie.h>
#include <DefensePlan.h>
#include <Middle.h>
#include <Offense.h>
#include <Basics.h>
#include <Logger.h>

using namespace std;
using namespace Basics;

BodyDecision::BodyDecision()
{
	command = NULL;
}

BodyDecision::~BodyDecision()
{
}

void BodyDecision::decide(const WorldModel* worldModel, Form &form)
{
	const WorldModel* wm = worldModel;
	LOG << "Decision Section" << endl;
	if (worldModel->getBody().getBattery() / worldModel->getBody().getStaminaCapacity()
			< worldModel->getCurCycle() / 3000 * (3 / 4)
			and worldModel->getPlayMode() != PM_PLAY_ON)
	{
		LOG << "i wanna going home!!!" << endl;
		// TODO : say request to coach
	}
	if (wm->getGlobalFastIC().getFastestTeammate(true))
		form.sayForm.attentionNum = wm->getGlobalFastIC().getFastestTeammate(true)->getUniNum();
	if (worldModel->getBall().isValid())
	{
		for (unsigned i = 0; i < Share::chain.size(); i++)
		{
			ChainAction ca = Share::chain[i]; //i don't use chain directly because they may delete themselves
			if (ca.cycle == wm->getCurCycle())
			{
				ca.wm = wm;
				LOG << ca.type << endl;
				ca.calCommand(form);
				if (ca.getCommand())
				{ //i don't wanna lose any command i have
					command = ca.getCommand();
					Share::chain.erase(Share::chain.begin() + i);
					i--;
				}
			}
		}
		if (!command)
		{
			if (worldModel->getPlayMode() == PM_PLAY_ON)
			{
				if (worldModel->getBody().getUniNum() == 1)
				{
					Goalie goalie = Goalie(worldModel);
					goalie.decide(form);
					command = goalie.getCommand();
				}
				else if (isInPlayers(worldModel->getBody().getUniNum(), "23456"))
				{
					DefensePlan defense = DefensePlan(worldModel);
					defense.decide(form);
					command = defense.getCommand();
				}
				//			else if (isInPlayers(worldModel->getBody().getUniNum(), "678"))
				//			{
				//				Middle middle = Middle(worldModel);
				//				middle.decide(form);
				//				command = middle.getCommand();
				//			}
				else if (isInPlayers(worldModel->getBody().getUniNum(), "789AB"))
				{
					Offense offense = Offense(worldModel);
					offense.decide(form);
					command = offense.getCommand();
				}
			}
			else
			{
				NonPlayOnDecision nonPlayOn = NonPlayOnDecision(worldModel);
				nonPlayOn.decide(form);
				command = nonPlayOn.getCommand();
			}
		}
	}
	else
	{
		form.headForm.tnMode = TNM_SEARCH;
		if (worldModel->getCurCycle() % 2 == 1)
			command = new TurnCommand(100);
		else
			command = new EmptyCommand();
	}
}

Command* BodyDecision::getCommand()
{
	return command;
}
