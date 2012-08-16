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

#ifndef __SAY_ENCODING_H
#define __SAY_ENCODING_H

#include <string>
#include <Vector.h>
#include <vector>

#ifndef NULL
#	define NULL 0
#endif // NULL


#define SAY_UPPER_CASE_NUM 35 // BECAREFUL if sayCodeKey is changed.

class SayEncoding
{
public:
	SayEncoding();

	static std::string sayCodeKey;

	static unsigned getCharNum(char ch);

	static void encodeObjectPosition(float x, float y, std::string &code);
	static void decodeObjectPosition(const std::string &code,
			unsigned codePtr, float &x, float &y);
	static void encodeObjectVelocity(float magnitude, float direction,
			std::string &code);
	static void decodeObjectVelocity(const std::string &code,
			unsigned codePtr, float &magnitude, float &direction);
	static void encodeWeight(float weight, float start,
			float end, std::string &code);
	static void decodeWeight(const std::string &code,
			unsigned codePtr, float &weight, float start, float end);
	static void encodeWMSend(std::vector<Vector> positions,
			std::string &code);
	static void decodeWMSend(const std::string &code,
			std::vector<Vector> &positions);

	static void encodeRadarObject(unsigned id, float magnitude,
			float direction, std::string &code);
	static void decodeRadarObject(const std::string &code,
			unsigned codePtr, unsigned &id, float &magnitude, float &direction);
};

#endif // __SAY_ENCODING_H

