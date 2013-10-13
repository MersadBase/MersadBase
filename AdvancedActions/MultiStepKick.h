#ifndef __MULTI_STEP_KICK_H_
#define __MULTI_STEP_KICK_H_

#include <WorldModel.h>
#include <ChainAction.h>

class MultiStepKick
{
	const WorldModel * wm;
	Body firstBody;
	Ball firstBall;

	Point target;
	float maxSpeed;
	unsigned steps;
	float kickableArea;

	bool isCalculated;

	ChainAction chain[3][3];
	float bestSpeed[3];
	Ball bestBall[3];
	float maxAvailableSpeed;
	unsigned maxAvailableStep;

	ChainAction finalChain[3];
	float finalSpeed;
	unsigned finalStep;
	Ball finalBall;
	Ball finalBallWithoutDynamics;
	float desiredSpeed;

public:
	MultiStepKick(const WorldModel * wm,const Body &body ,const Ball &ball,Point target, float speed, unsigned steps);

	void setTarget(Point _target);
	float calculate();
	bool isBetter(float vel,unsigned step);
	static float kickToPointVelocity(const Body &body,const Ball &ball,Point target,float maxSpeed,KickCommand &c);

	//utility
	bool moveToPoint(const Body &body,const Ball &ball,Point target,KickCommand &c)const;
	bool moveToPolar(const Body &body,const Ball &ball,float r,float direction,KickCommand &c)const;
	bool stopBall(const Body &body,const Ball &ball,KickCommand &c)const;
	bool willRemainInKickable(const Body &body,const Ball &ball)const;
	bool canHoldInKickable(const Body &body,const Ball &ball)const;

	static float getKickPower(const Body &body,const Ball &ball,float speed);

	unsigned getFinalStep()const;//steps needed for the desiredSpeed
	unsigned getMaxAvailableStep()const;//steps needed for the maxAvailabeSpeed;
	float getMaxAvailableSpeed(int step = -1)const;
	Ball getFinalBall()const;//right after the last kick with the desiredSpeed
	Ball getFinalBallWithoutDynamics() const;//right after the last kick with the desiredSpeed but before simulating by dynamic the last cycle
	void setDesiredSpeed(float speed);//age nakhaim ba maxAvailabeSpeed beshootim ino set mikonim.
	Command* getCommand(unsigned step=0)const;
	ChainAction getChain(unsigned step=0)const;
};

class HoldBall
{
	const WorldModel * wm;
	Command * command;

public:
	HoldBall(const WorldModel * worldModel);
	bool checkOppKickable(const WorldModel * worldModel, Vector ballPos,
			float & minOppDist);
	bool decide(unsigned cycle = 0);
	Command * getCommand();
};

#endif

