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
 *  This file is created by: Mostafa Rokooey
 *  	and is modified by: Pooria Kaviani, Mohammad Zareie
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <BasicKick.h>
#include <Body.h>
#include <Ball.h>
#include <Degree.h>
#include <Logger.h>
#include <Defines.h>
#include <Circle.h>

using namespace std;
using namespace Degree;

// class KickToDirectionVelocity

KickToDirectionVelocity::KickToDirectionVelocity(float direction,
		float velocity, const Ball &ball, const Body &body, float possibleArea): BasicKick(),
		direction(direction), velocity(velocity), ball(ball),
		body(body), possibleArea(possibleArea)
{
	float ballStopLength;

	kickVector.setAsPolar(velocity, direction);

	trajectoryVector = kickVector;

	kickVector = kickVector - ball.getVel();

	kickVector.rotate(-body.getBodyDir());
	ballStopLength = (kickVector.getMagnitude()) / (1 - ball.getDecay());

	kickVector.setAsPolar(needKick(kickVector.getMagnitude(), body , ball),
			kickVector.getDirection());

	kickVector.setAsPolar(kickVector.getMagnitude() /
			ball.getKickPowerRate(), kickVector.getDirection());

	if (kickVector.getMagnitude() > (100 + possibleArea))
		possibleFlag = false;
	else
		possibleFlag = true;

	trajectoryVector.setAsPolar(ballStopLength, trajectoryVector.getDirection());

	kickCommand = new KickCommand(kickVector.getMagnitude(),
			Degree::normalizeAngle(kickVector.getDirection()));
}

Command *KickToDirectionVelocity::getCommand()
{
	return kickCommand;
}


// class KickToPointVelocity

KickToPointVelocity::KickToPointVelocity(const Point point, float velocity,
		const Ball &ball, const Body &body, float possibleArea): BasicKick(),
		 point(point), velocity(velocity), ball(ball), body(body),
		possibleArea(possibleArea)
{
	Vector meToPoint;

	meToPoint.setByPoints(ball.getPos(), point);

	kickCommand = KickToDirectionVelocity(meToPoint.getDirection(),
			velocity, ball, body, possibleArea).getCommand();
	possibleFlag = KickToDirectionVelocity(meToPoint.getDirection(),
			velocity, ball, body, possibleArea).isPossible();
	trajectoryVector = KickToDirectionVelocity(meToPoint.getDirection(),
			velocity, ball, body, possibleArea).getTrajVec();

}

Command *KickToPointVelocity::getCommand()
{
	return kickCommand;
}


//	class KickToPointArriveVelocity

KickToPointArriveVelocity::KickToPointArriveVelocity(const Point point,
		float velocity, const Ball &ball, const Body &body,
		float possibleArea): BasicKick(),  point(point), velocity(velocity),
		ball(ball), body(body), possibleArea(possibleArea)
{
	Vector meToPoint;
	float firstVelocity;

	meToPoint.setByPoints(ball.getPos(),point);
	firstVelocity = ball.getDecay() * (velocity - (meToPoint.getMagnitude() *
			  (1 - (1 / ball.getDecay()))));
	meToPoint.setAsPolar(firstVelocity, meToPoint.getDirection());

	kickCommand = KickToDirectionVelocity(meToPoint.getDirection(),
			firstVelocity, ball, body, possibleArea).getCommand();
	possibleFlag = KickToDirectionVelocity(meToPoint.getDirection(),
			firstVelocity, ball, body, possibleArea).isPossible();
	trajectoryVector = KickToDirectionVelocity(meToPoint.getDirection(),
			firstVelocity, ball, body, possibleArea).getTrajVec();

}

Command *KickToPointArriveVelocity::getCommand()
{
	return kickCommand;
}


// class KickToPointCycles

KickToPointCycles::KickToPointCycles(const Point point, float cycles,
		const Ball &ball, const Body &body, float possibleArea): BasicKick(),
		 point(point), cycles(cycles), ball(ball), body(body),
		possibleArea(possibleArea)
{
	Vector meToPoint;
	float firstVelocity;

	meToPoint.setByPoints(ball.getPos(),point);
	firstVelocity = (meToPoint.getMagnitude() * (ball.getDecay() - 1)) /
					(pow(ball.getDecay(), cycles) - 1);
	meToPoint.setAsPolar(firstVelocity, meToPoint.getDirection());

	kickCommand = KickToDirectionVelocity(meToPoint.getDirection(),
			firstVelocity, ball, body, possibleArea).getCommand();
	possibleFlag = KickToDirectionVelocity(meToPoint.getDirection(),
			firstVelocity, ball, body, possibleArea).isPossible();
	trajectoryVector = KickToDirectionVelocity(meToPoint.getDirection(),
			firstVelocity, ball, body, possibleArea).getTrajVec();

}

Command *KickToPointCycles::getCommand()
{
	return kickCommand;
}


// class FastKickToDir

FastKickToDir::FastKickToDir(float dir, const Ball &ball, const Body &body): BasicKick(),
		dir(dir), ball(ball), body(body)
{
	float effectiveKickPower = 1 - 0.25 * (ball.getBodyVec().getMagnitude() - body.getSize() -
				ball.getSize()) / body.getKickableMargin() -
				0.25 * fabs(ball.getBodyVec().getDirection() / 180);
	float accel(body.getMaxPower() * ball.getKickPowerRate() * effectiveKickPower);
	Vector v = ball.getVel();

	float h(abs(sin(DEG2RAD * (v.getDirection() - dir)) * v.getMagnitude()));
	if (h > accel)
	{
//		LOG << "WARNING I Can't Fast Kick Ball With Velocity " << v << " to Direction " << dir << endl;
		h = accel;
	}
	float angle(asin(h / accel) * RAD2DEG *
			(normalizeAngle(v.getDirection() - dir) > .0f ? -1.0f : 1.0) + dir);
//	LOG << v << " " << h << " " << angle << " " << accel << " " << dir << endl;

	float kickAngle = Degree::normalizeAngle(angle - body.getBodyDir());
	kickCommand = new KickCommand(body.getMaxPower(), kickAngle);
}

Command *FastKickToDir::getCommand()
{
	return kickCommand;
}


// class FastKickToPoint

FastKickToPoint::FastKickToPoint(const Point point,
		const Ball &ball, const Body &body): BasicKick(),
		point(point), ball(ball), body(body)
{
	Vector dirVec;
	dirVec.setByPoints(body.getPos(), point);
	kickCommand = FastKickToDir(dirVec.getDirection(), ball, body).getCommand();
}

Command *FastKickToPoint::getCommand()
{
	return kickCommand;
}


// class BasicKick

BasicKick::BasicKick(): BasicAction()
{
}

Vector BasicKick::getKickVec(void)
{
	return kickVector;
}

Vector BasicKick::getTrajVec(void)
{
	return trajectoryVector;
}

bool BasicKick::isPossible(void)
{
	return possibleFlag;
}

float BasicKick::needKick(float velocity, const Body &body, const Ball &ball)
{
	float rate;
	rate = (1 - 0.25 * (ball.getBodyVec().getMagnitude() - body.getSize() -
				ball.getSize()) / body.getKickableMargin() -
				0.25 * (fabs(ball.getBodyVec().getDirection()) / 180));
    return velocity / rate;
}

float BasicKick::directionOfMaxPowerKick(float direction, const Body &body,
	const Ball &ball, float maxKickPower)
{
	float maxEffectivePower;
	float rate;
	float alpha;

	rate = (1 - 0.25 * (ball.getBodyVec().getMagnitude() - body.getSize() -
				ball.getSize()) / body.getKickableMargin() -
				0.25 * (fabs(ball.getBodyVec().getDirection()) / 180));

	maxEffectivePower = maxKickPower * ball.getKickPowerRate() * rate;

	alpha = Degree::arcSin(((Degree::sin(direction) * ball.getVel().getX()) -
						    (Degree::cos(direction) * ball.getVel().getY())) /
						   maxEffectivePower) + direction;
	alpha = Degree::normalizeAngle(alpha);
	return alpha;
}

float BasicKick::getMaxDirectionVelocity(float direction, const Player &body, const Ball &ball)
{
	// ball info may be the estimated value..
    Vector ballAbsVec = ball.getPos() - body.getPos();
    Vector ballBodyVec = ballAbsVec;
    ballBodyVec.rotate(-body.getBodyDir());

    float kickRate = ball.getKickPowerRate() * ( 1.0 - 0.25 * std::fabs( ballBodyVec.getDirection() ) / 180.0 - 0.25 * ( ballAbsVec.getMagnitude() - ball.getSize() - body.getSize() ) / body.getKickableMargin() );
    //LOG << " 22 " << body.getMaxPower() << " " <<  kickRate << endl;
    const double max_accel
        = std::min( body.getMaxPower() * kickRate,
                    ball.getAccelMax() );
	//LOG << "maxel" << max_accel << endl;
    // origin point is current ball pos
	HalfLine desired_ray;
	desired_ray.setByPointDir(Point(0.0, 0.0), direction);
    // center point is next ball pos relative to current ball pos
    Circle next_reachable_circle;
	//LOG << "ball vel = " << ball.getVel().asPoint() << "  " << max_accel << endl;
	//LOG << "Direction = " << direction << endl;
	//LOG << "center of circle : " << ball.getVel().asPoint() << endl;
	//LOG << "ray from " << Point(0.0, 0.0) << " to " << direction << endl;
	next_reachable_circle.setByCenterRadius( ball.getVel().asPoint(), max_accel );


    // sol is ball vel (= current ball vel + accel)
    //Vector sol1, sol2; // rel to current ball pos
	vector <Point> m;
    int num = desired_ray.getCircleIntersect(next_reachable_circle, m);
	vector <Vector> p;
	for (int i = 0; i < num; i++)
		p.push_back(Vector(m[i]));
    if ( num == 0 )
    {
        /*dlog.addText( Logger::KICK,
                      __FILE__": get_max_possible_vel: angle=%.1f. No solution",
                      target_angle.degree() );
        return Vector2D( 0.0, 0.0 );*/
		//LOG << "HICHI" << endl;
		return 0.0;
    }

    if ( num == 1 )
    {
        if ( p[0].getMagnitude() > ball.getSpeedMax() )
        {
            // next inertia ball point is within reachable circle.
            if (next_reachable_circle.checkPointInArea(Point(0.0, 0.0)))
            {
                // can adjust angle at least
                p[0].setAsPolar(ball.getSpeedMax(), p[0].getDirection());
				//LOG << "good at one" << endl;
                /*dlog.addText( Logger::KICK,
                              __FILE__": get_max_possible_vel: angle=%.1f."
                              " 1 solution  adjust.",
                              target_angle.degree() );*/
            }
            else
            {
                // failed
                //sol1.assign( 0.0, 0.0 );
				p[0].setAsCartesian(0.0, 0.0);
				//LOG << "failed at one" << endl;
                /*dlog.addText( Logger::KICK,
                              __FILE__": get_max_possible_vel: angle=%.1f."
                              " 1 solution. failed.",
                             target_angle.degree() );*/
            }
        }
/*#ifdef DEBUG
        dlog.addText( Logger::KICK,
                      "one kick -----> angle=%.1f max_vel=(%.2f, %.2f)r=%.2f",
                      target_angle.degree(),
                      sol1.x, sol1.y, sol1.r() );
#endif*/
		//LOG << "not bad at one" << endl;
        return p[0].getMagnitude();

    }


    // num == 2

    double length1 = p[0].getMagnitude();
    double length2 = p[1].getMagnitude();

	//LOG << "1 - " << length1 << endl;
	//LOG << "2 - " << length2 << endl;

    if ( length1 < length2 )
    {
		vector <Vector> q;
		q.push_back(p[1]);
		q.push_back(p[0]);
		p = q;
        //std::swap( sol1, sol2 );
        std::swap( length1, length2 );
        /*dlog.addText( Logger::KICK,
                      __FILE__": get_max_possible_vel: swap" );*/
    }

    if ( length1 > ball.getSpeedMax() )
    {
        if ( length2 > ball.getSpeedMax() )
        {
            p[0].setAsCartesian( 0.0, 0.0 );
			//LOG << "hardobad" << endl;
            /*dlog.addText( Logger::KICK,
                          __FILE__": get_max_possible_vel: angle=%.1f."
                          " 2 solutions. but never reach",
                          target_angle.degree() );*/
        }
        else
        {
            p[0].setAsPolar(ball.getSpeedMax(), p[0].getDirection());
			//LOG << "yeki khoob" << endl;
            /*dlog.addText( Logger::KICK,
                          __FILE__": get_max_possible_vel: angle=%.1f."
                          " 2 solutions. adjust to ballSpeedMax",
                          target_angle.degree() );*/
        }
    }

    /*dlog.addText( Logger::KICK,
                  __FILE__": one kick: 2 solutions: angle=%.1f max_vel=(%.2f, %.2f)r=%.2f",
                  target_angle.degree(),
                  sol1.x, sol1.y, sol1.r() );
*/
	//LOG << "taheKhat" << endl;
    return p[0].getMagnitude();

}

/*MultiStepKickToDir::MultiStepKickToDir(float dir, float velocity, unsigned steps, const Ball &ball, const Body &body): BasicKick()
    , dir(dir), velocity(velocity), steps(steps), ball(ball), body(body)
{
    switch (steps)
    {
	case 1:
	    break;
	case 2:
	    break;
	case 3:
	    break;
	default:
	    for (float d = -180.f;d < 180.f;d += 10.f)
	    {

	    }
	    break;
    };
}
*/
