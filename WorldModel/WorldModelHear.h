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

#ifndef __HEAR_H
#define __HEAR_H

#include <WorldModelUtilities.h>

class WorldModelHear: public WorldModelUtilities
{
protected:
	unsigned lastHearCycle;

	float hearedSuggestPassDir;
	float hearedSuggestPassSpeed;
	float hearedSuggestPassWeight;
	unsigned hearedSuggestPassNum;

	unsigned freeKickAdvicerSayCycle;
	unsigned freeKickAdvicerSayNumber;

	// Remain until next hear
	char hearedHeader;
	unsigned hearedSender;

	// Remain until next cycl0e
	int planStatusSender;
	unsigned hearedPlanStatus;
	unsigned hearedInterceptNum;
	unsigned hearedSuggestPassTime;

	std::string defenseMessage;
	bool defenseHeared;
	int defenseHearedTime;

	std::string goalPlanMessage;
	bool goalPlanHeard;

	unsigned ourPenaltyKicker;

public:
	WorldModelHear();

	void updateAfterSenseBody();

	virtual void parseHear(const SExpression &exp);
	virtual void parseRefereeHear(const SExpression &exp);
	virtual void parseTeammateCoachHear(const SExpression &exp);
	virtual void selfHear(const std::string &message);
	virtual void teammateHear(float hearDir, unsigned senderNum,
			const std::string &message);
	void opponentHear(float hearDir, const std::string &message);

	void routinTmmHear(const std::string &message, unsigned senderNum);
	void routinTmmHearWithoutVel(const std::string &message, unsigned senderNum);
	void beforePassTmmHear(const std::string &message, unsigned senderNum);
	void freeKickKickerHear(const std::string &messager, unsigned senderNum);
	void radarHear(const std::string &message, unsigned senderNum);
	void setDefensePlanHear(const std::string &message, unsigned senderNum);
	void goalPlanHear(const std::string &message, unsigned senderNum);

	void suggestPassTmmHear(const std::string &message, unsigned senderNum);
//	void beforePassRelatedTmmHear(const std::string &message, unsigned senderNum);

	void resetHearedVarsBeforeHear();

	// Getting functions
	unsigned getFreeKickAdvicerSayCycle() const;
	unsigned getFreeKickAdvicerSayNumber() const;

	char getHearedHeader() const;
	bool getMustAttention() const;
	float getHearedSuggestPassDir() const;
	float getHearedSuggestPassSpeed() const;
	float getHearedSuggestPassWeight() const;
	unsigned getHearedSender() const;
	unsigned getHearedPlanStatus() const;
	unsigned getHearedInterceptNum() const;
	unsigned getHearedSuggestPassNum() const;
	unsigned getHearedSuggestPassTime() const;
	int getPlanStatusSender() const;
	unsigned getOurPenaltyKicker() const;

};

#endif // __HEAR_H
