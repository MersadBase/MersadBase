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
 *  	and is modified by: Mohammad Salehe, Sassan Haradji, Pooria Kaviani, Mohammad Zareie, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <Basics.h>
#include <cmath>
#include <cassert>
#include <stdio.h>
#include <string.h>

using namespace std;

namespace Basics
{
	bool isInPlayers(const unsigned num, const std::string& str)
	{
		char charNum = num + '0';
		if (num >= 10) charNum = num - 10 + 'A';
		for (unsigned i = 0; i < str.size(); i++)
			if (str[i] == charNum) return 1;
		return 0;
	}

	unsigned charToUniNum(const char str)
	{
		unsigned uniNum = str - '0';
		if (str >= 'A') uniNum = str + 10 - 'A';
		return uniNum;
	}

	char uniNumToChar(const unsigned num)
	{
		char charNum = num + '0';
		if (num >= 10) charNum = num - 10 + 'A';
		return charNum;
	}

	unsigned firstPlayerInList(const string list, const string blackList)
	{
		for (unsigned i = 0; i < list.size(); i++)
			if (blackList.find(list[i]) == string::npos)
				return charToUniNum(list[i]);
		return 0;
	}

	float reRate(float value, float a, float b, float c, float d, ReRateMode mode)
	{
		if (a == b)
			return c;

		if (mode == RRM_DIRECT)
		{
			if (value < a)
				return c;
			if (value > b)
				return d;
			return (value - a) / (b - a) * (d - c) + c;
		}
		else
		{
			if (value < a)
				return d;
			if (value > b)
				return c;
			return (value - a) / (b - a) * (c - d) + d;
		}
	}

	float sigmoid(float x)
	{
		return (float)(1 / (1 + exp(-x)));
	}

	unsigned changeIntBase(int number, int base, string &str)
	{
		unsigned i = 0;

		for (i = 0; number >= base; i++)
		{
			str += (char)(number % base) ;
			number = (int)((float)number / base);
		}

		str += (char)number ;
		return i + 1;
	}

	unsigned baghimande(int number, unsigned base)
	{
		if (number < (int)base)
		{
			while (number <= (int)base)
				number += base;
			return (unsigned)(number - base);
		}

		while (number >= (int)base)
			number -= base;
		return (unsigned)number;
	}

	int sign(float number)
	{
		if (number > 0)
			return 1;
		else if (number < 0)
			return -1;
		return 0;
	}

	unsigned pairPlayer(unsigned number, unsigned side)
	{
		switch (number)
		{
		case 6:
			if (side == 0)
				return 9;
			else if (side == 1)
				return 6;
			else if (side == 2)
				return 9;
			assert(1);
		case 9:
			if (side == 0)
				return 6;
			else if (side == 1)
				return 6;
			else if (side == 2)
				return 9;
			assert(1);
		case 7:
			if (side == 0)
				return 8;
			else if (side == 1)
				return 7;
			else if (side == 2)
				return 8;
			assert(1);
		case 8:
			if (side == 0)
				return 7;
			else if (side == 1)
				return 7;
			else if (side == 2)
				return 8;
			assert(1);
		case 2:
			if (side == 0)
				return 4;
			else if (side == 1)
				return 2;
			else if (side == 2)
				return 4;
			assert(1);
		case 4:
			if (side == 0)
				return 2;
			else if (side == 1)
				return 2;
			else if (side == 2)
				return 4;
			assert(1);
		case 10:
			if (side == 0)
				return 11;
			else if (side == 1)
				return 10;
			else if (side == 2)
				return 11;
			assert(1);
		case 11:
			if (side == 0)
				return 10;
			else if (side == 1)
				return 10;
			else if (side == 2)
				return 11;
			assert(1);
		}
		return number;
	}

	string pairPlayers(string numbers, unsigned side)
	{
		string pairNumbers("");
		for (unsigned i = 0; i < numbers.size(); i++)
			pairNumbers += uniNumToChar(pairPlayer(charToUniNum(numbers[i])));
		return pairNumbers;
	}
}
