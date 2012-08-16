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
 *  	and is modified by: Pooria Kaviani, Mohammad Salehe, Mostafa Rokooey, Mehrdad Bakhtiari,
 *  						Sassan Haradji, Darioush Jalali, Mohammad Zareie
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <sstream>
#include <cassert>
#include <iostream>

#include <Config.h>
#include <Logger.h>
#include <Basics.h>
#include <Degree.h>
#include <Defines.h>
#include <WorldModel.h>
#include <SayEncoding.h>

using namespace std;
using namespace Basics;
using namespace Degree;

// class WorldModel
WorldModel::WorldModel() : fastICInitialized(false)
{
	if (not formationSet)
	{
		formation = Formation("./Configs/PlayOnPositioning.conf");
		beforeKickOffFormation = Formation("./Configs/BeforeKickOffPositioning.conf");
		nonPlayOnFormation = Formation("./Configs/NonPlayOnPositioning.conf");
		LOG << "HALAI: " << beforeKickOffFormation.getExactPosition(1, Point(100, 100)) << endl;;
		formationSet = true;
	}
}

WorldModel::~WorldModel()
{
}

void WorldModel::updateAfterSenseBody()
{
	WorldModelHear::updateAfterSenseBody();
}

void WorldModel::updateFastIC()
{
	LOG << "Updating global fastIC" << endl;

	if (not fastICInitialized)
	{
	    fastIC = FastIC(this, true);
	    fastICInitialized = true;
	}
//	fastIC.setTeammateCount(1);
//	fastIC.setOpponentCount(1);
    LOG << "FFIRST" << endl;
	fastIC.setByWorldModel();
    LOG << "FFSECOND" << endl;
	fastIC.setCalculateSelf(true);
    LOG << "FFTHIRD" << endl;
	fastIC.refresh();
	LOG << "FF4THIRD" << endl;
	fastIC.setMaxCycles(60);
	LOG << "FF5THIRD" << endl;
	fastIC.calculate();
	LOG << "FF6THIRD" << endl;
	//LOG << "My reach cycle(FastIC) = " << fastIC.getSelfReachCycle() << endl;
	//fastIC.logFastests();
}

const FastIC &WorldModel::getGlobalFastIC() const
{
	return (const FastIC &)fastIC;
}
