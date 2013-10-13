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
 *  	and is modified by: Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <cassert>
#include <iostream>

#include <Logger.h>
#include <Defines.h>
#include <Basics.h>
#include <Degree.h>
#include <SayEncoding.h>
#include <WorldModelHear.h>
#include <DecideCommonTypes.h>

using namespace std;
using namespace Basics;
using namespace Degree;

#define LOGH LOG

WorldModelHear::WorldModelHear()
{
	lastHearCycle = 0;
}

void WorldModelHear::updateAfterSenseBody()
{
	if (getCurCycle() > lastHearCycle)
	{
	}
}

void WorldModelHear::parseHear(const SExpression &exp)
{
	float hearDir;
	unsigned hearCycle;
	unsigned senderNum;
	SExpAtomic *at;
	SExpression *exp2;

	at = dynamic_cast<SExpAtomic *>(exp[1]);
	assert(at);
	hearCycle = at->asInt();

	at = dynamic_cast<SExpAtomic *>(exp[2]);
	assert(at);

	if (at->toString() == "referee")
	{
		parseRefereeHear(exp);
		refereeHearTime = curTime;
	}
	else if (at->toString().substr(0, 12) == "online_coach")
	{
		if ((fieldSide == SI_LEFT && at->toString().substr(13, 4) == "left") ||
			(fieldSide == SI_RIGHT && at->toString().substr(13, 5) == "right"))
		{ // teammate coach.
			exp2 = dynamic_cast<SExpression *>(exp[3]);
			assert(exp2);
			parseTeammateCoachHear(*exp2);
		}
		else
		{ // opponent coach.
		}
	}
	else if (at->toString() == "self")
	{
		at = dynamic_cast<SExpAtomic *>(exp[3]);
		assert(at);
		selfHear(at->toString());
	}
	else // a player has said that.
	{
		at = dynamic_cast<SExpAtomic *>(exp[2]);
		assert(at);
		hearDir = at->asInt();

		at = dynamic_cast<SExpAtomic *>(exp[3]);
		assert(at);
		if (at->toString() == "our")
		{
			at = dynamic_cast<SExpAtomic *>(exp[4]);
			assert(at);
			senderNum = at->asInt();

			at = dynamic_cast<SExpAtomic *>(exp[5]);
			assert(at);
			teammateHear(hearDir, senderNum, at->toString());
		}
		else
		{
			assert(at->toString() == "opp");

			at = dynamic_cast<SExpAtomic *>(exp[4]);
			assert(at);
			opponentHear(hearDir, at->toString());
		}
	}
}

void WorldModelHear::parseRefereeHear(const SExpression &exp)
{
	SExpAtomic *at;

	at = dynamic_cast<SExpAtomic *>(exp[3]);
	assert(at);

	playModeRemainCycle = 0;

	LOG << "refereeHear : " << at->toString() << endl;
	// normal PlayModes
	if (at->toString() != "play_on")
	{
		ball->setVel() = Vector(0.0,0.0);
	}

	if (at->toString() == "before_kick_off")
		playMode = PM_BEFORE_KICK_OFF;

	else if (at->toString() == "play_on")
	{
		playMode = PM_PLAY_ON;
		virtualPlayMode = VPM_NONE;
		playOnStartCycle = curTime;
	}

	else if (at->toString() == "kick_off_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_KICK_OFF;
		else
			playMode = PM_KICK_OFF_OPP;

		playModeRemainCycle = 200;

		ball->updateByHear(0, 0, 0, 0, *body);
	}
	else if (at->toString() == "kick_off_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_KICK_OFF;
		else
			playMode = PM_KICK_OFF_OPP;

		playModeRemainCycle = 200;
		ball->updateByHear(0, 0, 0, 0, *body);
	}

	else if (at->toString() == "free_kick_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_FREE_KICK;
		else
			playMode = PM_FREE_KICK_OPP;

		playModeRemainCycle = 200;
	}
	else if (at->toString() == "free_kick_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_FREE_KICK;
		else
			playMode = PM_FREE_KICK_OPP;

		playModeRemainCycle = 200;
	}

	else if (at->toString() == "indirect_free_kick_l")
	{
		if (fieldSide == SI_LEFT)
		{
			playMode = PM_FREE_KICK;
			virtualPlayMode = VPM_INDIRECT_FREE_KICK;
		}
		else
		{
			playMode = PM_FREE_KICK_OPP;
			virtualPlayMode = VPM_INDIRECT_FREE_KICK_OPP;
		}

		playModeRemainCycle = 200;
	}
	else if (at->toString() == "indirect_free_kick_r")
	{
		if (fieldSide == SI_RIGHT)
		{
			playMode = PM_FREE_KICK;
			virtualPlayMode = VPM_INDIRECT_FREE_KICK;
		}
		else
		{
			playMode = PM_FREE_KICK_OPP;
			virtualPlayMode = VPM_INDIRECT_FREE_KICK_OPP;
		}

		playModeRemainCycle = 200;
	}

	else if (at->toString() == "kick_in_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_KICK_IN;
		else
			playMode = PM_KICK_IN_OPP;

		playModeRemainCycle = 200;
	}
	else if (at->toString() == "kick_in_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_KICK_IN;
		else
			playMode = PM_KICK_IN_OPP;

		playModeRemainCycle = 200;
	}

	else if (at->toString() == "offside_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_OFFSIDE;
		else
			playMode = PM_OFFSIDE_OPP;

		playModeRemainCycle = 30;
	}
	else if (at->toString() == "offside_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_OFFSIDE;
		else
			playMode = PM_OFFSIDE_OPP;

		playModeRemainCycle = 30;
	}

	else if (at->toString() == "foul_charge_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_FOUL_CHARGE;
		else
			playMode = PM_FOUL_CHARGE_OPP;
	}
	else if (at->toString() == "foul_charge_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_FOUL_CHARGE;
		else
			playMode = PM_FOUL_CHARGE_OPP;
	}

	else if (at->toString() == "goal_kick_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_GOAL_KICK;
		else
			playMode = PM_GOAL_KICK_OPP;

		virtualPlayMode = VPM_NONE;
		playModeRemainCycle = 200;
	}
	else if (at->toString() == "goal_kick_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_GOAL_KICK;
		else
			playMode = PM_GOAL_KICK_OPP;

		virtualPlayMode = VPM_NONE;
		playModeRemainCycle = 200;
	}

	else if (at->toString() == "corner_kick_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_CORNER_KICK;
		else
			playMode = PM_CORNER_KICK_OPP;

		playModeRemainCycle = 200;
	}
	else if (at->toString() == "corner_kick_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_CORNER_KICK;
		else
			playMode = PM_CORNER_KICK_OPP;

		playModeRemainCycle = 200;
	}

	else if (at->toString() == "free_kick_fault_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_FREE_KICK_FAULT;
		else
			playMode = PM_FREE_KICK_FAULT_OPP;

		playModeRemainCycle = 30;
	}
	else if (at->toString() == "free_kick_fault_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_FREE_KICK_FAULT;
		else
			playMode = PM_FREE_KICK_FAULT_OPP;

		playModeRemainCycle = 30;
	}

	else if (at->toString() == "catch_fault_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_CATCH_FAULT;
		else
			playMode = PM_CATCH_FAULT_OPP;

		playModeRemainCycle = 30;
	}
	else if (at->toString() == "catch_fault_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_CATCH_FAULT;
		else
			playMode = PM_CATCH_FAULT_OPP;

		playModeRemainCycle = 30;
	}

	else if (at->toString() == "penalty_setup_l")
	{
		if (fieldSide == SI_LEFT)
		{
			playMode = PM_PENALTY_SETUP;

			// determine our kicker
			ourPenaltyKicker--;
			if (ourPenaltyKicker < 1 or ourPenaltyKicker > 11)
				ourPenaltyKicker = 11;
			// --------------------
		}
		else
			playMode = PM_PENALTY_SETUP_OPP;

		playModeRemainCycle = 100;
		virtualPlayMode = VPM_NONE;
	}
	else if (at->toString() == "penalty_setup_r")
	{
		if (fieldSide == SI_RIGHT)
		{
			playMode = PM_PENALTY_SETUP;

			// determine our kicker
			ourPenaltyKicker--;
			if (ourPenaltyKicker < 1 or ourPenaltyKicker > 11)
				ourPenaltyKicker = 11;
			// --------------------
		}
		else
			playMode = PM_PENALTY_SETUP_OPP;

		playModeRemainCycle = 100;
		virtualPlayMode = VPM_NONE;
	}

	else if (at->toString() == "penalty_ready_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_PENALTY_READY;
		else
			playMode = PM_PENALTY_READY_OPP;

		playModeRemainCycle = 50;
		virtualPlayMode = VPM_NONE;
	}
	else if (at->toString() == "penalty_ready_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_PENALTY_READY;
		else
			playMode = PM_PENALTY_READY_OPP;

		playModeRemainCycle = 50;
		virtualPlayMode = VPM_NONE;
	}

	else if (at->toString().substr(0, 6) == "goal_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_GOAL;
		else
			playMode = PM_GOAL_OPP;

		playModeRemainCycle = 50;
	}
	else if (at->toString().substr(0, 6) == "goal_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_GOAL;
		else
			playMode = PM_GOAL_OPP;

		playModeRemainCycle = 50;
	}

	// virtual PlayModes
	else if (at->toString() == "drop_ball")
	{
		virtualPlayMode = VPM_DROP_BALL;
		ball->kill();
	}

	else if (at->toString() == "time_up_without_a_team")
		playMode = PM_TIME_UP_WITHOUT_A_TEAM;

	else if (at->toString() == "time_up")
		playMode = PM_TIME_UP;

	else if (at->toString() == "time_over")
		playMode = PM_TIME_UP;

	else if (at->toString() == "half_time")
		virtualPlayMode = VPM_HALF_TIME;

	else if (at->toString() == "time_extended")
		virtualPlayMode = VPM_TIME_EXTENDED;

	else if (at->toString() == "penalty_draw")
		virtualPlayMode = VPM_PENALTY_DRAW;

	else if (at->toString() == "goalie_catch_ball_l")
	{
		if (fieldSide == SI_LEFT)
			virtualPlayMode = VPM_GOALIE_CATCH_BALL;
		else
			virtualPlayMode = VPM_GOALIE_CATCH_BALL_OPP;
	}
	else if (at->toString() == "goalie_catch_ball_r")
	{
		if (fieldSide == SI_RIGHT)
			virtualPlayMode = VPM_GOALIE_CATCH_BALL;
		else
			virtualPlayMode = VPM_GOALIE_CATCH_BALL_OPP;
	}

	else if (at->toString() == "foul_l")
	{
		if (fieldSide == SI_LEFT)
			virtualPlayMode = VPM_FOUL;
		else
			virtualPlayMode = VPM_FOUL_OPP;
	}
	else if (at->toString() == "foul_r")
	{
		if (fieldSide == SI_RIGHT)
			virtualPlayMode = VPM_FOUL;
		else
			virtualPlayMode = VPM_FOUL_OPP;
	}

	else if (at->toString() == "back_pass_l")
	{
		if (fieldSide == SI_LEFT)
			playMode = PM_BACK_PASS;
		else
			playMode = PM_BACK_PASS_OPP;
	}
	else if (at->toString() == "back_pass_r")
	{
		if (fieldSide == SI_RIGHT)
			playMode = PM_BACK_PASS;
		else
			playMode = PM_BACK_PASS_OPP;
	}

	else if (at->toString() == "penalty_onfield_l")
	{
		if (fieldSide == SI_LEFT)
			virtualPlayMode = VPM_PENALTY_ON_FIELD;
		else
			virtualPlayMode = VPM_PENALTY_ON_FIELD_OPP;
	}
	else if (at->toString() == "penalty_onfield_r")
	{
		if (fieldSide == SI_RIGHT)
			virtualPlayMode = VPM_PENALTY_ON_FIELD;
		else
			virtualPlayMode = VPM_PENALTY_ON_FIELD_OPP;
	}

	else if (at->toString() == "penalty_miss_l")
	{
		if (fieldSide == SI_LEFT)
			virtualPlayMode = VPM_PENALTY_MISS;
		else
			virtualPlayMode = VPM_PENALTY_MISS_OPP;
	}
	else if (at->toString() == "penalty_miss_r")
	{
		if (fieldSide == SI_RIGHT)
			virtualPlayMode = VPM_PENALTY_MISS;
		else
			virtualPlayMode = VPM_PENALTY_MISS_OPP;
	}

	else if (at->toString() == "penalty_score_l")
	{
		if (fieldSide == SI_LEFT)
			virtualPlayMode = VPM_PENALTY_SCORE;
		else
			virtualPlayMode = VPM_PENALTY_SCORE_OPP;
	}
	else if (at->toString() == "penalty_score_r")
	{
		if (fieldSide == SI_RIGHT)
			virtualPlayMode = VPM_PENALTY_SCORE;
		else
			virtualPlayMode = VPM_PENALTY_SCORE_OPP;
	}

	else if (at->toString() == "penalty_winner_l")
	{
		if (fieldSide == SI_LEFT)
			virtualPlayMode = VPM_PENALTY_WINNER;
		else
			virtualPlayMode = VPM_PENALTY_WINNER_OPP;
	}
	else if (at->toString() == "penalty_winner_r")
	{
		if (fieldSide == SI_RIGHT)
			virtualPlayMode = VPM_PENALTY_WINNER;
		else
			virtualPlayMode = VPM_PENALTY_WINNER_OPP;
	}

	else if (at->toString() == "penalty_foul_l")
	{
		if (fieldSide == SI_LEFT)
			virtualPlayMode = VPM_PENALTY_FOUL;
		else
			virtualPlayMode = VPM_PENALTY_FOUL_OPP;
	}
	else if (at->toString() == "penalty_foul_r")
	{
		if (fieldSide == SI_RIGHT)
			virtualPlayMode = VPM_PENALTY_FOUL;
		else
			virtualPlayMode = VPM_PENALTY_FOUL_OPP;
	}

	else if (at->toString() == "penalty_taken_l")
	{
		if (fieldSide == SI_LEFT)
			virtualPlayMode = VPM_PENALTY_TAKEN;
		else
			virtualPlayMode = VPM_PENALTY_TAKEN_OPP;
	}
	else if (at->toString() == "penalty_taken_r")
	{
		if (fieldSide == SI_RIGHT)
			virtualPlayMode = VPM_PENALTY_TAKEN;
		else
			virtualPlayMode = VPM_PENALTY_TAKEN_OPP;
	}

	else
	{
		// card takes
		const char* msg = at->toString().c_str();
		bool card_taked = false;
		int num;
		for (int i = 1; i <= 11; i++)
			LOG << i << " : " << fullPlayers[TID_TEAMMATE][i-1]->getCard() << endl;
		for (int i = 1; i <=11; i++)
			if (sscanf(msg, "yellow_card_l_%d", &num) == 1)
			{
				if (num != i)
					continue;
				card_taked = true;
				if (fieldSide == SI_LEFT)
					fullPlayers[TID_TEAMMATE][i-1]->setCard(CM_YELLOW);
				else
					fullPlayers[TID_OPPONENT][i-1]->setCard(CM_YELLOW);
			}
			else if (sscanf(msg, "yellow_card_r_%d", &num) == 1)
			{
				if (num != i)
					continue;
				card_taked = true;
				if (fieldSide == SI_RIGHT)
					fullPlayers[TID_TEAMMATE][i-1]->setCard(CM_YELLOW);
				else
					fullPlayers[TID_OPPONENT][i-1]->setCard(CM_YELLOW);
			}

			else if (sscanf(msg, "red_card_l_%d", &num) == 1)
			{
				if (num != i)
					continue;
				card_taked = true;
				if (fieldSide == SI_LEFT)
					fullPlayers[TID_TEAMMATE][i-1]->kill();
				else
					fullPlayers[TID_OPPONENT][i-1]->kill();
			}
			else if (sscanf(msg, "red_card_r_%d", &num) == 1)
			{
				if (num != i)
					continue;
				card_taked = true;
				if (fieldSide == SI_RIGHT)
					fullPlayers[TID_TEAMMATE][i-1]->kill();
				else
					fullPlayers[TID_OPPONENT][i-1]->kill();
			}


		if (!card_taked)
			assert(0);
	}
}

void WorldModelHear::selfHear(const std::string &message)
{
}

void WorldModelHear::teammateHear(float hearDir, unsigned senderNum, const string &message)
{
	lastHearCycle = getCurCycle();
	hearedSender = senderNum;
	string puremsg = message;
	puremsg.erase(0, 1);
	puremsg.erase(puremsg.size() - 1, 1); // removing " characters

	unsigned long long code = SayCodec::strToNum(puremsg);

	LOGH << "Message = " << puremsg << " Code = " << code << endl;
	CodeItem flag(ST_SIZE);
	while (code > 0)
	{

		flag.decode(code);
		LOGH << "heard --> flag = " << flag << endl;

		switch (flag)
		{
		case ST_PASS:
			hearPass(code);
			break;
		default:
			LOGH << "Invalid message????" << endl;
			return;
		};
	}
	logObjects();
}

void WorldModelHear::opponentHear(float hearDir, const string &message)
{
}

void WorldModelHear::parseTeammateCoachHear(const SExpression &exp)
{
	string messageString;
	string expString;
	unsigned dummyPtr = 0;

	int i, j;
	int typeNum;

	SExpression *exp2;
	SExpression *exp3;
	SExpAtomic *at;
	SExpAtomic *at2;

	LOG << "WorldModelHear::parseTeammateCoachHear" << endl;

	if (dynamic_cast<const SExpAtomic *>(exp[0])->toString() == "freeform")
	{
		expString = dynamic_cast<const SExpAtomic *>(exp[1])->toString();
		expString = "(" + expString.substr(1, expString.length() - 2) + ")";
		exp2 = new SExpression(expString, dummyPtr);
		for (i = 0; i < exp2->size(); i++)
		{
			exp3 = dynamic_cast<SExpression *>((*exp2)[i]);
			at = dynamic_cast<SExpAtomic *>((*exp3)[0]);
			assert(at);

			// Opponent Types (T)
			if (at->toString() == "T")
			{
				at2 = dynamic_cast<SExpAtomic *>((*exp3)[1]);
				assert(at2);
				messageString = at2->toString();
				assert(messageString.length() == 11);
				for (j = 0; j < 11; j++)
				{
					typeNum = messageString[j] - '0';
					setFullPlayer(TID_OPPONENT, j).setType(typeNum,	playerTypes[typeNum]);
					LOG << "\tAOTR: Opp Num " << j + 1 << " setted to type " << typeNum << endl;
				}
			}
		}
	}
}

unsigned WorldModelHear::getOurPenaltyKicker() const
{
	return ourPenaltyKicker;
}


void WorldModelHear::hearPass(unsigned long long int& code)
{
	saidVars.passVel.decode(code);
	saidVars.passDir.decode(code);
	saidVars.passBallPos.decode(code);
	saidVars.passReceiver.decode(code);
	saidVars.passCycle.decode(code);

	heardPassSender = hearedSender;
	if (getBody().getUniNum() == heardPassSender)
		return;
	lastPassHearCycle = curCycle;
	LOGH << "Player " << getBody().getUniNum() << " Is Hearing A Pass :" << endl;
	heardPassCycle = heardPassTime = saidVars.passCycle;
	heardPassCycle += getCurCycle();
	heardPassTime += getCurTime();
	LOGH << "Hearing Pass Cycle : " << heardPassCycle << endl;
	heardPassReceiver = saidVars.passReceiver;
	LOGH << "Hearing Pass Receiver : " << heardPassReceiver << endl;
	Vector heardPassBallPos;
	float heardPassDir;
	float heardPassVel;
	heardPassBallPos = saidVars.passBallPos;
	LOGH << "Hearing Ball Pos : " << heardPassBallPos << endl;
	heardPassDir = saidVars.passDir;
	LOGH << "Hearing Pass Angle : " << heardPassDir << endl;
	heardPassVel = saidVars.passVel;
	LOGH << "Hearing Pass Velocity : " << heardPassVel << endl;
//	LOGH<<"Hearing Completed"<<endl;
	heardPassBall = getBall();
	heardPassBall.setPos() = heardPassBallPos;
	heardPassBall.setVel().setAsPolar(heardPassVel, heardPassDir);
	int cycle = int(getCurCycle()) - int(heardPassCycle); //may be bigger than getCurCycle() because of Multistepkick
	LOGH << "HEAR***** sim : " << ball->getSimCounter() << " the other : " << cycle << endl;

	if (cycle == 0) //agar baraye cycle haye ayande bashe to pass positioning set mishe
	{
		ball->updateByHear(heardPassBallPos.getX(), heardPassBallPos.getY(), heardPassVel,
				heardPassDir, getBody());
//		ball->setPos() = heardPassBallPos;
//		ball->setVel().setAsPolar(heardPassVel, heardPassDir);
		for (int i = 0; i < cycle; i++)
			ball->simulateByDynamics(getBody());
		ball->setSimCounter(max(0, cycle));
		LOGH << "Heard A Pass , Setting Ball By Pass : Vel : (" << heardPassVel << ','
				<< heardPassDir << ") Pos : " << ball->getPos() << endl;
	}
}
