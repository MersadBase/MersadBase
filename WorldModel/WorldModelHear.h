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
#include <SayCodec.h>

class WorldModelHear: public WorldModelUtilities
{
protected:
	//hear stuff
	unsigned lastHearCycle;
	unsigned lastSelfHearCycle;
	unsigned hearedSender;
	unsigned lastPassHearCycle;

	//ST_PASS
	unsigned heardPassSender;
	unsigned heardPassCycle;
	unsigned heardPassTime;
	unsigned heardPassReceiver;
	Ball heardPassBall;

	SayHearVars saidVars;
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

	///////////////////////////////////////////////////////////////
	void hearPass(unsigned long long int& code);
	///////////////////////////////////////////////////////////////

	unsigned getOurPenaltyKicker() const;

};

#endif // __HEAR_H
