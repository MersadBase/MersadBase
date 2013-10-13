/*
 * ChainAction.h
 *
 *  Created on: 28/08/2012
 *      Author: amin
 */
#include <Command.h>
#include <WorldModel.h>
#include <Logger.h>
#include <Formation.h>
#include <BasicTurn.h>
#include <Intercept.h>
#include <DecideCommonTypes.h>
#include <Pass.h>
#include <MultiStepKick.h>

#ifndef CHAINACTION_H_
#define CHAINACTION_H_

enum CAType{
	CA_NONE,
	CA_INTERCEPT,
	CA_TURN,
	CA_DASH,
	CA_DRIBBLE,
	CA_TURNDIR,
	CA_MULTISTEP,
	CA_MULTISTEP_PASS,
	CA_HOLD,
	CA_VIEW,
	CA_KICK,
	CA_COMMAND,
	CA_SAYPASS
};


std::ostream &operator<<(std::ostream &stream, const CAType &var);

class ChainAction
{
public:
	unsigned cycle;
	CAType type;
	Command* command;
	const WorldModel* wm;

	float dir,power;

	Point point;//ChainTurn
	TurnNeckMode tnm;
	PassEntry passEntry;

	ChainAction();
	ChainAction(const WorldModel* _wm);
	void calCommand();
	void calCommand(Form &form);
	Command* getCommand();
	virtual ~ChainAction();
};

std::ostream &operator<<(std::ostream &stream, const ChainAction &var);

#endif /* CHAINACTION_H_ */
