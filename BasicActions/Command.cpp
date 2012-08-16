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
 *  This file is created by: Mohsen Izadi
 *  	and is modified by: Ahmad Boorghany, Mohammad Salehe, Darioush Jalali, Pooria Kaviani, Mehrdad Bakhtiari
 *
 *  Released on Monday 13 September 2010, 22 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Command.h>
#include <sstream>

#include <Config.h>

using namespace std;


// class Command

Command::Command()
{
}

Command::~Command()
{
}

// class AttentionToCommand

AttentionToCommand::AttentionToCommand(TeamId teamId, int uniformNum): Command(), teamId(teamId),
		uniformNum(uniformNum)
{
}

AttentionToCommand::~AttentionToCommand()
{
}

string AttentionToCommand::toString() const
{
	ostringstream ss;
	ss << "(attentionto " << (teamId == TID_TEAMMATE ? "our " : "opp ")
		<< uniformNum << ")";
	return ss.str();
}

TeamId AttentionToCommand::getTeamId() const
{
	return teamId;
}

int AttentionToCommand::getUniformNum() const
{
	return uniformNum;
}

// class AttentionToCommand

AttentionOffCommand::AttentionOffCommand(): Command()
{
}

AttentionOffCommand::~AttentionOffCommand()
{
}

string AttentionOffCommand::toString() const
{
	return "(attentionto off)";
}

// class ByeCommand

ByeCommand::ByeCommand(): Command()
{
}

ByeCommand::~ByeCommand()
{
}

string ByeCommand::toString() const
{
	return "(bye)";
}


// class CatchCommand

CatchCommand::CatchCommand(float direction): Command(), direction(direction)
{
}

CatchCommand::~CatchCommand()
{
}

string CatchCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(catch " << -direction << ")";
	return ss.str();
}

float CatchCommand::getDirection() const
{
	return direction;
}


// class ChangeViewCommand

ChangeViewCommand::ChangeViewCommand(ViewModeQuality quality,
	ViewModeWidth width): Command(), quality(quality), width(width)
{
}

ChangeViewCommand::~ChangeViewCommand()
{
}

string ChangeViewCommand::toString() const
{
	string res = "(change_view ";
	switch (width)
	{
	case VMW_NARROW:
		res += "narrow ";
		break;
	case VMW_NORMAL:
		res += "normal ";
		break;
	case VMW_WIDE:
		res += "wide ";
		break;
	}
	res += (quality == VMQ_LOW ? "low" : "high");
	res += ")";
	return res;
}

ViewModeQuality ChangeViewCommand::getQuality() const
{
	return quality;
}

ViewModeWidth ChangeViewCommand::getWidth() const
{
	return width;
}


// class DashCommand

DashCommand::DashCommand(float power, float direction): Command(), power(power), direction(direction)
{
}

DashCommand::~DashCommand()
{
}

string DashCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(dash " << power;
	if (direction != 0)
		ss << " " << direction;
	ss << ")";
	return ss.str();
}

float DashCommand::getPower() const
{
	return power;
}

float DashCommand::getDirection() const
{
	return direction;
}

void DashCommand::setPower(float powerArg)
{
	power = powerArg;
}

void DashCommand::setDirection(float directionArg)
{
	direction = directionArg;
}

// class DoneCommand

DoneCommand::DoneCommand(): Command()
{
}

DoneCommand::~DoneCommand()
{
}

string DoneCommand::toString() const
{
	return "(done)";
}


// class EmptyCommand

EmptyCommand::EmptyCommand(): Command()
{
}

EmptyCommand::~EmptyCommand()
{
}

string EmptyCommand::toString() const
{
	return "";
}


// class InitCommand

InitCommand::InitCommand(const std::string &teamName, const std::string &version,
	bool isGoalie): Command(), teamName(teamName), version(version), isGoalie(isGoalie)
{
}

InitCommand::~InitCommand()
{
}

string InitCommand::toString() const
{
	ostringstream ss;
	ss << "(init " << teamName << " (version " << version << ")"
		<< (isGoalie ? "(goalie))" : ")");
	return ss.str();
}

const string &InitCommand::getTeamName() const
{
	return teamName;
}

const string &InitCommand::getVersion() const
{
	return version;
}

bool InitCommand::getIsGoalie() const
{
	return isGoalie;
}


// class KickCommand

KickCommand::KickCommand(float power, float direction): Command(), power(power),
	direction(direction)
{
}

KickCommand::~KickCommand()
{
}

string KickCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(kick " << power << " " << -direction << ")";
	return ss.str();
}

float KickCommand::getPower() const
{
	return power;
}

float KickCommand::getDirection() const
{
	return direction;
}

void KickCommand::setPower(float powerArg)
{
	power = powerArg;
}

void KickCommand::setDirection(float directionArg)
{
	direction = directionArg;
}


// class MoveCommand

MoveCommand::MoveCommand(Point point): Command(), x(point.x), y(point.y)
{
}

MoveCommand::MoveCommand(float x, float y): Command(), x(x), y(y)
{
}

MoveCommand::~MoveCommand()
{
}

string MoveCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(move " << x << " " << -y << ")";
	return ss.str();
}

float MoveCommand::getX() const
{
	return x;
}

float MoveCommand::getY() const
{
	return y;
}


// class PointToCommand

PointToCommand::PointToCommand(bool status): Command(),
		status(status), dist(0), dir(0)
{
}

PointToCommand::PointToCommand(float dist, float dir): Command(),
		status(true), dist(dist), dir(dir)
{
}

PointToCommand::~PointToCommand()
{
}

string PointToCommand::toString() const
{
	ostringstream ss;

	if (status)
	{
		ss.setf(ios::fixed | ios::showpoint);
		ss.precision(2);
		ss << "(pointto " << dist << " " << -dir << ")";
	}
	else
		ss << "(pointto off)";

	return ss.str();
}

float PointToCommand::getDist() const
{
	return dist;
}

float PointToCommand::getDir() const
{
	return dir;
}


// class SayCommand

SayCommand::SayCommand(const string &message): Command(), message(message)
{
}

SayCommand::~SayCommand()
{
}

string SayCommand::toString() const
{
	return "(say \"" + message + "\")";
}

const string &SayCommand::getMessage() const
{
	return message;
}


// class TackleCommand

TackleCommand::TackleCommand(float direction, bool foul): Command(), direction(direction), foul(foul)
{
}

TackleCommand::~TackleCommand()
{
}

string TackleCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	string foulString = "false";
	if (foul)
		foulString = "true";
	ss << "(tackle " << direction;
	if (config["Agent"]["Server"]["Version"].asInt() > 13)
		ss << " " << foulString;
	ss << ")";
	return ss.str();
}

float TackleCommand::getDirection() const
{
	return direction;
}

bool TackleCommand::getFoulIntention() const
{
	return foul;
}

// class TurnCommand

TurnCommand::TurnCommand(float direction): Command(), direction(direction)
{
}

TurnCommand::~TurnCommand()
{
}

string TurnCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(turn " << -direction << ")";
	return ss.str();
}

float TurnCommand::getDirection() const
{
	return direction;
}

float TurnNeckCommand::getDirection() const
{
	return direction;
}


// class TurnNeckCommand

TurnNeckCommand::TurnNeckCommand(float direction): Command(), direction(direction)
{
}

TurnNeckCommand::~TurnNeckCommand()
{
}

string TurnNeckCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(turn_neck " << -direction << ")";
	return ss.str();
}


// class EyeCommand

EyeCommand::EyeCommand(bool on): Command(), on(on)
{
}

EyeCommand::~EyeCommand()
{
}

string EyeCommand::toString() const
{
	if (on)
		return "(eye on)";
	return "(eye off)";
}

bool EyeCommand::isOn() const
{
	return on;
}


// class EyeCommand

ChangePlayerCommand::ChangePlayerCommand(int uniformNum, int playerType): Command(),
	uniformNum(uniformNum), playerType(playerType)
{
}

ChangePlayerCommand::~ChangePlayerCommand()
{
}

string ChangePlayerCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(change_player_type " << uniformNum << " " << playerType << ")";
	return ss.str();
}

int ChangePlayerCommand::getUniformNum() const
{
	return uniformNum;
}

int ChangePlayerCommand::getPlayerType() const
{
	return playerType;
}


// class FreeFormCommand

FreeFormCommand::FreeFormCommand(const string &message): Command(), message(message)
{
}

FreeFormCommand::~FreeFormCommand()
{
}

string FreeFormCommand::toString() const
{
	return "(say (freeform \"" + message + "\"))";
}

const string &FreeFormCommand::getMessage() const
{
	return message;
}


// class CLangVersionCommand

CLangVersionCommand::CLangVersionCommand(int minVer, int maxVer): Command(), minVer(minVer), maxVer(maxVer)
{
}

CLangVersionCommand::~CLangVersionCommand()
{
}

string CLangVersionCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed);
	ss.precision(0);
	ss << "(clang (ver " << minVer << " " << maxVer << "))";
	return ss.str();
}

int CLangVersionCommand::getMinVer() const
{
	return minVer;
}

int CLangVersionCommand::getMaxVer() const
{
	return maxVer;
}


// Commands for the trainer
ChangePlayModeCommand::ChangePlayModeCommand(const string & pm): Command(), playMode(pm)
{
}

ChangePlayModeCommand::~ChangePlayModeCommand()
{
}

string ChangePlayModeCommand::toString() const
{
	ostringstream ss;
	ss << "(change_mode " << playMode << ")";
	return ss.str();
}

const string & ChangePlayModeCommand::getPlayMode() const
{
	return playMode;
}

StartCommand::StartCommand(): Command()
{
}

StartCommand::~StartCommand()
{
}

string StartCommand::toString () const
{
	return "(start)";
};

RecoverCommand::RecoverCommand(): Command()
{
}

RecoverCommand::~RecoverCommand()
{
}

string RecoverCommand::toString() const
{
	return "(recover)";
}

EarCommand::EarCommand(bool _hear): Command()
{
	hear = _hear;
}

EarCommand::~EarCommand()
{
}

string EarCommand::toString() const
{
	return ((hear) ? ("(ear on)") : ("(ear off)"));
}

TrainerInitCommand::TrainerInitCommand(const string & version): Command(), version(version)
{
}

TrainerInitCommand::~TrainerInitCommand()
{
}

string TrainerInitCommand::toString() const
{
	ostringstream ss;
	ss << "(init (version " << version << "))";
	return ss.str();
}

TrainerChangePlayerCommand::TrainerChangePlayerCommand(const string & teamName, int uniformNum, int playerType): Command(),
	uniformNum(uniformNum), playerType(playerType), teamName(teamName)
{
}

TrainerChangePlayerCommand::~TrainerChangePlayerCommand()
{
}

string TrainerChangePlayerCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(change_player_type " << teamName << " " << uniformNum << " " << playerType << ")";
	return ss.str();
}

int TrainerChangePlayerCommand::getUniformNum() const
{
	return uniformNum;
}

int TrainerChangePlayerCommand::getPlayerType() const
{
	return playerType;
}

const string & TrainerChangePlayerCommand::getTeamName() const
{
	return teamName;
}

LookCommand::LookCommand(): Command()
{
}

LookCommand::~LookCommand()
{
}

string LookCommand::toString() const
{
	return "(look)";
}

MoveBallCommand::MoveBallCommand(float x, float y, float vdir, float vx, float vy): Command(),
	x(x), y(y), vdir(vdir), vx(vx), vy(vy)
{
}

MoveBallCommand::~MoveBallCommand()
{
}

string MoveBallCommand::toString() const
{
	ostringstream ss;
	ss.setf(ios::fixed | ios::showpoint);
	ss.precision(2);
	ss << "(move (ball) " << x << " " << y << " " << vdir << " " << vx << " " << vy << ")";
	return ss.str();
};


MovePlayerCommand::MovePlayerCommand(const string &teamName, int uNum,
    float x, float y, float vdir, float vx, float vy): Command(),
	teamName(teamName), uNum(uNum), x(x), y(y), vdir(vdir), vx(vx), vy(vy)
{
}

MovePlayerCommand::~MovePlayerCommand()
{
}

string MovePlayerCommand::toString() const
{
	ostringstream ss;
	ss.precision(2);
	ss << "(move (player " << teamName << " " << uNum << ") " << x << " " << y << " " << vdir << " " << vx << " " << vy << ")";
	return ss.str();
}

TeamNamesCommand::TeamNamesCommand(): Command()
{
}

TeamNamesCommand::~TeamNamesCommand()
{
}

string TeamNamesCommand::toString() const
{
	return "(team_names)";
}
