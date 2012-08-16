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
 *  Released on Wednesday 25 August 2010, 3 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <AdvancedCoach.h>
#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>

#include <Logger.h>
#include <Defines.h>
#include <Basics.h>

#include "TeamLogo.xpm"

using namespace std;

unsigned AdvancedCoach::lastSubstituteCycle(0);
vector<unsigned> AdvancedCoach::usedTypes;

unsigned AdvancedCoach::remainSubstitutes(3);

AdvancedCoach::AdvancedCoach(int argc, char **argv): SyncedCoach(argc, argv)
{
	numSentFreeForm = 0;
	sendChangeType = -1;

        M_team_graphic.createXpmTiles( team_logo_xpm );
}

AdvancedCoach::~AdvancedCoach()
{
}

bool AdvancedCoach::process()
{
	if ( worldModel->getCurCycle() == 0
		and useTeamGraphic
		and M_team_graphic.tiles().size() != teamGraphicOKSet().size() )
	{
		sendTeamGraphic();
	}


	bool areAllPlayersConnected = true;
	for (int i = 0; i < FULL_PLAYERS_NUM; i++)
		if (not worldModel->getFullPlayer(TID_TEAMMATE, i).isAlive())
			areAllPlayersConnected = false;

	worldModel->getServerParam();
	LOG << "AdvancedCoach::process" << endl;

	bool allowSendFreeForm = false;
	string freeFormQueue = "";

	changePlayerCycleCommand = "";

	if (worldModel->getPlayMode() != PM_PLAY_ON)
		allowSendFreeForm = true;
	else
	{
		if (worldModel->getPlayOnPastCycles() > (unsigned)worldModel->getServerParam()["freeform_wait_period"].asInt() and
			worldModel->getPlayOnPastCycles() % worldModel->getServerParam()["freeform_send_period"].asInt() <=
				(unsigned)worldModel->getServerParam()["freeform_wait_period"].asInt())
			allowSendFreeForm = true;
	}

	if ((float)worldModel->getCurCycle() / 6000 * worldModel->getServerParam()["say_coach_cnt_max"].asInt() < numSentFreeForm)
		allowSendFreeForm = false;

	if (allowSendFreeForm and areAllPlayersConnected and sendChangeType < 9)
	{
		sendChangeType++;
		setPlayersType();
		changePlayerCycleCommand = ChangePlayerCommand(sendChangeType + 2, playersType[sendChangeType]).toString();
		LOG << "Change: " << changePlayerCycleCommand << endl;
	}

	LOG << "remainSubstitutes : " << remainSubstitutes << endl;
	// substitution
	if (allowSendFreeForm and worldModel->getPlayMode() != PM_PLAY_ON and worldModel->getCurCycle() > 10 and remainSubstitutes > 0 and lastSubstituteCycle != worldModel->getCurCycle())
	{
		LOG << "i can substitute a player!" << endl;

		for (unsigned i = 0; i < 11; i++)
			if (worldModel->getFullPlayer(TID_TEAMMATE, i).getCard() != CM_NONE)
				if (worldModel->getCurCycle() > 100 and worldModel->getPlayMode() != PM_PLAY_ON)
				{
					substitute(i+1);
					remainSubstitutes--;
					lastSubstituteCycle = worldModel->getCurCycle();
				}
	}
	// end of substitution

	if (allowSendFreeForm and freeFormQueue != "")
	{
		numSentFreeForm++;

		freeFormCycleCommand = new FreeFormCommand(freeFormQueue);
	}
	else
	{
		freeFormCycleCommand = new EmptyCommand();
	}

	return true;
}

void AdvancedCoach::setPlayersType()
{
	map<unsigned, Param> playerParams;
	for (unsigned i = 0; i < 18; i++)
		playerParams[i] = worldModel->getPlayerType(i);

	map<unsigned, Param> playerWeight;
	map<unsigned, Param>::iterator tmpIt;

//	Defense Parameters
	for (unsigned i = 0; i < 4; i++)
	{
		float max = -1;
		for (map<unsigned, Param>::iterator i = playerParams.begin(); i != playerParams.end(); i++)
			if ((*i).second["player_size"].asFloat() + (*i).second["kickable_margin"].asFloat() > max)
			{
				max = (*i).second["player_size"].asFloat() + (*i).second["kickable_margin"].asFloat();
				tmpIt = i;
			}

		playerWeight.insert((*tmpIt));
		playerParams.erase(tmpIt);
	}

	tmpIt = playerWeight.begin();
	playersType[0] = (*tmpIt).first; // Number 2
	tmpIt++;
	playersType[1] = (*tmpIt).first; // Number 3
	tmpIt++;
	playersType[2] = (*tmpIt).first; // Number 4
	tmpIt++;
	playersType[3] = (*tmpIt).first; // Number 5

//	Offensee Parameters
	playerWeight.clear();
	for (unsigned i = 0; i < 3; i++)
	{
		float max = -1;
		for (map<unsigned, Param>::iterator i = playerParams.begin(); i != playerParams.end(); i++)
		{
			float curValue = (*i).second["dash_power_rate"].asFloat() * 100 +
						(*i).second["stamina_inc_max"].asFloat() / 100 -
						(*i).second["player_decay"].asFloat() +
						((*i).second["player_size"].asFloat() + (*i).second["kickable_margin"].asFloat()) * .5f;
			if (curValue > max)
			{
				max = curValue;
				tmpIt = i;
			}
		}

		playerWeight.insert((*tmpIt));
		playerParams.erase(tmpIt);
	}

	tmpIt = playerWeight.begin();
	playersType[7] = (*tmpIt).first; // Number 9
	tmpIt++;
	playersType[8] = (*tmpIt).first; // Number 10
	tmpIt++;
	playersType[9] = (*tmpIt).first; // Number 11
	tmpIt++;

//	Middle Parameters
	playerWeight.clear();
	for (unsigned i = 0; i < 3; i++)
	{
		float max = -1;
		for (map<unsigned, Param>::iterator i = playerParams.begin(); i != playerParams.end(); i++)
		{
			float curValue = (*i).second["dash_power_rate"].asFloat() * 100 +
						(*i).second["stamina_inc_max"].asFloat() / 100 -
						(*i).second["player_decay"].asFloat() +
						(*i).second["player_size"].asFloat() + (*i).second["kickable_margin"].asFloat();
			if (curValue > max)
			{
				max = curValue;
				tmpIt = i;
			}
		}

		playerWeight.insert((*tmpIt));
		playerParams.erase(tmpIt);
	}

	tmpIt = playerWeight.begin();
	playersType[4] = (*tmpIt).first; // Number 6
	tmpIt++;
	playersType[5] = (*tmpIt).first; // Number 7
	tmpIt++;
	playersType[6] = (*tmpIt).first; // Number 8
	tmpIt++;
}

void AdvancedCoach::changePlayerType(const SExpression &exp)
{
	SyncedCoach::changePlayerType(exp);
}

void AdvancedCoach::sendTeamGraphic()
{
    int count = 0;
    for ( TeamGraphic::Map::const_reverse_iterator tile
              = M_team_graphic.tiles().rbegin();
          tile != M_team_graphic.tiles().rend();
          ++tile )
    {
        if ( teamGraphicOKSet().find( tile->first ) == teamGraphicOKSet().end() )
        {
            if ( ! doTeamGraphic( tile->first.first,
                                  tile->first.second,
                                  M_team_graphic ) )
            {
                break;
            }
            ++count;
        }
    }

/*    if ( count > 0 )
    {
        std::cout << config().teamName()
                  << " coach: "
                  << world().time()
                  << " send team_graphic " << count << " tiles"
                  << std::endl;
    }*/
}

bool AdvancedCoach::doTeamGraphic( const int x,
					const int y,
					const TeamGraphic & team_graphic )
{
    static int send_count = 0;
    static int send_time = -1;

    if ( send_time != (signed)worldModel->getCurCycle() )
    {
        send_count = 0;
    }

    send_time = worldModel->getCurCycle();
    ++send_count;

    if ( send_count > 2048 ) // XXX Magic Number
    {
        return false;
    }

    TeamGraphic::Index index( x, y );
    TeamGraphic::Map::const_iterator tile = team_graphic.tiles().find( index );

    if ( tile == team_graphic.tiles().end() )
    {
        std::cerr << " ESKILAS"
                  << " coach: "
                  << " ***WARNING*** The xpm tile ("
                  << x << ',' << y << ") does not found in the team graphic."
                  << std::endl;
        return false;
    }

    std::ostringstream ostr;

    ostr << "(team_graphic (" << x << ' ' << y << ' ';
    tile->second->print( ostr );
    ostr << "))";

	connection->send( ostr.str() );
    return true; //( M_client->sendMessage( ostr.str().c_str() ) > 0 );
}

void AdvancedCoach::substitute(unsigned num)
{
	usedTypes.push_back(worldModel->getFullPlayer(TID_TEAMMATE, num-1).getType());
	string teammate = " ";
	teammate[0] = num + '0';
	if (num == 10)
		teammate = "10";
	else if (num == 11)
		teammate = "11";

	string str = "(change_player_type ";
	str += teammate;
	str += " ";

	unsigned type = 18;
	for (unsigned tmp = 0; tmp < 18; tmp++)
	{
		bool BREAK = false;
		for (unsigned t = 0; t < usedTypes.size(); t++)
			if (usedTypes[t] == tmp)
				BREAK = true;
		for (unsigned i = 0; i < 11; i++)
			if (worldModel->getFullPlayer(TID_TEAMMATE, i).getType() == tmp)
				BREAK = true;
		if (!BREAK)
		{
			type = tmp;
			break;
		}
	}

	string typeStr = " ";
	typeStr = '0' + (type);
	if (type > 9)
	{
		typeStr = "1";
		typeStr += (type-10) + '0';
	}
	str += typeStr;
	str += ")";
	numSentFreeForm++;
	LOG << "-------------> i said substitution <--------------" << endl;
	LOG << "i sent " << str << endl;
	connection->send(str);
}

