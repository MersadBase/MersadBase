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
 *  	and is modified by: Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Formation.h>
#include <Vector.h>
#include <Logger.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <cmath>

using namespace std;

Formation::Formation(string fileName)
{
	if (fileName != "")
	{
		ifstream fin(fileName.c_str());
		string line;
		while (getline(fin, line))
		{
			int lastPos = line.find_first_of('#');
			if (lastPos != 0 and line != "")
			{
				lastPos = 0;
				Point ballPos = getPoint(line, lastPos);
				for (int i = 1; i <= 11; i++)
					positions[i].push_back(make_pair(ballPos, getPoint(line, lastPos)));
			}
		}
	}
}

Formation::~Formation()
{
}

void Formation::swap(Point &a, Point &b) const
{
	Point temp = a;
	a = b;
	b = temp;
}

void Formation::swap(float &a, float &b) const
{
	float temp = a;
	a = b;
	b = temp;
}


const Point Formation::getPosition(int number, Point ballPos, float powArg) const
{
	int n = 5;
	int i = 0;
	vector <Point> Points;
	vector <float> pointMinDistance;
	for (i = 0; i < n; i++)
		pointMinDistance.push_back(1000);
	for (i = 0; i < n; i++)
		Points.push_back(Point(0,0));
//	LOG << "position size = " << positions[number].size() << endl;
//	LOG << "ballPos : " << ballPos << endl;
	for (vector <pair <Point, Point> >::const_iterator it = positions[number].begin();
		it != positions[number].end(); it++)
	{
//		LOG << "(*it).first : " << (*it).first << endl;
//		LOG << "*** dist : " << (*it).first.getDistance(ballPos) << endl;
		if (pointMinDistance[n-1] > (*it).first.getDistance(ballPos) and (*it).first.getDistance(ballPos) > 0.01)
		{
//			LOG << "add" << endl;
			pointMinDistance[n-1] = (*it).first.getDistance(ballPos);
			Points[n-1] = (*it).second;
			for (i = n - 1; i >= 1; i--)
			{
				if (pointMinDistance[i] < pointMinDistance[i - 1])
				{
					swap(pointMinDistance[i], pointMinDistance[i - 1]);
					swap(Points[i], Points[i - 1]);
				}
			}
		}
	}

	float m = 0;
	for (int i = 0; i < n; i++)
	{
//		LOG << "Points[" << i << "] : " << Points[i] << endl;
//		LOG << "r[" << i << "] : " << pointMinDistance[i] << endl;
		m += (1.f / pow(pointMinDistance[i], powArg));
//		LOG << "\t" << 1.f / pointMinDistance[i] << endl;
	}
//	LOG << "M : " << m << endl;

	Point result(0,0);
	for(int i = 0; i < n; i++)
	{
		result += Points[i] * (1.f / pow(pointMinDistance[i], powArg)/**(Points[i].getDistance(ballPos))*/);
//		LOG << "result + : " << 1.f / pointMinDistance[i] << endl;
	}

	result.x /= m;
	result.y /= m;

//	LOG << "getPosition result : " << result << endl;

	return result;
}

const Point Formation::getExactPosition(int number, Point ballPos) const
{
	Vector point;

	float minDistance = 1000;
	for (vector <pair <Point, Point> >::const_iterator it = positions[number].begin();
		it != positions[number].end(); it++)
	{
		if ((*it).first.getDistance(ballPos) < minDistance)
		{
			point = (*it).second;
			minDistance = (*it).first.getDistance(ballPos);
		}
	}

	return point;
}

float Formation::getNum(string line, int &lastPos)
{
	int pos = line.find_first_of('\t', lastPos + 1);
	string str = line.substr(lastPos, pos);
	lastPos = pos;
	return atof(str.c_str());
}

Point Formation::getPoint(string line, int &lastPos)
{
	Point point(0, 0);
	point.x = getNum(line, lastPos);
	point.y = getNum(line, lastPos);
	return point;
}
