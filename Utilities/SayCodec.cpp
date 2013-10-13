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

#include "SayCodec.h"

#include <Logger.h>
#include <WorldModel.h>
#include <iostream>
#include <DecideCommonTypes.h>

#include <cmath>

using namespace std;

std::string chars =
		"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ -().+*/?<>_";

CodeItem::CodeItem(unsigned long long base, unsigned long long number) :
		base(base), number(number) {
}
void CodeItem::operator=(const unsigned long long& a) {
	if (number >= base) {
		LOG<< "ERROR : CodeItem assigned incorrectly" << std::endl;
		assert(0);
	}
	number = a;
}
void CodeItem::encode(CodeItem& code) const {
	code.base *= base;
	code.number *= base;
	code.number += number;
}
void CodeItem::decode(unsigned long long& code) {
	number = code % base;
	code /= base;
}
CodeItem::operator unsigned long long() const {
	return number;
}

CodeItemInt::CodeItemInt(long long int rangeMin, long long int rangeMax) :
		item(rangeMax - rangeMin + 1), rangeMin(rangeMin) {
}
void CodeItemInt::operator=(const long long int& a) {
	item = a - rangeMin;
}
void CodeItemInt::encode(CodeItem& code) const {
	item.encode(code);
}
void CodeItemInt::decode(unsigned long long& code) {
	item.decode(code);
}
CodeItemInt::operator long long int() const {
	return item + rangeMin;
}

CodeItemFloat::CodeItemFloat(float rangeMin, float rangeMax,
		short int precision) :
		item(rangeMin * precision, rangeMax * precision), precision(precision) {
}
void CodeItemFloat::operator=(const float& a) {
	item = a * precision;
}
void CodeItemFloat::encode(CodeItem& code) const {
	item.encode(code);
}
void CodeItemFloat::decode(unsigned long long& code) {
	item.decode(code);
}
CodeItemFloat::operator float() const {
	return ((float) item) / precision;
}

CodeItemPoint::CodeItemPoint(float XRangeMin, float XRangeMax, float YRangeMin,
		float YRangeMax, short int precision) :
		x(XRangeMin, XRangeMax, precision), y(YRangeMin, YRangeMax, precision) {
}
void CodeItemPoint::operator=(const Point& a) {
	x = a.x;
	y = a.y;
}
void CodeItemPoint::encode(CodeItem& code) const {
	x.encode(code);
	y.encode(code);
}
void CodeItemPoint::decode(unsigned long long& code) {
	y.decode(code);
	x.decode(code);
}
CodeItemPoint::operator Point() const {
	return Point(x, y);
}
CodeItemPoint::operator Vector() const {
	return Vector(x, y);
}

SayHearVars::SayHearVars() :
		passCycle(20), passReceiver(1, 11), passBallPos(-52.5, 52.5, -34, 34,
				100), passDir(0, 360, 10), passVel(0, 3, 10) {

}

int SayCodec::getCodeLength(unsigned long long cbase) {
	int base = chars.size();
	for (int j = 0; j < 15; j++) {
		if (cbase < base)
			return j + 1;
		cbase = cbase / base;
	}
	return 99;
}

int SayCodec::pos(char c) {
	return chars.find(c);
}

std::string SayCodec::numToStr(unsigned long long number, std::string &str,
		int gbase) {
	int base = chars.size();
	for (int i = 0; i < gbase; i++) {
		str += chars[number % base];
		number /= base;
	}
	return str;
}

unsigned long long SayCodec::strToNum(std::string str) {
	int base = chars.size();
	unsigned long long res = 0;
	unsigned long long cbase = 1;
	for (unsigned i = 0; i < str.size(); i++) {
		res += pos(str[i]) * cbase;
		cbase *= base;
	}
	return res;
}

unsigned long long SayCodec::getSayBase(int flag) {
	return 20;
}
