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
 *  This file is created by: Mohammad Amin Khashkhashi Moghaddam
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#include <cmath>
#include <iostream>
#include <Intercept.h>
#include <Ball.h>
#include <Degree.h>
#include <Basics.h>
#include <Logger.h>
#include <BasicDash.h>
#include <BasicTurn.h>
#include <WorldModel.h>
#include <HeadDecision.h>
#include <AdvancedAgent.h>

#define LOGI LOG

using namespace std;
using namespace Basics;
using namespace Degree;

std::ostream &operator<<(std::ostream &stream, const InterceptType &entry) {
	std::string str[] = { "WAIT_INTERCEPT", "FORWARD_DASH_INTERCEPT",
			"BACK_DASH_INTERCEPT", "TURN_INTERCEPT", "SIDE_DASH_INTERCEPT",
			"STOP_TURN_INTERCEPT", "ADJUSTED_DASH_INTERCEPT" };
	stream << str[entry];
	return stream;
}

Intercept::Intercept(const WorldModel *wm) :
		wm(wm) {
	firstBall = wm->getBall();
	firstBody = wm->getBody();
}

void Intercept::execute(Form &form) {
	LOGI<< "Intercept::execute" << endl;
	form.headForm.tnMode = TNM_LOOK_CAREFULLY_TO_BALL;
	form.comment.addComment("InterceptAdvAct");
}

void Intercept::calculate() {
	if (wm->getCurCycle() == 50) {
		LOGI<< "BOZ" <<endl;
	}
	const int MAXCYCLE = 100;
	InterceptEntry bestEntry;
	bestEntry.reachCycle = MAXCYCLE * 2;
	Point ballPosCache[MAXCYCLE]; //toop koja mire
	Point bodyPosCache[MAXCYCLE];//age player ro vel konim koja mire
	float playerDistCache[MAXCYCLE];//age hey dash konim cheghadr mirim jolo stamina dar nazar gerefte mishe
	LOGI<< "Filling Caches" << endl;
	{
		//filling Caches
		Body tBody = firstBody;
		tBody.setVel() = Vector(0, 0);
		tBody.setPos() = Vector(0, 0);
		tBody.setBodyDir(0);
		ballPosCache[0] = firstBall.getPos();
		bodyPosCache[0] = firstBody.getPos();
		playerDistCache[0] = 0;
		Vector ballVel = firstBall.getVel();
		Vector bodyVel = firstBody.getVel();
		for (int i = 1; i < MAXCYCLE; i++) {
			float dashPower = 100;
			dashPower = wm->getSafetyDashPower(dashPower, tBody.getStamina());
			DashCommand dashX(dashPower);
			tBody.simulateByAction(&dashX);
			tBody.simulateByDynamics();
			playerDistCache[i] = tBody.getPos().getX();

			ballPosCache[i] = ballPosCache[i - 1];
			ballPosCache[i] += ballVel;
			ballVel *= firstBall.getDecay();

			bodyPosCache[i] = bodyPosCache[i - 1];
			bodyPosCache[i] += bodyVel;
			bodyVel *= firstBody.getDecay();
		}
	}
	LOGI<< "Filling Finished" << endl;
	LOGI<< "Turn and Stop Check" << endl;
	const int dirs[] = {0, 45, 90, 135, 180, -45, -90, -135};
	for (int i = 1; i < MAXCYCLE; i++) {

		if (firstBody.getVel().getMagnitude() > 0.05) { //Stop Then Turn Then Dash
			int cycleLeft = i - 1;//ye cycle bara stop hadar mire
			float velDir = firstBody.getVel().getDirection()
			- firstBody.getBodyDir();
			int bj = 0;
			for (int j = 1; j < 8; j++)//finding best direction for tormoz
			{
				if (fabs(normalizeAngle(velDir - dirs[j]))
						< fabs(normalizeAngle(velDir - dirs[bj])))
				bj = j;
			}
			if (fabs(normalizeAngle(velDir - dirs[bj]) < 10)) // badan hazfesh kon
			{
				int revDir = normalizeAngle(dirs[bj] + 180);
				cycleLeft--; //ye cycle ham bara turn hadar mire
				Vector vv;
				vv.setAsPolar(firstBody.getVel().getMagnitude(),
						fabs(normalizeAngle(velDir - dirs[bj])));
				float velNeed = vv.getX();
				float powerNeed = velNeed
				/ (firstBody.getDashDirRate(dirs[bj])
						* firstBody.getDashPowerRate()
						* firstBody.getEffort());
				powerNeed = min(powerNeed, 100.f);
				Body stopBody = firstBody;
				DashCommand stopCommand(powerNeed, -revDir);//zavie ha too server barakse
				stopBody.simulateByAction(&stopCommand);
				stopBody.simulateByDynamics();
				//khob ye tormoz zadim hala bayad zavie ro dorost konim
				const float turnInertia = stopBody.getTurnInertia();
				const float maxDeltaDir = 180.f / turnInertia;
				Vector bodyDir = ballPosCache[i] - stopBody.getPos();//directioni ke badanam bayad bashe
				float deltaDir = normalizeAngle(
						bodyDir.getDirection() - firstBody.getBodyDir());
				if (fabs(deltaDir) > maxDeltaDir) {
					//zereshk ye tormoz zadim bazam nashod ba ye turn dorostesh konam sag khord ye turn dige ham mikonam mishe goft dige soratemoon 0 hast
					cycleLeft--;
					stopBody.simulateByDynamics();//daram alan turn mikonam
				}
				if (cycleLeft > 0
						&& ballPosCache[i].getDistance(stopBody.getPos())
						< playerDistCache[cycleLeft]
						+ wm->getKickableArea()) //in kar ghalate chon stamina ye cycle inja hefz shode
				{
//					LOGI << "Adding Stop Intercept Entry WIth Reach Cycle : " << i << endl;
					InterceptEntry entry;
					entry.reachCycle = i;
					entry.type = STOP_TURN_INTERCEPT;
					entry.inKickable = playerDistCache[cycleLeft]
					+ wm->getKickableArea()
					- ballPosCache[i].getDistance(stopBody.getPos());
					entry.command = new DashCommand(powerNeed, -revDir);
					if (entry < bestEntry)
					bestEntry = entry;
				}
			}
		}
		{ //turn without stop
			const float turnInertia = firstBody.getTurnInertia();
			const float maxDeltaDir = 180.f / turnInertia;
			Vector bodyDir = ballPosCache[i] - bodyPosCache[i];//directioni ke badanam bayad bashe
			float deltaDir = normalizeAngle(
					bodyDir.getDirection() - firstBody.getBodyDir());
			if (fabs(deltaDir) < maxDeltaDir && fabs(deltaDir) > 5) {
				//mitoonam badanamo oon ghadr becharkhoonam
//				LOGI << " Delta Dir : " << fabs(deltaDir) << endl;
				if (ballPosCache[i].getDistance(bodyPosCache[i])
						< playerDistCache[i - 1] + wm->getKickableArea()) {
					InterceptEntry entry;
//					LOGI << " Adding Entry with Reach Cycle : " << i << endl;
					entry.reachCycle = i;
					entry.type = TURN_INTERCEPT;
					entry.inKickable = playerDistCache[i - 1]
					+ wm->getKickableArea()
					- ballPosCache[i].getDistance(bodyPosCache[i]);
					entry.command = new TurnCommand(deltaDir * turnInertia);
					if (entry < bestEntry)
					bestEntry = entry;
				}
			}
		}
	}
	LOGI<< "Checking 8 Directions" << endl;
	vector<int> adjustDirs;
	{ //checking each 8 directions for some cycles
		int mc[] = {100, 8, 8, 8, 8, 8, 8, 8}; // max cycle for each direction
//		{ 100, 10, 50, 10, 100, 10, 50, 10 }; // max cycle for each direction
		for (int i = 0; i < 8; i++) {
			Body tBody = firstBody;
			Body waitBody = firstBody;
			for (int j = 1; j <= mc[i]; j++) {
				float dashPower = wm->getSafetyDashPower(100,
						tBody.getStamina());
				DashCommand dashX(dashPower, dirs[i]);
				tBody.simulateByAction(&dashX);
				if (j > 1) {
					float waitPower = wm->getSafetyDashPower(100,
							waitBody.getStamina());
					DashCommand dashX2(waitPower, dirs[i]);
					waitBody.simulateByAction(&dashX2);
				}
				tBody.simulateByDynamics();
				waitBody.simulateByDynamics();
				Segment seg(waitBody.getPos(), bodyPosCache[j]);
				if (seg.getDist(ballPosCache[j]) < wm->getKickableArea()
						&& j == 2) //in j==2 chize khoobi nist bayad dorost she
				{
					InterceptEntry entry;
					entry.reachCycle = j;
					entry.type = WAIT_INTERCEPT;
					entry.inKickable = wm->getKickableArea()
					- seg.getDist(ballPosCache[j]);
					entry.command = new EmptyCommand();
					if (entry < bestEntry)
					bestEntry = entry;
				}
				Segment seg2(tBody.getPos(), bodyPosCache[j]);
				if (seg2.getDist(ballPosCache[j]) < wm->getKickableArea()) {
					InterceptEntry entry;
					entry.reachCycle = j;
					if (dirs[i] == 0)
					entry.type = FORWARD_DASH_INTERCEPT;
					else if (dirs[i] == 180)
					entry.type = BACK_DASH_INTERCEPT;
					else
					entry.type = SIDE_DASH_INTERCEPT;
					dashPower = 100;
					dashPower = wm->getSafetyDashPower(dashPower,
							firstBody.getStamina());
					entry.command = new DashCommand(dashPower, dirs[i]);
					entry.inKickable = wm->getKickableArea()
					- seg2.getDist(ballPosCache[j]);
					if (entry < bestEntry)
					bestEntry = entry;
					if (j == 1)
					adjustDirs.push_back(dirs[i]);
					break;
				}
			}
		}
	}
	if (adjustDirs.size()) //we are gonna get the ball next cycle so it's important how to dash so the ball is in the best position relative to our body
	{
		bestEntry.type = ADJUSTED_DASH_INTERCEPT;
		float bestAccel = -1;
		for (int i = 0; i < adjustDirs.size(); i++) {
			int dir = adjustDirs[i];
			Ball nextBall = firstBall;
			nextBall.simulateByDynamics(firstBody);
			for (int power = 0; power <= 100; power += 5) {
				Body tBody = firstBody;
				double finalPower = wm->getSafetyDashPower(power,
						firstBody.getStamina());
				DashCommand dash(finalPower, dir);
				tBody.simulateByAction(&dash);
				tBody.simulateByDynamics();
				float accel = tBody.getBallAccelMax(nextBall);
				if (accel > bestAccel) {
					bestAccel = accel;
					bestEntry.command = new DashCommand(finalPower, dir);
					bestEntry.inKickable = wm->getKickableArea()
					- tBody.getPos().getDistance(nextBall.getPos());
				}
			}
		}
	}
	command = NULL;
	if (bestEntry.reachCycle <= MAXCYCLE) {
		command = bestEntry.command;
		interceptPoint = ballPosCache[bestEntry.reachCycle];
		interceptTime = bestEntry.reachCycle;
		LOGI<< "Best Candidate Reach Cycle : " << bestEntry.reachCycle << endl;
		LOGI<< "Best Candidate Reach Type : " << bestEntry.type << endl;
		LOGI<< "Best Candidate inKickable : " << bestEntry.inKickable << endl;
		LOGI<< "Best Candidate Reach Point : "<<interceptPoint<<endl;
	}
	else
	LOGI << "CRAP!!! THERE IS NO CANDIDATE" << endl;
}

void Intercept::getValue() {
	calculate();
}

Point Intercept::getInterceptPoint() const {
	return interceptPoint;
}

unsigned Intercept::getInterceptTime() const {
	return interceptTime;
}

Command *Intercept::getCommand() {
	return command;
}
