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

#ifndef __COMMAND_H
#define __COMMAND_H

#include <string>
#include <Types.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

// در این فایل کلاس Command قرار دارد. کلاس Command پدر کلاس های DashCommand و TurnCommand و KickCommand و MoveCommand و... است. در این کلاس اطلاعات مربوط به یک حرکت ذخیره می شوند. از اطلاعات مربوط به هر کاری رشته ی آن کار است که برای سرور فرستاده می شود. به عنوان مثال رشته ی یک حرکت Dash رشته ی زیر است.
// dash(100)


enum CommandType
{
	CT_ATTENTION,
	CT_ATTENTION_OFF,
	CT_BYE,
	CT_CATCH,
	CT_CHANGE_VIEW,
	CT_DASH,
	CT_DONE,
	CT_EMPTY,
	CT_INIT,
	CT_KICK,
	CT_MOVE,
	CT_SAY,
	CT_TACKLE,
	CT_TURN,
	CT_TURN_NECK,
	CT_EYE,
	CT_FREEFORM,
	CT_CHANGE_PLAYER
};

class Command
{
protected:

public:
	Command();
	virtual ~Command();

	virtual std::string toString() const = 0;
};

class AttentionToCommand: public Command
{
protected:
	TeamId teamId;
	int uniformNum;

public:
	AttentionToCommand(TeamId teamId, int uniformNum);
	virtual ~AttentionToCommand();

	virtual std::string toString() const;

	// Getting functions
	TeamId getTeamId() const;
	int getUniformNum() const;

};

class AttentionOffCommand: public Command
{
public:
	AttentionOffCommand();
	virtual ~AttentionOffCommand();

	virtual std::string toString() const;
};

class ByeCommand: public Command
{
public:
	ByeCommand();
	virtual ~ByeCommand();

	virtual std::string toString() const;
};

// در اینجا کلاس CatchCommand قرار دارد که اطلاعات مربوط به یک حرکت Catch را در خود ذخیره می کند.
class CatchCommand: public Command
{
protected:
	float direction;

public:
	CatchCommand(float direction);
	virtual ~CatchCommand();

	virtual std::string toString() const;

	// Getting functions
	float getDirection() const;

};

class ChangeViewCommand: public Command
{
protected:
	ViewModeQuality quality;
	ViewModeWidth width;

public:
	ChangeViewCommand(ViewModeQuality quality, ViewModeWidth width);
	virtual ~ChangeViewCommand();

	virtual std::string toString() const;

	// Getting functions
	ViewModeQuality getQuality() const;
	ViewModeWidth getWidth() const;

};

// در اینجا کلاس DashCommand قرار دارد که اطلاعات مربوط به یک حرکت Dash را در خود ذخیره می کند.
class DashCommand: public Command
{
protected:
	float power;
	float direction;

public:
// نحوه ساخته شدن کلاس DashCommand در زیر مشخص شده است. همان طور که می بینید به عنوان ورودی اول قدرت یک Dash را می گیرد و ورودی دوم هم جهت Dash هست که اگر صفر نباشد SideDash به حساب می آید. یک کد نمونه برای اینکه با زاویه ی ۹۰ درجه نسبت به جهت بدن و قدرت ۱۰۰ حرکت کنیم باید کد زیر را بنویسیم:
// command = new DashCommand(100, 90);
	DashCommand(float power, float direction = 0);
	virtual ~DashCommand();

	virtual std::string toString() const;

	// Getting functions
	float getPower() const;
	float getDirection() const;

	// Setting functions
	void setPower(float powerArg);
	void setDirection(float directionArg);
};

class DoneCommand: public Command
{
public:
	DoneCommand();
	virtual ~DoneCommand();

	virtual std::string toString() const;
};


// در اینجا کلاس EmptyCommand قرار دارد. از این کلاس در جاهایی استفاده می شود که می خواهیم هیچ کاری صورت نگیرد و هیچ یک از کار های Dash, Turn, Catch, Tackle , Kick انجام نشود.

class EmptyCommand: public Command
{
public:
	EmptyCommand();
	virtual ~EmptyCommand();

	virtual std::string toString() const;
};

class InitCommand: public Command
{
protected:
	std::string teamName;
	std::string version;
	bool isGoalie;

public:
	InitCommand(const std::string &teamName, const std::string &version,
			bool isGoalie);
	virtual ~InitCommand();

	virtual std::string toString() const;

	// Getting functions
	const std::string &getTeamName() const;
	const std::string &getVersion() const;
	bool getIsGoalie() const;

};

// در اینجا کلاس KickCommand قرار دارد که اطلاعات مربوط به یک Kick را در خود ذخیره می کند.
class KickCommand: public Command
{
protected:
	float power;
	float direction;

public:
// نحوه ساخته شدن کلاس KickCommand در زیر مشخص شده است. همان طور که می بینید به عنوان ورودی اول قدرت یک Kick را می گیرد و ورودی دوم هم جهت Kick هست. یک کد نمونه برای اینکه با زاویه ی ۹۰ درجه نسبت به جهت بدن و قدرت ۱۰۰ شوت بزنیم باید کد زیر را بنویسیم:
// command = new KickCommand(100, 90);
	KickCommand(float power, float direction);
	virtual ~KickCommand();

	virtual std::string toString() const;

	// Getting functions
	float getPower() const;
	float getDirection() const;

	// Setting functions
	void setPower(float powerArg);
	void setDirection(float directionArg);
};

// در اینجا کلاس MoveCommand قرار دارد که اطلاعات مربوط به یک Move را در خود ذخیره می کند. همان طور که می دانید Move به حرکتی گفته می شود که بازیکن در نقطه ای از صفحه که مشخص می کنیم ظاهر می شود و این کار را فقط می توان در بعضی از NonPlayOn ها انجام داد.
class MoveCommand: public Command
{
protected:
	float x;
	float y;

public:
// در اینجا دو نوع نحوه ی ساخته شدن یک نمونه از کلاس مشخص شده است. در یکی یک Point به عنوان ورودی می گیرد و دیگری هم مختصات نقطه ی مورد نظر را می گیرد.
	MoveCommand(Point point);
	MoveCommand(float x, float y);
	virtual ~MoveCommand();

	virtual std::string toString() const;

	// Getting functions
	float getX() const;
	float getY() const;

};

class PointToCommand: public Command
{
protected:
	bool status;
	float dist;
	float dir;

public:
	PointToCommand(bool status = false);
	PointToCommand(float dist, float dir);
	virtual ~PointToCommand();

	virtual std::string toString() const;

	// Getting functions
	float getDist() const;
	float getDir() const;

};

class SayCommand: public Command
{
protected:
	std::string message;

public:
	SayCommand(const std::string &message);
	virtual ~SayCommand();

	virtual std::string toString() const;

	// Getting functions
	const std::string &getMessage() const;

};

// در اینجا کلاس TackleCommand قرار دارد که اطلاعات مربوط به یک Tackle را در خود ذخیره می کند.
class TackleCommand: public Command
{
protected:
	float direction;
	bool foul;

public:
// در اینجا نحوه ی ساخته شدن یک نمونه از کلاس TackleCommand را می بینید که جهت Tackle را به عنوان ورودی می گیرد.
	TackleCommand(float direction, bool foul = false);
	virtual ~TackleCommand();

	virtual std::string toString() const;

	// Getting functions
	float getDirection() const;
	bool getFoulIntention() const;

};

// در اینجا کلاس TurnCommand قرار دارد که اطلاعات مربوط به یک Turn را در خود ذخیره می کند.
class TurnCommand: public Command
{
protected:
	float direction;

public:
// در اینجا نحوه ی ساخته شدن یک نمونه از کلاس TurnCommand را می بینید که میزان درجه ی چرخیدن را نسبت به جهت بدن به عنوان ورودی می گیرد.
	TurnCommand(float direction);
	virtual ~TurnCommand();

	virtual std::string toString() const;

	// Getting functions
	float getDirection() const;

};

class TurnNeckCommand: public Command
{
protected:
	float direction;

public:
	TurnNeckCommand(float direction);
	virtual ~TurnNeckCommand();

	virtual std::string toString() const;

	// Getting functions
	float getDirection() const;

};

class EyeCommand: public Command
{
protected:
	bool on;

public:
	EyeCommand(bool on);
	virtual ~EyeCommand();

	virtual std::string toString() const;

	// Getting functions
	bool isOn() const;

};

class ChangePlayerCommand: public Command
{
protected:
	int uniformNum;
	int playerType;

public:
	ChangePlayerCommand(int uniformNum, int playerType);
	virtual ~ChangePlayerCommand();

	virtual std::string toString() const;

	// Getting functions
	int getUniformNum() const;
	int getPlayerType() const;
};

class FreeFormCommand: public Command
{
protected:
	std::string message;

public:
	FreeFormCommand(const std::string &message);
	virtual ~FreeFormCommand();

	virtual std::string toString() const;

	// Getting functions
	const std::string &getMessage() const;

};

class CLangVersionCommand: public Command
{
protected:
	int minVer, maxVer;

public:
	CLangVersionCommand(int minVer, int maxVer);
	virtual ~CLangVersionCommand();

	virtual std::string toString() const;

	// Getting functions
	int getMinVer() const;
	int getMaxVer() const;

};

// Commands added for trainer
class ChangePlayModeCommand : public Command
{
protected:
	std::string playMode;

public:
	ChangePlayModeCommand(const std::string & playMode);
	virtual ~ChangePlayModeCommand();
	virtual std::string toString() const;

	// Getting functions
	const std::string & getPlayMode() const;
};

class StartCommand: public Command
{
public:
	StartCommand();
	virtual ~StartCommand();
	virtual std::string toString() const;
};

class RecoverCommand: public Command
{
public:
	RecoverCommand();
	virtual ~RecoverCommand();
	virtual std::string toString() const;
};

class EarCommand: public Command
{
protected:
	bool hear;

public:
	EarCommand(bool hear);
	virtual ~EarCommand( );
	virtual std::string toString() const;
};

class TrainerInitCommand: public Command
{
protected:
	std::string version;

public:
	TrainerInitCommand(const std::string & version);
	virtual ~TrainerInitCommand();
	virtual std::string toString() const;
};

class TrainerChangePlayerCommand: public Command
{
protected:
	int uniformNum;
	int playerType;
	std::string teamName;

public:
	TrainerChangePlayerCommand(const std::string &teamName, int uniformNum, int playerType);
	virtual ~TrainerChangePlayerCommand();

	virtual std::string toString() const;

	// Getting functions
	int getUniformNum() const;
	int getPlayerType() const;
	const std::string & getTeamName() const;
};

class LookCommand:public Command
{
public:
	LookCommand();
	virtual ~LookCommand();

	virtual std::string toString() const;
};

class MoveBallCommand: public Command {
protected:
	float x, y, vdir, vx, vy; //note vdir is NOT important

public:
	MoveBallCommand(float x, float y, float vdir, float vx, float vy);
	virtual ~MoveBallCommand();

	std::string toString() const;
};

class MovePlayerCommand : public Command
{
protected:
	std::string teamName;
	int uNum;
	float x,y,vdir,vx,vy;

public:
	MovePlayerCommand(const std::string &teamName, int uNum, float x, float y, float vdir,float vx, float vy);
	virtual ~MovePlayerCommand();

	std::string toString() const;
};

class TeamNamesCommand :public Command
{
public:
	TeamNamesCommand();
	virtual ~TeamNamesCommand();
	std::string toString() const;
};

#endif // __COMMAND_H
