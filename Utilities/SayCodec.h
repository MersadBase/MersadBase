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
 *  This file is created by: Mohammadreza Montazeri
 *
 *  Released on Monday 13 September 2010, 22 Shahrivar 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
 */

#ifndef SAYCODEC_H_
#define SAYCODEC_H_

#include <vector>
#include <Types.h>
#include <Logger.h>
#include <string>
#include <Vector.h>
#include <cassert>

class CodeItem {
public:
	unsigned long long base;
	unsigned long long number;
	//add lastUsed to see what was the last I said or heard this thing
	CodeItem(unsigned long long base = 1, unsigned long long number = 0);
	void operator=(const unsigned long long& a);
	void encode(CodeItem& code) const;
	void decode(unsigned long long& code);
	operator unsigned long long() const;
};
class CodeItemInt {
	CodeItem item;
	const long long int rangeMin;
public:
	CodeItemInt(long long int rangeMin, long long int rangeMax);
	void operator=(const long long int& a);
	void encode(CodeItem& code) const;
	void decode(unsigned long long& code);
	operator long long int() const;
};

class CodeItemFloat {
	CodeItemInt item;
	const short int precision;
public:
	CodeItemFloat(float rangeMin, float rangeMax, short int precision);
	void operator=(const float& a);
	void encode(CodeItem& code) const;
	void decode(unsigned long long& code);
	operator float() const;
};
class CodeItemPoint {
	CodeItemFloat x, y;
public:
	CodeItemPoint(float XRangeMin, float XRangeMax, float YRangeMin,
			float YRangeMax, short int precision);
	void operator=(const Point& a);
	void encode(CodeItem& code) const;
	void decode(unsigned long long& code);
	operator Point() const;
	operator Vector() const;
};

class SayHearVars {
public:
	CodeItem passCycle;
	CodeItemInt passReceiver;
	CodeItemPoint passBallPos;
	CodeItemFloat passDir;
	CodeItemFloat passVel;

	SayHearVars();
};

class SayCodec {
public:
	static int getCodeLength(unsigned long long cbase);
	static std::string numToStr(unsigned long long number, std::string &str,
			int gbase = -1);
	static unsigned long long strToNum(std::string str);
	static unsigned long long getSayBase(int flag);
private:
	static int pos(char c);
};

#endif /* SAYCODEC_H_ */
