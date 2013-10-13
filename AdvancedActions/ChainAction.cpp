/*
 * ChainAction.cpp
 *
 *  Created on: 28/08/2012
 *      Author: amin
 */

#include <ChainAction.h>
#include <Share.h>

std::ostream &operator<<(std::ostream &stream, const CAType &var)
{
	switch (var)
	{
	case CA_NONE:
		stream << "CA_NONE";
		break;
	case CA_INTERCEPT:
		stream << "CA_INTERCEPT";
		break;
	case CA_TURN:
		stream << "CA_TURN";
		break;
	case CA_DASH:
		stream << "CA_DASH";
		break;
	case CA_DRIBBLE:
		stream << "CA_DRIBBLE";
		break;
	case CA_TURNDIR:
		stream << "CA_TURNDIR";
		break;
	case CA_MULTISTEP:
		stream << "CA_MULTISTEP";
		break;
	case CA_HOLD:
		stream << "CA_HOLD";
		break;
	case CA_VIEW:
		stream << "CA_VIEW";
		break;
	case CA_KICK:
		stream << "CA_KICK";
		break;
	case CA_COMMAND:
		stream << "CA_COMMAND";
		break;
	case CA_MULTISTEP_PASS:
		stream << "CA_MULTISTEP_PASS";
		break;
	case CA_SAYPASS:
		stream << "CA_SAYPASS";
		break;
	}
	return stream;
}

std::ostream &operator<<(std::ostream &stream, const ChainAction &var)
{
	stream << "Type : " << var.type << " Cycle : " << var.cycle;
	return stream;
}

ChainAction::ChainAction()
{
	cycle = -1; //cycle = inf
	type = CA_NONE;
	wm = NULL;
}

ChainAction::ChainAction(const WorldModel* _wm)
{
	cycle = -1; //cycle = inf
	type = CA_NONE;
	wm = _wm;
}

void ChainAction::calCommand()
{
	Form chertForm;
	calCommand(chertForm);
}

void ChainAction::calCommand(Form &form)
{
	command = NULL;
	switch (type)
	{
	case CA_TURN:
	{
		command = TurnToPoint(point, wm->getBody()).getCommand();
		break;
	}
	case CA_TURNDIR:
	{
		Ball vBall = wm->getBall();
		vBall.simulateByDynamics(wm->getBody());
		Body vBody = wm->getBody();
		vBody.simulateByDynamics();
		if (!vBody.isBallInKickable(vBall))
		{
			LOG << "Ghaedatan Nabayad Injoori Mishod  Noise dige !!!" << endl;
			break;
		}
		command = TurnToAngle(dir, wm->getBody()).getCommand();
		break;
	}
	case CA_VIEW:
	{
		form.headForm.tnMode = tnm;
		form.headForm.direction = dir;
		break;
	}
	case CA_MULTISTEP:
	{
		if (wm->getBody().isCollisionBall() or !wm->isBallKickable(0.055))
		{
			//Gand Khorde Be Multistepet dige nemishe edame dad
			Share::chain.clear();
			break;
		}
		command = new KickCommand(power, dir);
		break;
	}
	case CA_MULTISTEP_PASS:
	{
		if (wm->getBody().isCollisionBall() or !wm->isBallKickable(0.055))
		{
			Share::lastKick.noKickCycle = 0;
			Share::lastKick.noMoveCycle = 0;
			Share::chain.clear();
			break;
		}
		Pass pass(wm);
		bool isStillGood = pass.isStillGood(passEntry);
		if (isStillGood or wm->getOppCountInDistanceFromPoint(wm->getBall().getPos(), 4))
		{
			if (!isStillGood)
				LOG << "Passe Khooob Nabood Vali Majboorim Dige" << endl;
			command = new KickCommand(power, dir);
		}
		else
		{
//			HoldBall hold(wm);
//			hold.decide();
//			command = hold.getCommand();
			Share::lastKick.noKickCycle = 0;
			Share::lastKick.noMoveCycle = 0;
			Share::chain.clear();
			LOG << "Pass Is Not Good Any More" << endl;
		}
		break;
	}
	default:
		break;
	}
}

Command* ChainAction::getCommand()
{
	return command;
}

ChainAction::~ChainAction()
{
	// TODO Auto-generated destructor stub
}
