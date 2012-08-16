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

WorldModelHear::WorldModelHear()
{
	freeKickAdvicerSayCycle = 0;
	lastHearCycle = 0;
}

void WorldModelHear::updateAfterSenseBody()
{
	if (getCurCycle() > lastHearCycle)
	{
		hearedPlanStatus = 0;
		hearedInterceptNum = 0;
		hearedSuggestPassTime = 0;
	}
}

void WorldModelHear::parseHear(const SExpression &exp)
{
	float hearDir;
	unsigned hearCycle;
	unsigned senderNum;
	SExpAtomic *at;
	SExpression *exp2;

	resetHearedVarsBeforeHear();

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
	if ((message[1] == 'A' || message[1] == 'B') &&
		curTime != refereeHearTime + 1)
	{
		hearedHeader = message[1];
		hearedSender = body->getUniNum();
		hearedInterceptNum = SayEncoding::getCharNum(message[8]);
	}
}

void WorldModelHear::teammateHear(float hearDir, unsigned senderNum, const string &message)
{
	lastHearCycle = getCurCycle();
	hearedHeader = message[1];

	goalPlanHeard = false;

	switch (message[1])
	{
	case 'A':
		beforePassTmmHear(message, senderNum);
		break;

	case 'P':
		suggestPassTmmHear(message, senderNum);
		break;

/*	case 'B':
		beforePassRelatedTmmHear(message, senderNum);
		break;

	case 'C':
		suggestPassTmmHear(message, senderNum);
		break;
*/
	case 'Z':
			if (curTime != refereeHearTime + 1)
				routinTmmHear(message, senderNum);
			break;

	case 'Y':
			if (curTime != refereeHearTime + 1)
				routinTmmHearWithoutVel(message, senderNum);
			break;

	case 'F':
			if (curTime != refereeHearTime + 1)
				freeKickKickerHear(message, senderNum);
			break;

	case 'R':
		if (curTime != refereeHearTime + 1)
			radarHear(message, senderNum);
		break;

	case 'G':
			if (curTime != refereeHearTime + 1)
				goalPlanHear(message, senderNum);
			break;

	default:
			assert(0);
	}

	logObjects();
}

void WorldModelHear::opponentHear(float hearDir, const string &message)
{
}

void WorldModelHear::routinTmmHear(const std::string &message,
		unsigned senderNum)
{
	float x, y, velMag, velDir;

	if (SayEncoding::getCharNum(message[8]) >= SAY_UPPER_CASE_NUM &&
		(!isBallKickable() || ball->getSeePosDeviation() > 10)) // It means sender now has the ball.
	{
		// Updating the ball
		SayEncoding::decodeObjectPosition(message, 2, x, y);
		SayEncoding::decodeObjectVelocity(message, 5, velMag, velDir);

		ball->updateByHear(x, y, velMag, velDir, *body);

		// Updating the sender
		fullPlayers[TID_TEAMMATE][senderNum - 1]->updateByHear(x, y, velMag, velDir, *body);
	}

	else if (((ball->getSeePosDeviation() > 2 && !isBallKickable()) ||
			ball->getSeePosDeviation() > 10) &&
			ball->isUpdateByHearPermitted())
	{
		// Updating the ball
		SayEncoding::decodeObjectPosition(message, 2, x, y);
		SayEncoding::decodeObjectVelocity(message, 5, velMag, velDir);

		ball->updateByHear(x, y, velMag, velDir, *body);

		// Updating owner
		int owner = SayEncoding::getCharNum(message[10]);
		if (owner != 0)
		{
			if (owner <= 11)
				fullPlayers[TID_TEAMMATE][owner - 1]->
					updateByHear(x, y, velMag, velDir, *body);
			else
				fullPlayers[TID_OPPONENT][owner -11 - 1]->
					updateByHear(x, y, velMag, velDir, *body);
		}
	}

	if (SayEncoding::getCharNum(message[8]) >= SAY_UPPER_CASE_NUM)
		hearedInterceptNum = SayEncoding::getCharNum(message[8]) -
				SAY_UPPER_CASE_NUM;
	else
		hearedInterceptNum = SayEncoding::getCharNum(message[8]);

	hearedSender = senderNum;
	planStatusSender = hearedSender;
	hearedPlanStatus = SayEncoding::getCharNum(message[9]);
}

void WorldModelHear::routinTmmHearWithoutVel(const std::string &message,
		unsigned senderNum)
{
	float x, y;

	if (SayEncoding::getCharNum(message[5]) >= SAY_UPPER_CASE_NUM &&
		(!isBallKickable() || ball->getSeePosDeviation() > 10)) // It means sender now has the ball.
	{
		// Updating the ball
		SayEncoding::decodeObjectPosition(message, 2, x, y);

		ball->updateByHear(x, y,
			fullPlayers[TID_TEAMMATE][senderNum - 1]->getVel().getMagnitude(),
			fullPlayers[TID_TEAMMATE][senderNum - 1]->getVel().getDirection(), *body);

		// Updating the sender
		fullPlayers[TID_TEAMMATE][senderNum - 1]->updateByHear(x, y,
			fullPlayers[TID_TEAMMATE][senderNum - 1]->getVel().getMagnitude(),
			fullPlayers[TID_TEAMMATE][senderNum - 1]->getVel().getDirection(), *body);
	}

	else if (((ball->getSeePosDeviation() > 2 && !isBallKickable()) ||
			  ball->getSeePosDeviation() > 10) &&
			 ball->isUpdateByHearPermitted())
	{
		// Updating the ball
		SayEncoding::decodeObjectPosition(message, 2, x, y);

		ball->updateByHear(x, y, ball->getVel().getMagnitude(),
				ball->getVel().getDirection(), *body);
	}

	if (SayEncoding::getCharNum(message[5]) >= SAY_UPPER_CASE_NUM)
		hearedInterceptNum = SayEncoding::getCharNum(message[5]) -
				SAY_UPPER_CASE_NUM;
	else
		hearedInterceptNum = SayEncoding::getCharNum(message[5]);

	hearedSender = senderNum;
	planStatusSender = hearedSender;
	hearedPlanStatus = SayEncoding::getCharNum(message[6]);
}

void WorldModelHear::setDefensePlanHear(const std::string &message, unsigned senderNum)
{
	defenseHeared = true;
	defenseHearedTime = 0;
	defenseMessage = message;
}

void WorldModelHear::beforePassTmmHear(const std::string &message, unsigned senderNum)
{
	float x, y, velMag, velDir;

	SayEncoding::decodeObjectPosition(message, 2, x, y);
	SayEncoding::decodeObjectVelocity(message, 5, velMag, velDir);

	hearedSender = senderNum;
	hearedInterceptNum = SayEncoding::getCharNum(message[8]);
	ball->updateByHear(x, y, velMag, velDir, *body);
}

/*void WorldModelHear::beforePassRelatedTmmHear(const std::string &message,
		unsigned senderNum)
{
	float x, y, velMag, velDir;

	SayEncoding::decodeObjectPosition(message, 2, x, y);
	SayEncoding::decodeObjectVelocity(message, 5, velMag, velDir);

	hearedSender = senderNum;
	hearedInterceptNum = SayEncoding::getCharNum(message[8]);

	unsigned num = hearedInterceptNum;
	if (num > 11)
		num -= 11;

	if (fullPlayers[TID_TEAMMATE][num - 1]->isValid())
		ball->updateByHear(
			fullPlayers[TID_TEAMMATE][num - 1]->getPos().getX() + x,
			fullPlayers[TID_TEAMMATE][num - 1]->getPos().getY() + y,
			velMag, velDir, *body);
}
*/
void WorldModelHear::suggestPassTmmHear(const std::string &message, unsigned senderNum)
{
	float magnitude, direction, weight;

	SayEncoding::decodeObjectVelocity(message, 2, magnitude, direction);
	SayEncoding::decodeWeight(message, 5, weight, -50, 150);

	hearedSender = senderNum;
	hearedSuggestPassNum = SayEncoding::getCharNum(message[8]);
	hearedSuggestPassDir = direction;
	hearedSuggestPassSpeed = magnitude;
	hearedSuggestPassWeight = weight;

	hearedSuggestPassTime = curTime;
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

void WorldModelHear::freeKickKickerHear(const std::string &message, unsigned senderNum)
{
	unsigned adviceNumber;

	adviceNumber = SayEncoding::getCharNum(message[2]);

	LOG << "WorldModelHear::freeKickAdvicerHear sender=" << senderNum <<
		" adviceNumber=" << adviceNumber << endl;

	freeKickAdvicerSayCycle = curTime;
	freeKickAdvicerSayNumber = adviceNumber;
}

void WorldModelHear::radarHear(const std::string &message, unsigned senderNum)
{
	LOG << "WorldModelHear::radarHear" << endl;

	if (getBall().getPosDeviation() > 3 ||
		getBall().getAbsVec().getMagnitude() > 7)
		return;

	unsigned id;
	float magnitude, direction;

	for (int i = 0; message[3 * i + 2] != '"'; i++)
	{
		SayEncoding::decodeRadarObject(message, (unsigned)(3 * i + 2),
				id, magnitude, direction);

		LOG << "\tPlayer " << i << " ID: " << id
			<< " Mag: " << magnitude	<< " Dir: " << direction << endl;

		TeamId teamId = (TeamId)(int)(id / 11);
		int uniNum = (id % 11) + 1;

		setFullPlayer(teamId, uniNum - 1).
				updateByRadar(magnitude, direction, getBody(), getBall());

		if (i == 0) // updating view parts
		{
			for (int j = 0; j < VIEW_PARTS_NUM; j++)
			{
				float deltaAngle = abs(getDeltaAngle(
					getFullPlayer(teamId, uniNum - 1).getAbsVec().getDirection(),
					getViewPartDir(j)));

				if (deltaAngle <= (VIEW_PART_SIZE / 2) - 2.5)
				{
					viewPartCycles[j] = 1;
					break;
				}
			}
		}
	}

	hearedSender = senderNum;
}

void WorldModelHear::goalPlanHear(const std::string &message,
		unsigned senderNum)
{
	LOG << "WorldModelHear::goalPlanHear" << endl;

	goalPlanHeard = true;
	goalPlanMessage = message;
}

void WorldModelHear::resetHearedVarsBeforeHear()
{
	hearedHeader = '\0';
	hearedSender = 0;
}

// Getting functions
unsigned WorldModelHear::getFreeKickAdvicerSayNumber() const
{
	return freeKickAdvicerSayNumber;
}

unsigned WorldModelHear::getFreeKickAdvicerSayCycle() const
{
	return freeKickAdvicerSayCycle;
}

char WorldModelHear::getHearedHeader() const
{
	return hearedHeader;
}

unsigned WorldModelHear::getHearedSender() const
{
	return hearedSender;
}

unsigned WorldModelHear::getHearedInterceptNum() const
{
	return hearedInterceptNum;
}

unsigned WorldModelHear::getHearedPlanStatus() const
{
	return hearedPlanStatus;
}

int WorldModelHear::getPlanStatusSender() const
{
	return planStatusSender;
}

unsigned WorldModelHear::getHearedSuggestPassNum() const
{
	return hearedSuggestPassNum;
}

unsigned WorldModelHear::getHearedSuggestPassTime() const
{
	return hearedSuggestPassTime;
}

float WorldModelHear::getHearedSuggestPassDir() const
{
	return hearedSuggestPassDir;
}

float WorldModelHear::getHearedSuggestPassSpeed() const
{
	return hearedSuggestPassSpeed;
}

float WorldModelHear::getHearedSuggestPassWeight() const
{
	return hearedSuggestPassWeight;
}

unsigned WorldModelHear::getOurPenaltyKicker() const
{
	return ourPenaltyKicker;
}

