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
 *  	and is modified by: Sassan Haradji, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>

#include <Line.h>
#include <HalfLine.h>
#include <Circle.h>
#include <Degree.h>
#include <Vector.h>
#include <Logger.h>

using namespace std;
using namespace Degree;

inline double SQUARE( const double & val )
{
    return val * val;
}

inline int QUADRATIC_FOMULA( const double & a, const double & b, const double & c, double & sol1, double & sol2 )
{
    double d = SQUARE( b ) - 4.0 * a * c;
    // ignore small noise
    if ( std::fabs( d ) < 0.001 )
    {
        sol1 = -b / (2.0 * a);
        return 1;
    }
    else if ( d < 0.0 )
    {
        return 0;
    }
    else
    {
        d = std::sqrt( d );
        sol1 = (-b + d) / (2.0 * a);
        sol2 = (-b - d) / (2.0 * a);
        return 2;
    }
}


Line::Line(Point point1, Point point2)
{
	float n;
	if (point2.x == point1.x)
	{
		b = 0;
		a = 1;
		c = 0 - point1.x;
		m = 0xFFFFFF;
	}
	else
	{
		m = (point2.y - point1.y) / (point2.x - point1.x);
		n = point1.y - (point1.x * m);

		a = point1.y - point2.y;
		b = point2.x - point1.x;
		c = point1.y * (point1.x - point2.x) + point1.x * (point2.y - point1.y);
	}
	direction = Degree::arcTan2(point2.y - point1.y, point2.x - point1.x);
}

float Line::getA(void)
{
	return a;
}

float Line::getB(void)
{
	return b;
}

float Line::getC(void)
{
	return c;
}

float Line::getDirection(void)
{
	return direction;
}

Point Line::getLineIntersect(Line line)
{
    double tmp = this->getA() * line.getB() - this->getB() * line.getA();
	LOG << "tmp : " << tmp << endl;
    if ( std::fabs( tmp ) < EPSILON )
    {
    	LOG << "tmp < EPSILON" << endl;
//        return Vector2D::INVALIDATED;
	return 0;
    }

	float a1 = a, b1 = b, c1 = c;
	float a2 = line.getA(), b2 = line.getB(), c2 = line.getC();

	Point tmpp;
	tmpp.x = ((b1 * c2) - (b2 * c1)) / ((a1 * b2) - (a2 * b1));
	tmpp.y = ((a2 * c1) - (a1 * c2)) / ((a1 * b2) - (a2 * b1));

	return tmpp;
}

unsigned Line::getLineIntersect(Line line, Point &intersectPoint)
{
	float a1 = a, b1 = b, c1 = c;
	float a2 = line.getA(), b2 = line.getB(), c2 = line.getC();

	intersectPoint.x = ((b1 * c2) - (b2 * c1)) / ((a1 * b2) - (a2 * b1));
	intersectPoint.y = ((a2 * c1) - (a1 * c2)) / ((a1 * b2) - (a2 * b1));

	return 1;
}

unsigned Line::getHalfLineIntersect(HalfLine halfLine, Point &intersectPoint)
{
	float a1 = a, b1 = b, c1 = c;
	float a2 = halfLine.getA(), b2 = halfLine.getB(), c2 = halfLine.getC();

	intersectPoint.x = ((b1 * c2) - (b2 * c1)) / ((a1 * b2) - (a2 * b1));
	intersectPoint.y = ((a2 * c1) - (a1 * c2)) / ((a1 * b2) - (a2 * b1));

	if (halfLine.checkPoint(intersectPoint))
		return 1;
	return 0;
}

unsigned Line::getCircleIntersect(Circle circle, vector <Point> &points)
{
//	LOG << "a = " << a << " b = " << b << " c = " << c << endl;
    if ( std::fabs( a ) < EPSILON )
    {
        if ( std::fabs( b ) < EPSILON )
            return 0;

        // Line:    By + C = 0  ---> y = -C/B
        // Circle:  (x - cx)^2 + (y - cy)^2 = r^2
        // --->
        double x1 = 0.0, x2 = 0.0;
        int n_sol = QUADRATIC_FOMULA( 1.0, -2.0 * circle.getCenter().x,
                                  ( SQUARE( circle.getCenter().x )
                                  + SQUARE( c / b + circle.getCenter().y )
                                  - SQUARE( circle.getRadius() ) ),
                                x1,
                                x2 );

        if ( n_sol > 0 )
        {
            double y1 = -c / b;

            points.push_back(Point(x1, y1));

            if ( n_sol > 1 )
            {
                points.push_back(Point(x2, y1));
            }
        }
        return n_sol;
    }
    else
    {
        // include (fabs(l.b()) < EPSILON) case
        // use line & circle formula
        //   Ax + By + C = 0
        //   (x - cx)^2 + (y - cy)^2 = r^2
        // make y's quadratic formula using these fomula.
        double m = b / a;
        double d = c / a;
        double a1 = 1.0 + m * m;
        double b1 = 2.0 * ( -circle.getCenter().y + ( d + circle.getCenter().x ) * m );
        double c1 = SQUARE( d + circle.getCenter().x )
            + SQUARE( circle.getCenter().y )
            - SQUARE( circle.getRadius() );
        double y1 = 0.0, y2 = 0.0;
        int n_sol = QUADRATIC_FOMULA( a1, b1, c1,
                                      y1, y2 );

        if ( n_sol > 0 )
        {
            //sol1->assign( line.getX( y1 ), y1 );
			points.push_back(Point(-( b * y1 + c ) / a, y1));
			//LOG << "3- " << Vector(Point(-( b * y1 + c ) / a, y1)) << endl;
        }

        if ( n_sol > 1 )
        {
            //sol2->assign( line.getX( y2 ), y2 );
			points.push_back(Point(-( b * y2 + c ) / a, y2));
			//LOG << "4- " << Vector(Point(-( b * y2 + c ) / a, y2)) << endl;
        }

        return n_sol;
    }
	return 0;
/*	float x0 = circle.getCenter().x, y0 = circle.getCenter().y;
	float r = circle.getRadius();
	float formulaA, formulaB, formulaC;
	float delta;
	Point ansPoint1, ansPoint2;


	formulaA = (pow(a, 2) + pow(b, 2)) / pow(a, 2);
	formulaB = ((2 * a * b * x0) - (2 * b * c) - (2 * pow(a, 2) * y0)) /
		pow(a, 2);
	formulaC = (pow(a * x0, 2) + pow(a * y0, 2) - pow(a * r, 2) + pow(c, 2) +
		(2 * a * c * x0)) / pow(a, 2);
	delta = pow(formulaB, 2) - (4 * formulaA * formulaC);

	if (delta < 0)
		return 0;

	ansPoint1.y = ((0 - formulaB) + sqrt(delta)) / (2 * formulaA);
	ansPoint2.y = ((0 - formulaB) - sqrt(delta)) / (2 * formulaA);

	ansPoint1.x = (0 - ((b * ansPoint1.y) + c)) / a;
	ansPoint2.x = (0 - ((b * ansPoint2.y) + c)) / a;

	if (delta == 0)
	{
		points.push_back(ansPoint1);
		return 1;
	}

	points.push_back(ansPoint1);
	points.push_back(ansPoint2);
	return 2;*/
}

bool Line::checkPoint(Point point)
{
	if (((a * point.x) + (b * point.y) + c) == 0)
		return true;
	return false;
}

void Line::setByABC(float inpA, float inpB, float inpC)
{
	a = inpA;
	b = inpB;
	c = inpC;
	if (b == 0)
		direction = 90;
	else if (a == 0)
		direction = 0;
	else
		direction = arcTan2(-c / b, -c / a);
}

void Line::setBySourceDir(Point point, float dir)
{
	float cs, sn, n;
	if (((dir + 90) / 180) == (int)((dir + 90) / 180))
	{
		b = 0;
		a = 1;
		c = 0 - point.x;
		m = 0xFFFFFF;
	}
	else
	{

		m = Degree::tan(dir);
		n = point.y - (m * point.x);
/*
		a = 0 - m;
		b = 1;
		c = 0 - n;
*/
		sn = Degree::sin(dir);
		cs = Degree::cos(dir);
		a = 0 - sn;
		b = cs;
		c = 0 - (cs * n);

	}
	direction = dir;
}

void Line::setByPoints(Point point1, Point point2)
{
//	float cs, sn
	float n;
	if (point2.x == point1.x)
	{
		b = 0;
		a = 1;
		c = 0 - point1.x;
		m = 0xFFFFFF;
	}
	else
	{
		m = (point2.y - point1.y) / (point2.x - point1.x);
		n = point1.y - (point1.x * m);

		a = point1.y - point2.y;
		b = point2.x - point1.x;
		c = point1.y * (point1.x - point2.x) + point1.x * (point2.y - point1.y);

/*
		a = 0 - sn;
		b = cs;
		c = 0 - (cs * n);
*/
	}
	direction = Degree::arcTan2(point2.y - point1.y, point2.x - point1.x);
}

float Line::getPointDist(Point point)
{
	Line tempLine;
	Point interPoint;
	Vector tempVector;

	tempLine.setBySourceDir(point, Degree::normalizeAngle(direction + 90));
	getLineIntersect(tempLine, interPoint);
	tempVector.setByPoints(interPoint,point);

	return tempVector.getMagnitude();
}

double
Line::getY(double x)
{
	if ( std::fabs( b ) < EPSILON )
	{
//		return std::numeric_limits< double >::max();
		return 100000.0;
	}

	return -( a * x + c ) / b;
}

float getDist(Point pos1, Point pos2)
{
	return sqrt(pow (pos1.x - pos2.x , 2) + pow(pos1.y - pos2.y,2));
}

float getShib(Point pos1, Point pos2)
{
	if (pos1.x != pos2.x)
	    return (pos2.y - pos1.y) / (pos2.x - pos1.x);
	return 0;
}

bool getdistance(Point pos1, Point pos2, Point pos3, Point pos4)
{
	float x,y,z;
	x = getDegree(pos1,pos2,pos4);
	y = getDegree(pos4,pos2,pos3);
	z = getDegree(pos1,pos2,pos3);
	if(x + y < z+1)
	{
	  	x = getDegree(pos2,pos3,pos4);
    	y = getDegree(pos4,pos3,pos1);
		z = getDegree(pos2,pos3,pos1);
		if(x + y < z+1)
		{
			x = getDegree(pos3,pos1,pos4);
	    	y = getDegree(pos4,pos1,pos2);
			z = getDegree(pos3,pos1,pos2);
			if(x + y < z+1)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}
