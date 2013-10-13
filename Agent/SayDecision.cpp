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
#include <cmath>
#include <string>
#include <cassert>

#include <WorldModel.h>
#include <Logger.h>
#include <Degree.h>
#include <Command.h>
#include <Defines.h>
#include <SayEncoding.h>
#include <SayCodec.h>
#include <SayDecision.h>
#include <Basics.h>

using namespace std;
using namespace Degree;

#define LOGS LOG

const unsigned long long MAXBASE = 4923990397355877376ull;

// class SayDecision

SayDecision::SayDecision(const WorldModel *worldModel) :
		worldModel(worldModel)
{
}

SayDecision::~SayDecision()
{
}

void SayDecision::decide(const Command *bodyCycleCommand, const SayDecisionForm &sayForm)
{
	LOG << "SayDecision::decide" << endl;
	LOG << "MAXBASE IS \t\t" << MAXBASE << endl;
	curCode = CodeItem();
	for (vector<SayType>::const_iterator i = sayForm.says.begin(); i != sayForm.says.end(); i++)
	{
		switch (*i)
		{
		case ST_PASS:
			sayPass(bodyCycleCommand, sayForm);
			break;
		default:
			break;
		}
		LOG << " Current Base = " << curCode.base << endl;
	}

	if (curCode.base > 1)
	{
		string realMessage;
		SayCodec::numToStr(curCode.number, realMessage, 10);
		sayCommand = new SayCommand(realMessage);
	}
	else
		sayCommand = new EmptyCommand();
	if (sayForm.attentionNum)
		attentionCommand = new AttentionToCommand(TID_TEAMMATE, sayForm.attentionNum);
	else
		attentionCommand = new AttentionOffCommand();
}

Command *SayDecision::getSayCommand()
{
	return sayCommand;
}

Command *SayDecision::getAttentionCommand()
{
	return attentionCommand;
}

void SayDecision::addSayCode(CodeItem& code)
{
	if (MAXBASE / curCode.base <= code.base)
	{
		LOG << "Say message is POR!!! you can't add this message!" << endl;
		return;
	}
	code.encode(curCode);
	LOG << "Code added" << endl;
}

void SayDecision::sayPass(const Command *bodyCycleCommand, const SayDecisionForm &sayForm)
{
	LOGS << "SAY PASS " << endl;
	CodeItem finalCode;
	sayForm.vars.passCycle.encode(finalCode);
	sayForm.vars.passReceiver.encode(finalCode);
	sayForm.vars.passBallPos.encode(finalCode);
	sayForm.vars.passDir.encode(finalCode);
	sayForm.vars.passVel.encode(finalCode);
	CodeItem(ST_SIZE, ST_PASS).encode(finalCode);
	addSayCode(finalCode);
}
