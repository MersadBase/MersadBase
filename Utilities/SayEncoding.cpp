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
 *  This file is created by: Ahmad Boorghany
 *  	and is modified by: Sassan Haradji
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <cmath>
#include <Logger.h>

#include <Basics.h>
#include <Degree.h>
#include <SayEncoding.h>
#include <cassert>

using namespace std;
using namespace Basics;
using namespace Degree;

string SayEncoding::sayCodeKey = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-().+*/?<>_";

unsigned SayEncoding::getCharNum(char ch)
{
	return sayCodeKey.find(ch);
}

void SayEncoding::encodeObjectPosition(float x, float y, string &code)
{
	int i;
	int codeNum = 0;
	int tempCodeLen = 0;
	string tempCodeStr;

	if (x > 53)
		x = 52.8;
	if (x < -53)
		x = -53;
	if (y > 35)
		y = 34.8;
	if (y < -35)
		y = -35;

	x += 53;
	x *= 5;
	y += 35;
	y *= 5;

	x = floor(x);
	y = floor(y);

	codeNum = (int)y * 530 + (int)x;
	tempCodeLen = changeIntBase(codeNum, sayCodeKey.length(), tempCodeStr);

	for (i = 0; i < 3; i++)
		if (i < tempCodeLen)
			code += sayCodeKey[tempCodeStr[i]];
		else
			code += sayCodeKey[0];
}

void SayEncoding::decodeObjectPosition(const string &code, unsigned codePtr,
		float &x, float &y)
{
	int codeNum;

	codeNum = sayCodeKey.find(code[codePtr + 0]) * 1 +
			sayCodeKey.find(code[codePtr + 1]) * 73 +
			sayCodeKey.find(code[codePtr + 2]) * 73 * 73;

	x = codeNum % 530;
	y = (int)(codeNum / 530.00);

	x /= 5;
	x -= 53;

	y /= 5;
	y -= 35;
}

void SayEncoding::encodeObjectVelocity(float magnitude, float direction,
		string &code)
{
	int i;
	int codeNum = 0;
	int tempCodeLen = 0;
	string tempCodeStr;

	if (magnitude > 2.7)
		magnitude = 2.7;

	magnitude *= 100;
	magnitude = floor(magnitude);
	direction = absoluteAngle(direction) * 3.5;
	direction = floor(direction);

	codeNum = (int)magnitude * 1440 + (int)direction;
	tempCodeLen = changeIntBase(codeNum, sayCodeKey.length(), tempCodeStr);
	for (i = 0; i < 3; i++)
		if (i < tempCodeLen)
			code += sayCodeKey[tempCodeStr[i]];
		else
			code += sayCodeKey[0];
}

void SayEncoding::decodeObjectVelocity(const string &code, unsigned codePtr,
		float &magnitude, float &direction)
{
	int codeNum = sayCodeKey.find(code[codePtr + 0]) * 1 +
			sayCodeKey.find(code[codePtr + 1]) * 73 +
			sayCodeKey.find(code[codePtr + 2]) * 73 * 73;

	direction = codeNum % 1440;
	magnitude = floor(codeNum / 1440.00);

	magnitude /= 100;
	direction /= 3.5;
}

void SayEncoding::encodeWeight(float weight, float start,
		float end, std::string &code)
{
	unsigned codeNum = (unsigned)Basics::reRate(weight, start, end,
			0, 73 * 73 * 73 - 1);

	string tempCodeStr;
	unsigned tempCodeLen = changeIntBase(codeNum,
			sayCodeKey.length(), tempCodeStr);
	for (unsigned i = 0; i < 3; i++)
		if (i < tempCodeLen)
			code += sayCodeKey[tempCodeStr[i]];
		else
			code += sayCodeKey[0];
}

void SayEncoding::decodeWeight(const std::string &code,
		unsigned codePtr, float &weight, float start, float end)
{
	int codeNum = sayCodeKey.find(code[codePtr + 0]) * 1 +
			sayCodeKey.find(code[codePtr + 1]) * 73 +
			sayCodeKey.find(code[codePtr + 2]) * 73 * 73;

	weight = Basics::reRate(codeNum, 0, 73 * 73 * 73 - 1,
			start, end);
}

void SayEncoding::encodeWMSend(vector<Vector> positions, string &code)
{
	if (positions.size() != 5)
	{
		LOG << "ERROR : THIS FUNCTION ONLY IS FOR 5 POSITIONS"
					<< endl;
		assert(0);
	}

	float mag = positions[0].getMagnitude();
	float dir = positions[0].getDirection();

	mag = floor(mag * 4);
	dir = floor(absoluteAngle(dir) - 45);
	long long unsigned posCode = int(mag) * 270 + int(dir);

	for (unsigned i = 1; i < 5; i++)
	{
		float mag = positions[i].getMagnitude();
		float dir = positions[i].getDirection();

		mag = floor(mag * 4);
		dir = floor(dir / 2);

		unsigned codeToAdd = int(mag) * (28 / 2) + int(dir);

		posCode *= (20 * 4) * (28 / 2);
		posCode += codeToAdd;
	}
	string tempCodeStr;
	unsigned tempCodeLen = 0;

	for (tempCodeLen = 0; posCode > 72; tempCodeLen++)
	{
		tempCodeStr += (char)(posCode % 72);
		posCode -= posCode % 72;
		posCode = (long long unsigned)(posCode / 72);
	}
	tempCodeStr += (char)posCode;
	tempCodeLen++;

	for (unsigned i = 0; i < 9; i++)
		if (i < tempCodeLen)
			code += sayCodeKey[tempCodeStr[i]];
		else
			code += sayCodeKey[0];
}

void SayEncoding::decodeWMSend(const string &code, vector<Vector> &positions)
{
	long long unsigned posCode = 0;

	for (unsigned i = 0; i < 9; i++)
		posCode += sayCodeKey.find(code[i]) *
				(long long unsigned)(pow((long double)72, int(i)));

	positions.clear();

	for (unsigned i = 0; i < 5; i++)
		positions.push_back(Vector(Point(0, 0)));

	for (unsigned i = 1; i < 5; i++)
	{
		unsigned tmpCode = (posCode % ((20 * 4) * (28 / 2)));
		float mag = unsigned(tmpCode / (28.00 / 2));
		float dir = tmpCode % (28 / 2);
		mag /= 4;
		dir *= 2;

		Vector tmpPos;
		tmpPos.setAsPolar(mag, dir);

		positions[5 - i] = tmpPos;

		posCode -= tmpCode;
		posCode /= (20 * 4) * (28 / 2);
	}

	unsigned tmpCode = posCode;
	float mag = unsigned(tmpCode / 270.00);
	float dir = tmpCode % (270);
	mag /= 4.00;
	dir += 45;

	Vector tmpPos;
	tmpPos.setAsPolar(mag, dir);
	positions[0] = tmpPos;
}

void SayEncoding::encodeRadarObject(unsigned id, float magnitude,
		float direction, std::string &code)
{
	int codeNum = 0;
	int tempCodeLen = 0;
	string tempCodeStr;

	if (id > 21)
		assert(0);

	if (magnitude > 29.5)
		magnitude = 29.5;
	direction = Degree::absoluteAngle(direction);

//LOG << "\t\tmagnitude: " << magnitude << endl;
//LOG << "\t\tdirection: " << direction << endl;

	magnitude *= 2;
	direction /= 2;

	magnitude = floor(magnitude);
	direction = floor(direction);

	codeNum = (int)id * 180*60 + (int)magnitude * 180 + (int)direction;
	tempCodeLen = changeIntBase(codeNum, sayCodeKey.length(), tempCodeStr);

//LOG << "\t\tid: " << id << endl;
//LOG << "\t\tcodeNum: " << codeNum << endl;

	for (int i = 0; i < 3; i++)
		if (i < tempCodeLen)
			code += sayCodeKey[tempCodeStr[i]];
		else
			code += sayCodeKey[0];
}

void SayEncoding::decodeRadarObject(const std::string &code,
		unsigned codePtr, unsigned &id, float &magnitude, float &direction)
{
	int codeNum;

//LOG << "XYZ: " << code[codePtr + 0] << code[codePtr + 1] << code[codePtr + 2] << endl;

	codeNum = sayCodeKey.find(code[codePtr + 0]) * 1 +
			sayCodeKey.find(code[codePtr + 1]) * 73 +
			sayCodeKey.find(code[codePtr + 2]) * 73 * 73;

	id = (unsigned)((float)codeNum / (180*60));
	int temp = codeNum % (180*60);
	magnitude = (int)(temp / 180);
	direction = temp % 180;

	magnitude /= 2;
	direction *= 2;

//LOG << "\t\tmagnitude: " << magnitude << endl;
//LOG << "\t\tdirection: " << direction << endl;
//LOG << "\t\tid: " << id << endl;
//LOG << "\t\tcodeNum: " << codeNum << endl;

}
