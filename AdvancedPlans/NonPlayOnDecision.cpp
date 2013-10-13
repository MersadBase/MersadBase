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
 *  	and is modified by: Mohammad Zareie, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#include <cassert>
#include <cmath>
#include <algorithm>
#include <Logger.h>
#include <Types.h>
#include <Command.h>
#include <BasicTurn.h>
#include <BasicKick.h>
#include <BasicDash.h>
#include <NonPlayOnDecision.h>
#include <Pass.h>
#include <Intercept.h>
#include <Share.h>
#include <Basics.h>
#include <Rectangle.h>
#include <DefensePlan.h>
#include <Middle.h>
#include <Offense.h>
#include <Penalty.h>
#include <Goalie.h>
#include <Share.h>

#include <DecideCommonTypes.h>

#define BEFORE_KICK_OFF_POINT Point(-100, -100)
#define VPM_GOALIE_POINT Point(-90, -100)
#define STATIC_FORMATION_PM_BEFORE_KICK_OFF 0
#define STATIC_FORMATION_PM_FREE_KICK 1
#define STATIC_FORMATION_PM_GOAL_KICK 2
#define STATIC_FORMATION_PM_GOAL_KICK_OPP 3

#define LOGN LOG

using namespace std;
using namespace Basics;

NonPlayOnDecision::NonPlayOnDecision(const WorldModel *wm) :
		wm(wm)
{
}

NonPlayOnDecision::~NonPlayOnDecision()
{
}

void NonPlayOnDecision::usualPos(Form &form)
{
	LOGN << "starting usualPos" << endl;
	if (wm->getBody().isGoalie())
	{
		Goalie goalie = Goalie(wm);
		goalie.decide(form);
		command = goalie.getCommand();
		//LOGN<<"getting command from goalie which is"<<endl<<command->toString()<<endl;
		return;
	}
	Point targetPoint;
	PlayMode mode = wm->getPlayMode();
	targetPoint = wm->getPlayOnFormation().getPosition(wm->getUniNum() - 1, wm->getBallPos());
	if (mode == PM_CORNER_KICK_OPP or mode == PM_KICK_IN_OPP or mode == PM_BACK_PASS
			or mode == PM_OFFSIDE_OPP or mode == PM_CATCH_FAULT_OPP or mode == PM_GOAL_KICK_OPP
			or mode == PM_FREE_KICK_OPP)
	{
		if (targetPoint.getDistance(wm->getBall().getPos()) < 9.15)
		{
			Vector v = targetPoint - wm->getBall().getPos();
			v.setAsPolar(9.5, v.getDirection());
			targetPoint = wm->getBall().getPos() + v;
		}
	}
	form.headForm.tnMode = TNM_LOOK_CAREFULLY_TO_BALL;
	command = ForbiddenAreaGotoPoint(targetPoint, wm->getBall().getPos(), 10, wm->getBody(),
			1/*tof*/).getCommand();
	if (wm->getPlayMode() == PM_GOAL_KICK_OPP)
		command = ForbiddenAreaGotoPoint(targetPoint, 36, 20, -20, wm->getBody()).getCommand();
//	PlayMode mode = wm->getPlayMode();
//	if (mode == PM_CORNER_KICK_OPP or mode == PM_KICK_IN_OPP or mode == PM_BACK_PASS
//			or mode == PM_OFFSIDE_OPP or mode == PM_CATCH_FAULT_OPP or mode == PM_GOAL_KICK_OPP
//			or mode == PM_FREE_KICK_OPP)
//	{
//		command = ForbiddenAreaGotoPoint(targetPoint,wm->getBall().getPos(),10,wm->getBody(),1/*tof*/).getCommand();
//
//	}
//	else
//		command = DashNormalGotoPoint(targetPoint,1,wm->getBody()).getCommand();
	LOGN << "the command is " << command->toString() << endl;
	LOGN << "usual pos finished" << endl;
}
void NonPlayOnDecision::usualKick(Form &form, int forceKicker)
{
	LOGN << "Usual Kick started" << endl;
	if (wm->getBody().isGoalie())
	{
		command = DashNormalGotoPoint(Point(-50, 0), 1, wm->getBody()).getCommand();
		return;
	}
//	Vector t;
//	t.setAsPolar(0.5,
//			Vector(wm->getBall().getPos() - Point(52.5, 0)).getDirection());
//	t = wm->getBall().getPos() + t;
//	Point firstPos = t; //nearest Player Point
//	t.setAsPolar(8,
//			Vector(wm->getBall().getPos() - Point(52.5, 0)).getDirection());
//	t = wm->getBall().getPos() - t;
//	Point secondPos = t; //second nearest Player Point
//	LOGN << "First Pos : " << firstPos << endl;
//	LOGN << "Second Pos : " << secondPos << endl;
//	if (wm->getBall().getSimCounter() > 3)
//	{
//		form.headForm.tnMode = TNM_LOOK_NORMALLY_TO_BALL;
//		usualPos(form);
//		return; //to ke toopo nemibini ghalat mikoni nazar bedi ki bayad bere toopo begire
//	}
//	LOGN << "last NPO cycle : " << wm->lastNPOHearCycle << endl;
//	if (wm->lastNPOHearCycle == wm->getCurCycle())
//	{
//		LOGN << "i heard an npo this cycle" << endl;
//		if (wm->heardNPOReciever == wm->getBody().getUniNum())
//		{
//			LOGN << "i should catch the ball from the other guy" << endl;
//			command = DashNormalGotoPoint(secondPos, 0.75, wm->getBody(), 50, 6,
//					0).getCommand();
//			//LOGN<<" so the command is :"<<command->toString()<<endl;
//			return;
//		}
//		usualPos(form);
//		if (wm->heardNPOSender != wm->getBody().getUniNum())
//		{
//			LOGN << "i'm no the sender neither so just doing the usual pos"
//					<< endl;
//			return;
//		}
//	}
//
//	int kicker = -1;
//	int receiver = -1;
//
//	if (Share::lastNPOSendCycle == wm->getCurCycle() - 1)
//	{
//		LOGN
//				<< "i heard myself and i'm the kicker so i'm not gonna sort any stuff"
//				<< endl;
//		form.sayForm.npoReceiver = Share::lastNPOReceiver;
//		form.sayForm.addSay(ST_NON_PLAY_ON);
//		LOGN << "saying it again " << endl;
//		kicker = wm->getBody().getUniNum();
//		receiver = Share::lastNPOReceiver;
//	}
//	else
//	{
//		vector < pair<double, int> > list;
//		LOGN << "finding kicker and receiver by sorting datas by my self"
//				<< endl;
//		for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
//		{
//			if (wm->getFullPlayer(TID_TEAMMATE, i).isValid())
//				list.push_back(
//						pair<double, unsigned>(
//								wm->getFullPlayer(TID_TEAMMATE, i).getPos().getDistance(
//										firstPos), i + 1));
//		}
//		sort(list.begin(), list.end());
//		LOGN << "second pos : " << secondPos << endl;
//		if (list.size())
//		{
//			kicker = list[0].second;
//			LOGN << "kicker : " << kicker << endl;
//		}
//		if (list.size() > 1)
//			receiver = list[1].second;
//	}
//	if (forceKicker != -1)
//		kicker = forceKicker;
//	LOGN << "kicker is :" << kicker << endl;
//	LOGN << "receiver is :" << receiver << endl;
//	if (kicker == (int) wm->getBody().getUniNum())
//	{
//		if (receiver != -1)
//		{
//			LOGN << " second fastest : " << receiver - 1 << endl;
//			form.sayForm.npoReceiver = receiver;
//			form.sayForm.addSay(ST_NON_PLAY_ON);
//			Share::lastNPOSendCycle = wm->getCurCycle();
//			Share::lastNPOReceiver = receiver;
//		}
//		else
//			LOGN << "i didn't find receiver" << endl;
//		LOGN << "Point to go : " << firstPos << endl;
//		if (wm->getBody().getPos().getDistance(firstPos) <= 0.75
//				&& wm->isBallKickable())
//		{
//			LOGN << "I'M READY TO SHOOT" << endl;
////			t = wm->getFullPlayer(TID_TEAMMATE, receiver  - 1).getPos(); //final target for shoot
////			LOGN << "the fucking distance  : " << t.getDistance(secondPos)
////					<< endl;
////			if ((t.getDistance(secondPos) < 0.75
////					and wm->getPlayModeRemainCycle() < 120)
////					or wm->getPlayModeRemainCycle() < 110)
//			if (wm->getPlayModeRemainCycle() < 110)
//			{
//				form.sayForm.says.pop_back();
//				Pass p(wm);
//				p.getValue();
//				Share::lastFreeKick = 0; //the player can't kick the ball again for 5 cycles
//				p.decide(form);
//				Share::lastKick = p.getBestEntry();
//				command = p.getCommand();
//				//command = KickToPointVelocity(t, 1.5, wm->getBall(), wm->getBody()).getCommand();
//			}
//			else
//				command =
//						TurnToPoint(Point(52.5, 0), wm->getBody()).getCommand();
//
//		}
//		else
//		{
//			command = DashNormalGotoPoint(firstPos, 0.5, wm->getBody(), 50, 6,
//					0).getCommand();
//			LOGN << "I'm going to shooting point" << endl;
//			LOGN << "the command is : " << command->toString() << endl;
//		}
//	}
//	else
//		usualPos(form);
	vector < pair<double, unsigned> > list;
	LOGN << "finding kicker and receiver by sorting datas by my self using NormalFormation" << endl;
	Point targetPos;
	for (unsigned i = 1; i < FULL_PLAYERS_NUM; i++)
	{
		Point hisTargetPos;
		if (wm->getPlayMode() == PM_GOAL_KICK)
			hisTargetPos = wm->getStaticFormation().getPosition(i, STATIC_FORMATION_PM_GOAL_KICK);
		else if (wm->getPlayMode() == PM_KICK_IN or wm->getPlayMode() == PM_CORNER_KICK)
			hisTargetPos = wm->getKickInFormation().getPosition(i, wm->getBall().getPos());
		else
			hisTargetPos = wm->getPlayOnFormation().getPosition(i, wm->getBall().getPos());
		if (i == wm->getBody().getUniNum() - 1)
			targetPos = hisTargetPos;
		list.push_back(make_pair(hisTargetPos.getDistance(wm->getBall().getPos()), i + 1));
	}
	sort(list.begin(), list.end());
	int kicker = -1;
	if (list.size())
		kicker = list[0].second;
	if (forceKicker != -1)
		kicker = forceKicker;
	if (kicker == (int) wm->getBody().getUniNum())
	{
//				Vector t;
//				t.setAsPolar(0.5,
//						Vector(wm->getBall().getPos() - Point(52.5, 0)).getDirection());
//				t = wm->getBall().getPos() + t;
		targetPos = wm->getBall().getPos(); //nearest Player Point
		if (wm->getPlayMode() == PM_KICK_IN)
		{
			if (targetPos.y > 0)
				targetPos.y += wm->getKickableArea() / 2;
			else
				targetPos.y -= wm->getKickableArea() / 2;
		}
		else
			targetPos.x -= wm->getKickableArea() / 2;
	}
	command = DashNormalGotoPoint(targetPos, 0.5, wm->getBody(), 60, 6, 0).getCommand();
	LOGN << "distance too ball : " << wm->getBody().getPos().getDistance(targetPos) << endl;
	if (wm->getBody().getPos().getDistance(targetPos) <= 0.75 && wm->isBallKickable())
	{
		LOGN << "I'm Ready to shoooot" << endl;
		Pass p(wm);
//		if (wm->getPlayMode() == PM_GOAL_KICK)
//			p.getinValidRect() = Rectangle(Point(-52.5, 20), Point(-35, -20));
		if (wm->getPlayMode() == PM_GOAL_KICK)
			p.setValidPlayers("456");
		p.getValue();
		if (wm->getPlayModeRemainCycle() < 170 && p.isPassAvailable()
				&& (wm->getPlayMode() == PM_KICK_IN or fabs(wm->getBody().getBodyDir()) < 90)
				&& wm->getSeeTime() == wm->getCurTime())
		{
			LOGN << "I'm gonna pass this cycle" << endl;
			p.decide(form);
			Share::lastKick = p.getBestEntry();
			Share::lastKick.noKickCycle = wm->getCurCycle() + 5;
			command = p.getCommand();
			//command = KickToPointVelocity(t, 1.5, wm->getBall(), wm->getBody()).getCommand();
		}
		else
		{
//			float angle = Vector(Point(52.5,0)-wm->getBody().getPos()).getDirection();
			LOGN << "just changing my body dir and headDir" << endl;
//			command = TurnToAngle(angle, wm->getBody(), 5).getCommand();
//			form.headForm.tnMode = TNM_LOOK_TO_DIR_WIDE;
//			form.headForm.direction = angle;
			float headDir = wm->getBody().getHeadDir();
			float bodyDir = wm->getBody().getBodyDir();
//			if (wm->getCurTime() % 3 == 0)
//			{
			bodyDir += 180;
			headDir += 180;
			headDir = Degree::normalizeAngle(headDir);
			bodyDir = Degree::normalizeAngle(bodyDir);

//			}
			if (fabs(Degree::getDeltaAngle(bodyDir, 0)) > 10
					&& fabs(Degree::getDeltaAngle(bodyDir, -180)) > 10)
			{
				bodyDir = 0;
				headDir = 0;
			}
			if (wm->getPlayMode() == PM_GOAL_KICK)
			{ //poshtam faghat darvaze bane nemikham rikhte nahsesho bebinam
				bodyDir = 0;
				headDir = 0;
			}
			if (wm->getPlayMode() == PM_KICK_IN)
			{
				float angle = 90;
				if (targetPos.y > 0)
					angle = -90;
				bodyDir = angle;
				headDir = angle;
			}
			command = TurnToAngle(bodyDir, wm->getBody(), 5).getCommand();
			form.headForm.tnMode = TNM_LOOK_TO_DIR_WIDE;
			form.headForm.direction = headDir;
		}
	}
}
void NonPlayOnDecision::decide(Form &form)
{
	command = new EmptyCommand();
	unsigned kicker = 0;
	float minDistance = 1000;
	LOGN << "PlayModeRemainCycle : " << wm->getPlayModeRemainCycle() << endl;
	LOGN << "isBallKickable : " << wm->isBallKickable() << endl;
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
	{
		positions[i] = wm->getFormation().getExactPosition(i + 1, wm->getBall().getPos());
		if (positions[i].getDistance(wm->getBall().getPos()) < minDistance and i != 0)
		{
			minDistance = positions[i].getDistance(wm->getBall().getPos());
			kicker = i + 1;
		}
	}
	LOGN << "PlayMode : " << wm->getPlayMode() << endl;
	LOGN << "VirtualPlayMode : " << wm->getVirtualPlayMode() << endl;

	if (wm->getPlayMode() == PM_FREE_KICK and wm->getVirtualPlayMode() == VPM_GOALIE_CATCH_BALL
			and wm->getBody().isGoalie())
	{
		Point movePoint1(-40, -15);
		Point movePoint2(-40, 15);
		if (wm->getPlayModeRemainCycle() < 195)
		{
			if (getDist(wm->getBody().getPos(), movePoint1) > 1
					&& getDist(wm->getBody().getPos(), movePoint2) > 1)
			{
				Rectangle r1(-52, 0, 0, -34); //lower half
				Rectangle r2(-52, 34, 0, 0); //upper half
				unsigned r1Count = 0, r2Count = 0;
				for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
				{
					if (wm->getFullPlayer(TID_OPPONENT, i).isValid())
					{
						if (r1.isInRectangle(wm->getFullPlayer(TID_OPPONENT, i).getPos()))
							r1Count++;
						else if (r2.isInRectangle(wm->getFullPlayer(TID_OPPONENT, i).getPos()))
							r2Count++;
					}
				}
				for (unsigned i = 0; i < HALF_PLAYERS_NUM; i++)
				{
					if (wm->getHalfPlayer(TID_OPPONENT, i).isValid())
					{
						if (r1.isInRectangle(wm->getHalfPlayer(TID_OPPONENT, i).getPos()))
							r1Count++;
						else if (r2.isInRectangle(wm->getHalfPlayer(TID_OPPONENT, i).getPos()))
							r2Count++;
					}
				}
				if (r1Count < r2Count)
					command = new MoveCommand(movePoint1);
				else
					command = new MoveCommand(movePoint2);

			}
			else
			{
				if (wm->getPlayModeRemainCycle() < 175)
				{
					Pass pass(wm);
					pass.getinValidRect() = Rectangle(Point(-52.5, 20), Point(-35, -20));
					pass.getValue();
					if (pass.isPassAvailable())
					{
						pass.decide(form);
						Share::lastKick = pass.getBestEntry();
						command = pass.getCommand();
						LOGN << "goalie is gonna pass " << command->toString() << endl;
					}
					else
					{
						command = TurnToAngle(0, wm->getBody(), 5).getCommand();
						form.headForm.tnMode = TNM_SEARCH;
					}
				}
				else
				{
					if (wm->getPlayModeRemainCycle() < 165 && wm->getPlayModeRemainCycle() > 162)
					{
						LOGN << "I couldn't Pass here so i'm gonna try the other side" << endl;
						Point p2 = wm->getBody().getPos();
						p2.y *= -1;
						command = new MoveCommand(p2);
					}
					else
					{
						command = TurnToAngle(0, wm->getBody()).getCommand();
						form.headForm.tnMode = TNM_LOOK_TO_DIR_WIDE;
						form.headForm.direction = 0;
					}
				}
			}
		}
		else
		{
			command = TurnToAngle(0, wm->getBody()).getCommand();
			form.headForm.tnMode = TNM_LOOK_TO_DIR_WIDE;
			form.headForm.direction = 0;
		}
		return;
	}
	switch (wm->getPlayMode())
	{
	case PM_BEFORE_KICK_OFF:
	case PM_KICK_OFF_OPP:
	case PM_GOAL:
	case PM_GOAL_OPP:
	{
		Point movePoint(-1, 0);
//		if (wm->getBody().getUniNum() == 1)
//			movePoint = Point(-52.5, 0);
		movePoint = wm->getStaticFormation().getPosition(wm->getBody().getUniNum() - 1,
				STATIC_FORMATION_PM_BEFORE_KICK_OFF);
		LOGN << "Point: " << movePoint << endl;
		if (wm->getBody().getPos().getDistance(movePoint) > 0.01)
			command = new MoveCommand(movePoint);
		else
			command = TurnToPoint(Point(0, 0), wm->getBody()).getCommand();
		break;
	}
	case PM_KICK_OFF:
	{
		if (wm->getPlayModeRemainCycle() < 180 and isStaminaEnough()
				and /*wm->getBody().getUniNum() == kicker*/wm->isBallKickable())
		{
//			Pass pass(wm);
//			pass.setValidPlayers("69A");
//			if (pass.isPassAvailable())
//			{
//				pass.decide(form);
//				Share::lastKick = pass.getBestEntry();
//				Share::lastKick.noKickCycle = wm->getCurCycle() + 5;
//				Share::lastKick.noMoveCycle = wm->getCurCycle() + 5;
//				command = pass.getCommand();
//			}
//			else
//			{
//				form.headForm.tnMode = TNM_LOOK_TO_DIR_WIDE;
//				form.headForm.direction = -90;
//			}
			command = FastKickToPoint(wm->getStaticFormation().getPosition(5, STATIC_FORMATION_PM_BEFORE_KICK_OFF),
					wm->getBall(), wm->getBody()).getCommand();
		}
		if (wm->isBallKickable())
		{
			form.headForm.tnMode = TNM_LOOK_TO_DIR_WIDE;
			if (wm->getBody().getHeadDir() > 0)
				form.headForm.direction = -90;
			else
				form.headForm.direction = 90;
		}
		else
			form.headForm.tnMode = TNM_LOOK_CAREFULLY_TO_BALL;
		break;
	}

	case PM_CORNER_KICK:
	case PM_FREE_KICK_FAULT_OPP:
	case PM_KICK_IN:
	{
		usualKick(form);
//		LOGN << "PM_KICK_IN STARTED" << endl;
//		form.headForm.tnMode = TNM_LOOK_NORMALLY_TO_BALL;
//		vector < pair<double, unsigned> > list;
//		LOGN << "finding kicker and receiver by sorting datas by my self using kickInFormation"
//				<< endl;
//		Point targetPos = wm->getKickInFormation().getPosition(wm->getBody().getUniNum() - 1,
//				wm->getBall().getPos());
//		for (unsigned i = 1; i < FULL_PLAYERS_NUM; i++)
//		{
//			list.push_back(
//					make_pair(
//							wm->getKickInFormation().getPosition(i, wm->getBall().getPos()).getDistance(
//									wm->getBall().getPos()), i + 1));
//		}
//		sort(list.begin(), list.end());
//		if (list.size())
//		{
//			if (list[0].second == wm->getBody().getUniNum())
//			{
////				Vector t;
////				t.setAsPolar(0.5,
////						Vector(wm->getBall().getPos() - Point(52.5, 0)).getDirection());
////				t = wm->getBall().getPos() + t;
//				targetPos = wm->getBall().getPos(); //nearest Player Point
//				if (targetPos.y > 0)
//					targetPos.y += wm->getKickableArea() / 2;
//				else
//					targetPos.y -= wm->getKickableArea() / 2;
//			}
//		}
//		command = DashNormalGotoPoint(targetPos, 0.5, wm->getBody(), 60, 6, 0).getCommand();
//		LOGN << "distance to ball : " << wm->getBody().getPos().getDistance(targetPos) << endl;
//		if (wm->getBody().getPos().getDistance(targetPos) <= 0.75 && wm->isBallKickable())
//		{
//			LOGN << "I'm Ready to shoooot" << endl;
//			Pass p(wm);
//			p.getValue();
//			if (wm->getPlayModeRemainCycle() < 170 && p.isPassAvailable())
//			{
//				LOGN << "I'm gonna pass this cycle" << endl;
//				p.decide(form);
//				Share::lastKick = p.getBestEntry();
//				Share::lastKick.noKickCycle = wm->getCurCycle() + 5;
//				command = p.getCommand();
//				//command = KickToPointVelocity(t, 1.5, wm->getBall(), wm->getBody()).getCommand();
//			}
//			else
//			{
//				float angle = 90;
//				LOGN << "just changing my body dir and headDir" << endl;
//				if (targetPos.y > 0)
//					angle = -90;
//				command = TurnToAngle(angle, wm->getBody(), 5).getCommand();
//				form.headForm.tnMode = TNM_LOOK_TO_DIR_WIDE;
//				form.headForm.direction = angle;
//			}
//		}
		break;
	}
	case PM_BACK_PASS_OPP:
	case PM_OFFSIDE:
	case PM_FREE_KICK_FAULT:
	case PM_CATCH_FAULT:
	case PM_FREE_KICK:
	case PM_OFFSIDE_OPP:
	{
		if (wm->getVirtualPlayMode() == VPM_GOALIE_CATCH_BALL)
		{
			command = DashNormalGotoPoint(
					wm->getStaticFormation().getPosition(wm->getBody().getUniNum() - 1,
							STATIC_FORMATION_PM_FREE_KICK), 0.5, wm->getBody(), 60, 6, 0).getCommand();
			form.headForm.tnMode = TNM_LOOK_CAREFULLY_TO_BALL;
			break;
		}
		usualKick(form);
		break;
	}
	case PM_GOAL_KICK:
	{
//		if (!wm->getBody().isGoalie())
//		{
//			command = DashNormalGotoPoint(
//					wm->getStaticFormation().getPosition(wm->getBody().getUniNum() - 1,
//							STATIC_FORMATION_PM_GOAL_KICK), 0.5, wm->getBody(), 60, 6, 0).getCommand();
//		}
//		else
//			usualKick(form, 1);
		usualKick(form);
		break;
	}
	case PM_FOUL_CHARGE_OPP:
	{
//		Point point = positions[wm->getBody().getUniNum() - 1];
//		if (wm->getBody().getUniNum() == kicker and kicker != 1)
//			point = wm->getBall().getPos();
//		LOGN << "Point: " << point << endl;
//
//		if (wm->getBody().getPos().getDistance(point) > 0.7)
//		{
//			if (wm->getBody().getUniNum() == kicker and kicker == 1)
//				command = new MoveCommand(point);
//			else
//				command =
//						DashNormalGotoPoint(point, 0.5, wm->getBody(), 30).getCommand();
//		}
//		else if (wm->getCurCycle() % 2 == 1)
//			command = new TurnCommand(100);
//
//		if (wm->getBody().getUniNum() == kicker and wm->isBallKickable()
//				and isAllPlayersReady())
//		{
//			Pass pass(wm);
//			if (pass.isPassAvailable())
//			{
//				pass.decide(form);
//				Share::lastKick = pass.getBestEntry();
//				command = pass.getCommand();
//			}
//		}
		usualKick(form);
		break;
	}

	case PM_CORNER_KICK_OPP:
	case PM_KICK_IN_OPP:
	case PM_BACK_PASS:
	case PM_CATCH_FAULT_OPP:
	case PM_GOAL_KICK_OPP:
	case PM_FREE_KICK_OPP:
	{
		usualPos(form);
		break;
	}
	case PM_FOUL_CHARGE:
	{
		Point point = positions[wm->getBody().getUniNum() - 1];
		LOGN << "Point: " << point << endl;
		if (wm->getBody().getPos().getDistance(point) > 0.7 and wm->getBody().getStamina() >= 3000)
			command = DashNormalGotoPoint(point, 0.5, wm->getBody(), 60).getCommand();
		else
			command = new TurnCommand(80);
	}
		break;

	case PM_PENALTY_SETUP:
	case PM_PENALTY_SETUP_OPP:
	case PM_PENALTY_READY:
	case PM_PENALTY_READY_OPP:
	{
		LOGN << "Penalty" << endl;
		Penalty penalty(wm);
		penalty.execute(form);
		command = penalty.getCommand();
	}

	case PM_TIME_UP_WITHOUT_A_TEAM:
	case PM_TIME_UP:
	default:
		break;
	}
	//LOGN<<"the command is :"<<command->toString()<<endl;
}

Command* NonPlayOnDecision::getCommand()
{
	return command;
}

bool NonPlayOnDecision::isStaminaEnough()
{
	for (unsigned i = 0; i < FULL_PLAYERS_NUM; i++)
		if (wm->getFullPlayer(TID_TEAMMATE, i).isValid()
				and wm->getFullPlayer(TID_TEAMMATE, i).getStamina() < 3999)
		{
			LOGN << "Bad Stamina Player: " << i + 1 << endl;
			LOGN << "D: " << wm->getFullPlayer(TID_TEAMMATE, i).getStaminaMax() << endl;
			return false;
		}

	return true;
}

bool NonPlayOnDecision::areAllPlayersReady()
{
	if (not isStaminaEnough())
		return false;

	return true;
}

bool NonPlayOnDecision::isAllPlayersReady()
{
	if (!isStaminaEnough())
		return false;

	for (unsigned i = 1; i < FULL_PLAYERS_NUM; i++)
		if (wm->getFullPlayer(TID_TEAMMATE, i).getDistance(positions[i]) > 2
				and i + 1 != wm->getBody().getUniNum()
				and positions[i].getDistance(wm->getBody().getPos()) < 40)
		{
			LOGN << "Bad Player: " << i + 1 << endl;
			LOGN << "Distance: " << wm->getFullPlayer(TID_TEAMMATE, i).getDistance(positions[i])
					<< endl;
			return false;
		}

	return true;
}

#undef LOGN
