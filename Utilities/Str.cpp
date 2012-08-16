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
 *  This file is created by: Mohsen Izadi
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Str.h>
#include <cassert>
#include <cctype>
#include <cstdlib>

using namespace std;


// class Str

Str::Str(const string &value): value(value)
{
}

Str::~Str()
{
}

const string &Str::asString() const
{
	return value;
}

char Str::asChar() const
{
	assert(value.length() == 1);
	return value[0];
}

bool Str::asBool() const
{
	string tmp = "";
	for (string::const_iterator it = value.begin(); it != value.end(); it++)
		tmp += tolower(*it);
	assert(tmp == "true" || tmp == "1" || tmp == "false" | tmp == "0");
	if (tmp == "true" || tmp == "1")
		return true;
	return false;
}

int Str::asInt() const
{
	char *end;
	long val = strtol(value.c_str(), &end, 0);
	assert(end != value.c_str());
	return val;
}

float Str::asFloat() const
{
	char *end;
	float val = strtof(value.c_str(), &end);
	assert(end != value.c_str());
	return val;
}
