#include <MultiStepKick.h>
#include <Degree.h>
#include <Circle.h>
#include <HalfLine.h>
#include <Basics.h>
#include <Command.h>
#include <Logger.h>
#include <BasicKick.h>
#include <Share.h>

//#define MULTI_STEP_LOG

using namespace std;
using namespace Degree;
using namespace Basics;

MultiStepKick::MultiStepKick(const WorldModel * wm, const Body &body, const Ball &ball,
		Point target, float speed, unsigned steps) :
		wm(wm), firstBody(body), firstBall(ball), target(target), maxSpeed(speed), steps(steps)
{
	maxSpeed = min(maxSpeed, ball.getSpeedMax());
	kickableArea = wm->getKickableArea();
	isCalculated = false;
	for (unsigned i = 0; i < 3; i++)
	{
		bestSpeed[i] = 0;
		for (unsigned j = 0; j < 3; j++)
			chain[i][j].wm = wm;
	}
	if (this->steps < 1)
	{
		this->steps = 1;
		LOG << "**WARNING*********Ye Ravanii Be Man Stepo kamtar az yek dade*****WARNING*******"
				<< endl;
	}
	desiredSpeed = -1;
}

void MultiStepKick::setTarget(Point _target)
{
	target = _target;
}

bool MultiStepKick::isBetter(float vel, unsigned step)
{
	if (vel > maxAvailableSpeed)
	{
		maxAvailableSpeed = vel;
		maxAvailableStep = step;
	}
	if (vel > bestSpeed[step - 1])
	{
		bestSpeed[step - 1] = vel;
		return true;
	}
	return false;
}

float MultiStepKick::calculate()
{
//	vector<Command*> c;
//	float maxVel = getMaxDirectionVelocity(firstBody, firstBall, target, steps, c);
//	for (int i = 0; i < c.size(); i++)
//		command[0][i] = c[i];
//	return maxVel;
	if (isCalculated)
		return maxAvailableSpeed;
	isCalculated = true;
	KickCommand tempCommand(0, 0);
	KickCommand tempCommand2(0, 0);
	if (steps >= 1) //hatman hast
	{
		bestSpeed[0] = kickToPointVelocity(firstBody, firstBall, target, maxSpeed, tempCommand);
		bestBall[0] = firstBall;
//		bestBall[0].simulateByAction(firstBody, &tempCommand);
//		bestBall[0].simulateByDynamics(firstBody);
		//command[0] = tempCommand;
		chain[0][0].type = CA_MULTISTEP;
		chain[0][0].dir = tempCommand.getDirection();
		chain[0][0].power = tempCommand.getPower();
		maxAvailableSpeed = bestSpeed[0];
		maxAvailableStep = 1;
	}
	if (!canHoldInKickable(firstBody, firstBall) or maxSpeed == maxAvailableSpeed)
	{
		setDesiredSpeed(maxAvailableSpeed);
		return maxAvailableSpeed;
	}

	//some usual vars:
	float tempVel;
	Body nextBody = firstBody;
	nextBody.simulateByDynamics();
	Body next2Body = nextBody;
	next2Body.simulateByDynamics();
//	const Vector ballToTarget = target - firstBall.getPos();
	const Vector bodyToTarget = target - firstBody.getPos();
	const Vector nextBodyToTarget = target - nextBody.getPos();
	const Vector next2BodyToTarget = target - next2Body.getPos();

	if (steps >= 2)
	{
		//aval turn mikonam bad mishootam
		if (willRemainInKickable(firstBody, firstBall))
		{
			Body secondBody = firstBody;
			secondBody.simulateByAction(TurnToPoint(target, firstBody).getCommand());
			secondBody.simulateByDynamics();
			Ball secondBall = firstBall;
			secondBall.simulateByDynamics(firstBody);
			tempVel = kickToPointVelocity(secondBody, secondBall, target, maxSpeed, tempCommand);
			if (!secondBall.isOut() and isBetter(tempVel, 2))
			{
				bestBall[1] = secondBall;
//				bestBall[1].simulateByAction(nextBody, &tempCommand);
//				bestBall[1].simulateByDynamics(nextBody);
				chain[1][0].type = CA_TURN;
				chain[1][0].point = target;
				chain[1][1].type = CA_MULTISTEP;
				chain[1][1].dir = tempCommand.getDirection();
				chain[1][1].power = tempCommand.getPower();

			}
		}
		//aval mibaram jolo khodam bad mishootam
		if (moveToPolar(firstBody, firstBall, 0.6, 0, tempCommand)) //yani mitoonam inkaro bokonam
		{
			Ball secondBall = firstBall;
			secondBall.simulateByAction(firstBody, &tempCommand);
			secondBall.simulateByDynamics(firstBody);
			tempVel = kickToPointVelocity(nextBody, secondBall, target, maxSpeed, tempCommand2);
			if (!secondBall.isOut() and isBetter(tempVel, 2))
			{
				bestBall[1] = secondBall;
//				bestBall[1].simulateByAction(nextBody, &tempCommand2);
//				bestBall[1].simulateByDynamics(nextBody);
				chain[1][0].type = CA_MULTISTEP;
				chain[1][0].dir = tempCommand.getDirection();
				chain[1][0].power = tempCommand.getPower();
				chain[1][1].type = CA_MULTISTEP;
				chain[1][1].dir = tempCommand2.getDirection();
				chain[1][1].power = tempCommand2.getPower();
			}
		}
		//toopo mibaram be samte target bad mishootam
		if (moveToPolar(firstBody, firstBall, wm->getKickableArea() - 0.3,
				bodyToTarget.getDirection() - firstBody.getBodyDir(), tempCommand))
		{
			Ball secondBall = firstBall;
			secondBall.simulateByAction(firstBody, &tempCommand);
			secondBall.simulateByDynamics(firstBody);
			tempVel = kickToPointVelocity(nextBody, secondBall, target, maxSpeed, tempCommand2);
			if (!secondBall.isOut() and isBetter(tempVel, 2))
			{
				bestBall[1] = secondBall;
//				bestBall[1].simulateByAction(nextBody, &tempCommand2);
//				bestBall[1].simulateByDynamics(nextBody);
				chain[1][0].type = CA_MULTISTEP;
				chain[1][0].dir = tempCommand.getDirection();
				chain[1][0].power = tempCommand.getPower();
				chain[1][1].type = CA_MULTISTEP;
				chain[1][1].dir = tempCommand2.getDirection();
				chain[1][1].power = tempCommand2.getPower();
			}
		}
	}

	if (maxSpeed == maxAvailableSpeed) //mage marizam edame bedam
	{
		setDesiredSpeed(maxAvailableSpeed);
		return maxAvailableSpeed;
	}

	if (steps >= 3)
	{
		for (float ang = nextBodyToTarget.getDirection() - 180;
				ang < nextBodyToTarget.getDirection() + 180; ang += 30)
		{
//			Vector v;
//			v.setAsPolar(r, nextBodyToTarget.getDirection() + 90 * sign);
//			v = nextBody.getPos() + v;
//			Circle c;
//			c.setByCenterRadius(nextBody.getPos(), r + 0.1);
//			Line l;
//			l.setByPoints(v, target);
//			vector<Point> points;
//			//cout<<c.getLineIntersect(l,points)<<endl;
//			l.getCircleIntersect(c, points);
//			if (points.size() < 2)
//				LOG << "RIIIIIIIIIIIIIIIIIIIIIIDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMMMMMMMMMMM"
//						<< endl;
//			Point p1 = points[0], p2 = points[1];
//			if (p1.getDistance(target) < p2.getDistance(target))
//				swap(p1, p2);
			Vector v;
			v.setAsPolar(0.6, ang);
			if (moveToPoint(firstBody, firstBall, v + nextBody.getPos(), tempCommand))
			{
				Ball secondBall = firstBall;
				secondBall.simulateByAction(firstBody, &tempCommand);
				secondBall.simulateByDynamics(firstBody);
				if (secondBall.isOut())
					continue;
				for (float ang2 = next2BodyToTarget.getDirection() - 90;
						ang2 <= next2BodyToTarget.getDirection() + 90; ang2 += 30)
				{
					Vector v2;
					v2.setAsPolar(wm->getKickableArea() - 0.3, ang2);
					if (moveToPoint(nextBody, secondBall, v2 + next2Body.getPos(), tempCommand2)) //if (moveToPoint(nextBody, vBall, p2, tempCommand2))
					{
						Ball thirdBall = secondBall;
						thirdBall.simulateByAction(nextBody, &tempCommand2);
						thirdBall.simulateByDynamics(nextBody);
						if (thirdBall.isOut())
							continue;
						KickCommand tempCommand3(0, 0);
						tempVel = kickToPointVelocity(next2Body, thirdBall, target, maxSpeed,
								tempCommand3);
						if (isBetter(tempVel, 3))
						{
							bestBall[2] = thirdBall;
//							bestBall[2].simulateByAction(next2Body, &tempCommand3);
//							bestBall[2].simulateByDynamics(next2Body);
							chain[2][0].type = CA_MULTISTEP;
							chain[2][0].dir = tempCommand.getDirection();
							chain[2][0].power = tempCommand.getPower();
							chain[2][1].type = CA_MULTISTEP;
							chain[2][1].dir = tempCommand2.getDirection();
							chain[2][1].power = tempCommand2.getPower();
							chain[2][2].type = CA_MULTISTEP;
							chain[2][2].dir = tempCommand3.getDirection();
							chain[2][2].power = tempCommand3.getPower();
						}
					}
				}
			}
		}
	}
	setDesiredSpeed(maxAvailableSpeed);
	return maxAvailableSpeed;

}

float MultiStepKick::kickToPointVelocity(const Body &body, const Ball &ball, Point target,
		float maxSpeed, KickCommand &c)
{
	Vector v2 = ball.getVel();
	float dir = (target - ball.getPos()).getDirection();
	v2.rotate(-dir);
	float deltaY = v2.getAbsY();
	float tempMaxAccel = body.getBallAccelMax(ball);
	float deltaX = tempMaxAccel * tempMaxAccel - deltaY * deltaY;
	c = KickCommand(100, Degree::normalizeAngle(dir - body.getBodyDir()));
	if (deltaX < 0)
		return false;
	deltaX = sqrt(deltaX);
	if (deltaX + v2.getX() < 0)
	{
//		delete c;
//		v2.setAsPolar(-(deltaX + v2.getX()), 180+dir);
//		Vector vel = v2 - ball.getVel();
//		moveToPoint(body,ball,target,c);
		return false;
	}
	float speed = deltaX + v2.getX();
	if (speed > maxSpeed)
		speed = maxSpeed;
	if (speed > ball.getSpeedMax())
		speed = ball.getSpeedMax();
	v2.setAsPolar(speed, dir);
	Vector vel = v2 - ball.getVel();
	vel.setAsPolar(min_max(0.f, vel.getMagnitude(), ball.getSpeedMax()), vel.getDirection());
	float kickPower = getKickPower(body, ball, vel.getMagnitude());
	c = KickCommand(kickPower, Degree::normalizeAngle(vel.getDirection() - body.getBodyDir()));
	return speed;
}

bool MultiStepKick::moveToPoint(const Body &body, const Ball &ball, Point target,
		KickCommand &c) const
{
	Ball vBall = ball;
	vBall.simulateByDynamics(body);
	Vector vel = target - vBall.getPos();
	float speed = getKickPower(body, ball, vel.getMagnitude());
	c = KickCommand(min_max(0.f, speed, 100.f),
			Degree::normalizeAngle(vel.getDirection() - body.getBodyDir()));
	if (speed > 100 or speed < 0) //speed<0 never happens
		return false;
	return true;
}

bool MultiStepKick::moveToPolar(const Body &body, const Ball &ball, float r, float direction,
		KickCommand &c) const
{
	Vector v;
	v.setAsPolar(r, direction);
	Body vBody = body;
	vBody.simulateByDynamics();
	v.rotate(vBody.getBodyDir());
	v = v + vBody.getPos();
	return moveToPoint(body, ball, v, c);
}

bool MultiStepKick::stopBall(const Body &body, const Ball &ball, KickCommand &c) const
{
	return moveToPoint(body, ball, ball.getPos(), c);
}

bool MultiStepKick::willRemainInKickable(const Body &body, const Ball &ball) const
{
	Point nextBody = body.getPos() + body.getVel();
	Point nextBall = ball.getPos() + ball.getVel();
	if (nextBall.getDistance(nextBody) < kickableArea)
		return true;
	return false;
}

bool MultiStepKick::canHoldInKickable(const Body &body, const Ball &ball) const
{
	Point nextBody = body.getPos() + body.getVel();
	Point nextBall = ball.getPos() + ball.getVel();
	float maxAccel = body.getBallAccelMax(ball);
	if (nextBall.getDistance(nextBody) < kickableArea + maxAccel)
		return true;
	return false;
}

float MultiStepKick::getKickPower(const Body &body, const Ball &ball, float speed)
{
	float effectiveKickPower = ball.getEffectiveKickPower(body);
	float kickPowerRate = ball.getKickPowerRate();
	float kickPower;
	kickPower = speed / (kickPowerRate * effectiveKickPower);
//	kickPower = min_max(0,kickPower,100);
	return kickPower;
}

unsigned MultiStepKick::getFinalStep() const
{
	return finalStep;
}

unsigned MultiStepKick::getMaxAvailableStep() const
{
	return maxAvailableStep;
}

float MultiStepKick::getMaxAvailableSpeed(int step) const //step= [1,3]
{
	if (step == -1)
		return maxAvailableSpeed;
	else
		return bestSpeed[step - 1];
}

Ball MultiStepKick::getFinalBall() const
{
	return finalBall;
}

Ball MultiStepKick::getFinalBallWithoutDynamics() const
{
	return finalBallWithoutDynamics;
}

void MultiStepKick::setDesiredSpeed(float speed)
{
	KickCommand tempCommand(0, 0);
	if (speed > maxAvailableSpeed)
	{
		LOG << "*****WARNING**********Nemitoonam Ba In Speed Bezanam**********WARNING*********"
				<< endl;
		return;
	}
	if (speed == desiredSpeed)
		return; //no need to change
	desiredSpeed = speed;
	if (speed <= bestSpeed[0])
	{
		finalSpeed = speed;
		finalStep = 1;
		finalBall = firstBall;
		kickToPointVelocity(firstBody, firstBall, target, speed, tempCommand);
		finalBall.simulateByAction(firstBody, &tempCommand);
		finalBallWithoutDynamics = finalBall;
		finalBall.simulateByDynamics(firstBody);
		finalChain[0].type = CA_MULTISTEP;
		finalChain[0].dir = tempCommand.getDirection();
		finalChain[0].power = tempCommand.getPower();
	}
	else if (speed <= bestSpeed[1])
	{
		finalSpeed = speed;
		finalStep = 2;
		finalBall = bestBall[1];
		finalChain[0] = chain[1][0];
		Body nextBody = firstBody;
		nextBody.simulateByDynamics();
		kickToPointVelocity(nextBody, finalBall, target, speed, tempCommand);
		finalBall.simulateByAction(nextBody, &tempCommand);
		finalBallWithoutDynamics = finalBall;
		finalBall.simulateByDynamics(nextBody);
		finalChain[1].type = CA_MULTISTEP;
		finalChain[1].dir = tempCommand.getDirection();
		finalChain[1].power = tempCommand.getPower();
	}
	else
	{
		finalSpeed = speed;
		finalStep = 3;
		finalBall = bestBall[2];
		finalChain[0] = chain[2][0];
		finalChain[1] = chain[2][1];
		Body next2Body = firstBody;
		next2Body.simulateByDynamics();
		next2Body.simulateByDynamics();
		kickToPointVelocity(next2Body, finalBall, target, speed, tempCommand);
		finalBall.simulateByAction(next2Body, &tempCommand);
		finalBallWithoutDynamics = finalBall;
		finalBall.simulateByDynamics(next2Body);
		finalChain[2].type = CA_MULTISTEP;
		finalChain[2].dir = tempCommand.getDirection();
		finalChain[2].power = tempCommand.getPower();
	}
}

Command* MultiStepKick::getCommand(unsigned step) const
{
	ChainAction ca = getChain(step);
	ca.wm=wm;
	ca.calCommand();
	return ca.getCommand();
}

ChainAction MultiStepKick::getChain(unsigned step) const
{
	return finalChain[step];
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////New Multi Step Kick By MAKM/////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

HoldBall::HoldBall(const WorldModel * worldModel) :
		wm(worldModel)
{
}

bool HoldBall::checkOppKickable(const WorldModel * worldModel, Vector ballPos, float & minOppDist)
{
	float extraOpponentKickable = worldModel->getBall().getSize() + worldModel->getBody().getSize();
	for (int i = 0; i < 11; i++)
	{
		const Player * opponent = &worldModel->getFullPlayer(TID_OPPONENT, i);
		if (opponent->getSimCounter() > 3)
		{
			continue;
		}
		if (opponent->getDistance(ballPos) > 6)
		{
			continue;
		}
		if (opponent->getTackleRemainCycle() > 3) ///TEMP
		{
			continue;
		}
		float oppKickable = extraOpponentKickable + opponent->getKickableMargin();
		float distToBall = ballPos.getDistance(opponent->getPos() + opponent->getVel());
		if (opponent->getVel().getMagnitude()
				> opponent->getSpeedMax() / (opponent->getDecay() * 2))
		{
			distToBall = max(
					distToBall,
					ballPos.getDistance(
							opponent->getPos() + (opponent->getVel() * opponent->getDecay())));
		}
		if (distToBall < minOppDist)
		{
			minOppDist = distToBall;
		}
		if (distToBall < oppKickable + 0.5)
		{
			return true;
		}
	}
	for (int i = 0; i < 30; i++)
	{
		const Player * opponent = &worldModel->getQuarterPlayer(i);
		if (opponent->getSimCounter() > 3)
		{
			continue;
		}
		if (opponent->getDistance(ballPos) > 6)
		{
			continue;
		}
		float oppKickable = extraOpponentKickable + opponent->getKickableMargin();
		float distToBall = ballPos.getDistance(opponent->getPos() + opponent->getVel());
		if (distToBall < minOppDist)
		{
			minOppDist = distToBall;
		}
		if (distToBall < oppKickable + 0.2)
		{
			return true;
		}
	}
	return false;
}

bool HoldBall::decide(unsigned cycle)
{
	Vector bodyToBall = wm->getBall().getPos() - wm->getBody().getPos();
	bodyToBall.rotate(-wm->getBody().getBodyDir());
	float ballDist = bodyToBall.getMagnitude();
	float ballAngle = bodyToBall.getDirection();
	float kickRate = wm->getServerParam()["kick_power_rate"].asFloat()
			* (1.f - 0.25f * fabs(ballAngle) / 180.f
					- 0.25 * (ballDist - wm->getBall().getSize() - wm->getBody().getSize())
							/ wm->getBody().getKickableMargin());
	float kickableArea = wm->getBody().getSize() + wm->getBall().getSize()
			+ wm->getBody().getKickableMargin() - 0.055f;
	float maxAccel = min(wm->getBall().getAccelMax(),
			wm->getServerParam()["maxpower"].asFloat() * kickRate);
	float kickDist = max(kickableArea * 0.6, kickableArea - 0.3);
	float maxDist = -1.f;
	float bestDir = normalizeAngle(wm->getBall().getVel().getDirection() + 180.f);
	float bestPower = 100.f;

	Body nextBody = wm->getBody();
	nextBody.simulateByDynamics();
	float makhraj = 1;
	float decay = wm->getBall().getDecay();
	for (unsigned c = 0; c < cycle; c++)
	{
		nextBody.simulateByDynamics();
		makhraj += decay;
		decay *= decay;
	}

	vector<float> mag;
	mag.push_back(kickDist);
	for (float i = kickDist + 0.1; i <= kickableArea - 0.08; i += 0.1)
		mag.push_back(i);
	for (float i = kickDist - 0.1; i >= 0.2; i -= 0.1)
		mag.push_back(i);
	bool kickFound = false;
	for (unsigned i = 0; i < mag.size(); i++)
	{
		if (kickFound)
			break; //no need to check higher kickDist which are risky because it's always worse than the thing we found
		for (float direction = 0.f; direction < 360.f; direction += 10.f)
		{
			Vector nextBallPos;
			nextBallPos.setAsPolar(mag[i], direction);
			nextBallPos += nextBody.getPos();
			const float fieldMargin = -0.5;
			if (!wm->isPointInField(nextBallPos, fieldMargin))
			{ //toop mire out bishoor
				continue;
			}
			Vector vel = (nextBallPos - wm->getBall().getPos()) / makhraj; //vel + vel*decay + vel*decay^2 + ...
			if (vel.getMagnitude() > wm->getBall().getSpeedMax())
			{
				vel.setAsPolar(wm->getBall().getSpeedMax(), vel.getDirection());
				nextBallPos = wm->getBall().getPos() + vel;
			}
			Vector accel = (vel - wm->getBall().getVel());
			if (maxAccel < accel.getMagnitude())
			{
				continue;
			}
			if (nextBody.getPos().getDistance(nextBallPos) > kickableArea - 0.15)
			{
				continue;
			}
			float minDist = 1000.f;
			checkOppKickable(wm, nextBallPos, minDist); //if removed because we should choose the best even if it is worst
			if (minDist > maxDist)
			{
				kickFound = true;
				maxDist = minDist;
				bestPower = accel.getMagnitude() / kickRate;
				bestDir = accel.getDirection() - wm->getBody().getBodyDir();
			}
		}
	}
	if (!kickFound)
	{
		LOG << "Emergency HoldBall" << endl;
		bestDir = 180 + wm->getBall().getVel().getDirection() - wm->getBody().getBodyDir();
		bestPower = maxAccel / kickRate;
	}
	bestDir = normalizeAngle(bestDir);
	LOG << "Best Power = " << bestPower << " Best Dir = " << bestDir << endl;
	command = new KickCommand(bestPower, bestDir);
	return kickFound;
}

Command * HoldBall::getCommand()
{
	return command;
}

