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
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#include <FieldData.h>
#include <assert.h>

// Temp
#include <iostream>
using namespace std;

FieldData::FieldData()
{
	rightSideRate = 1;
	penaltySideRate = 1;
}

int FieldData::getFlagId(const SExpression &exp) const
{
	switch (exp[1]->toString()[0])
	{
		case 'c':
			if (exp.size() <= 2)
				return 0;
			switch (exp[2]->toString()[0])
			{
				case 't': return 3;
				case 'b': return 4;
			}
			assert(0);

		case 't':
			switch (exp[2]->toString()[0])
			{
				case '0': return 19;
				case 'l':
					switch (exp[3]->toString()[0])
					{
						case '1': return 21;
						case '2': return 22;
						case '3': return 23;
						case '4': return 24;
						case '5': return 25;
					}
					assert(0);
				case 'r':
					switch (exp[3]->toString()[0])
					{
						case '1': return 26;
						case '2': return 27;
						case '3': return 28;
						case '4': return 29;
						case '5': return 30;
					}
					assert(0);
			}

		case 'b':
			switch (exp[2]->toString()[0])
			{
				case '0': return 20;
				case 'l':
					switch (exp[3]->toString()[0])
					{
						case '1': return 31;
						case '2': return 32;
						case '3': return 33;
						case '4': return 34;
						case '5': return 35;
					}
					assert(0);
				case 'r':
					switch (exp[3]->toString()[0])
					{
						case '1': return 36;
						case '2': return 37;
						case '3': return 38;
						case '4': return 39;
						case '5': return 40;
					}
					assert(0);
			}

		case 'l':
			switch (exp[2]->toString()[0])
			{
				case '0': return 17;
				case 't':
					if (exp.size() <= 3)
						return 1;
					switch (exp[3]->toString()[0])
					{
					case '1':
						return 41;
					case '2':
						return 42;
					case '3':
						return 43;
					}
					assert(0);
				case 'b':
					if (exp.size() <= 3)
						return 2;
					switch (exp[3]->toString()[0])
					{
					case '1':
						return 44;
					case '2':
						return 45;
					case '3':
						return 46;
					}
					assert(0);
			}

		case 'r':
			switch (exp[2]->toString()[0])
			{
				case '0': return 18;
				case 't':
					if (exp.size() <= 3)
						return 5;
					switch (exp[3]->toString()[0])
					{
					case '1':
						return 47;
					case '2':
						return 48;
					case '3':
						return 49;
					}
					assert(0);

				case 'b':
					if (exp.size() <= 3)
						return 6;
					switch (exp[3]->toString()[0])
					{
					case '1':
						return 50;
					case '2':
						return 51;
					case '3':
						return 52;
					}
					assert(0);
			}

		case 'p':
			switch (exp[2]->toString()[0])
			{
				case 'l':
					switch (exp[3]->toString()[0])
					{
					case 't':
						return 7;
					case 'c':
						return 8;
					case 'b':
						return 9;
					}
					assert(0);
				case 'r':
					switch (exp[3]->toString()[0])
					{
					case 't':
						return 10;
					case 'c':
						return 11;
					case 'b':
						return 12;
					}
					assert(0);
			}

		case 'g':
			switch (exp[2]->toString()[0])
			{
				case 'l':
					switch (exp[3]->toString()[0])
					{
					case 't':
						return 13;
					case 'b':
						return 14;
					}
					assert(0);
				case 'r':
					switch (exp[3]->toString()[0])
					{
					case 't':
						return 15;
					case 'b':
						return 16;
					}
					assert(0);
			}
	}

	assert(0);
	return 0;
}

int FieldData::getGoalId(const SExpression &exp) const
{
	if (exp[1]->toString()[0] == 'l') return 0;
	if (exp[1]->toString()[0] == 'r') return 1;

	assert(0);
	return 0;
}

int FieldData::getLineId(const SExpression &exp) const
{
	if (exp[1]->toString()[0] == 'l') return 0;
	if (exp[1]->toString()[0] == 'r') return 1;
	if (exp[1]->toString()[0] == 't') return 2;
	if (exp[1]->toString()[0] == 'b') return 3;

	assert(0);
	return 0;
}

float FieldData::getPrependOrient(int id) const
{
	int reverseValue;

	reverseValue = rightSideRate * penaltySideRate;

	if (reverseValue == 1)
		switch(id)
		{
		case 0: // Left Line
			return 180;
		case 1: // Right Line
			return 0;
		case 2: // Top Line
			return 90;
		case 3: // Button Line
			return -90;
		}
	else
		switch(id)
		{
		case 1: // Right Line
			return 180;
		case 0: // Left Line
			return 0;
		case 3: // Button Line
			return 90;
		case 2: // Top Line
			return -90;
		}

	assert(0);
	return 0;
}

Point FieldData::getFlagPos(int flagId) const
{
	Point returnPos;
	int reverseValue = rightSideRate * penaltySideRate;

	if (flagId == 0)
	{
		returnPos.x = 0;
		returnPos.y = 0;
	}

	else if (flagId == 1)
	{
		returnPos.x = -52.5;
		returnPos.y = +34;
	}

	else if (flagId == 2)
	{
		returnPos.x = -52.5;
		returnPos.y = -34;
	}

	else if (flagId == 3)
	{
		returnPos.x = 0;
		returnPos.y = +34;
	}

	else if (flagId == 4)
	{
		returnPos.x = 0;
		returnPos.y = -34;
	}

	else if (flagId == 5)
	{
		returnPos.x = 52.5;
		returnPos.y = +34;
	}

	else if (flagId == 6)
	{
		returnPos.x = 52.5;
		returnPos.y = -34;
	}

	else if (flagId == 7)
	{
		returnPos.x = -36.2;
		returnPos.y = +20;
	}

	else if (flagId == 8)
	{
		returnPos.x = -36.2;
		returnPos.y = 0;
	}

	else if (flagId == 9)
	{
		returnPos.x = -36.2;
		returnPos.y = -20;
	}

	else if (flagId == 10)
	{
		returnPos.x = 36.2;
		returnPos.y = +20;
	}

	else if (flagId == 11)
	{
		returnPos.x = 36.2;
		returnPos.y = 0;
	}

	else if (flagId == 12)
	{
		returnPos.x = 36.2;
		returnPos.y = -20;
	}

	else if (flagId == 13)
	{
		returnPos.x = -52.5;
		returnPos.y = 7.32;
	}

	else if (flagId == 14)
	{
		returnPos.x = -52.5;
		returnPos.y = -7.32;
	}

	else if (flagId == 15)
	{
		returnPos.x = 52.5;
		returnPos.y = +7.32;
	}

	else if (flagId == 16)
	{
		returnPos.x = 52.5;
		returnPos.y = -7.32;
	}

	else if (flagId == 17)
	{
		returnPos.x = -57.5;
		returnPos.y = 0;
	}

	else if (flagId == 18)
	{
		returnPos.x = 57.5;
		returnPos.y = 0;
	}

	else if (flagId == 19)
	{
		returnPos.x = 0;
		returnPos.y = +39;
	}

	else if (flagId == 20)
	{
		returnPos.x = 0;
		returnPos.y = -39;
	}

	else if (flagId == 21)
	{
		returnPos.x = -10;
		returnPos.y = 39;
	}

	else if (flagId == 22)
	{
		returnPos.x = -20;
		returnPos.y = 39;
	}

	else if (flagId == 23)
	{
		returnPos.x = -30;
		returnPos.y = 39;
	}

	else if (flagId == 24)
	{
		returnPos.x = -40;
		returnPos.y = 39;
	}

	else if (flagId == 25)
	{
		returnPos.x = -50;
		returnPos.y = 39;
	}

	else if (flagId == 26)
	{
		returnPos.x = 10;
		returnPos.y = 39;
	}

	else if (flagId == 27)
	{
		returnPos.x = 20;
		returnPos.y = 39;
	}

	else if (flagId == 28)
	{
		returnPos.x = 30;
		returnPos.y = 39;
	}

	else if (flagId == 29)
	{
		returnPos.x = 40;
		returnPos.y = 39;
	}

	else if (flagId == 30)
	{
		returnPos.x = 50;
		returnPos.y = 39;
	}

	else if (flagId == 31)
	{
		returnPos.x = -10;
		returnPos.y = -39;
	}

	else if (flagId == 32)
	{
		returnPos.x = -20;
		returnPos.y = -39;
	}

	else if (flagId == 33)
	{
		returnPos.x = -30;
		returnPos.y = -39;
	}

	else if (flagId == 34)
	{
		returnPos.x = -40;
		returnPos.y = -39;
	}

	else if (flagId == 35)
	{
		returnPos.x = -50;
		returnPos.y = -39;
	}

	else if (flagId == 36)
	{
		returnPos.x = 10;
		returnPos.y = -39;
	}

	else if (flagId == 37)
	{
		returnPos.x = 20;
		returnPos.y = -39;
	}

	else if (flagId == 38)
	{
		returnPos.x = 30;
		returnPos.y = -39;
	}

	else if (flagId == 39)
	{
		returnPos.x = 40;
		returnPos.y = -39;
	}

	else if (flagId == 40)
	{
		returnPos.x = 50;
		returnPos.y = -39;
	}

	else if (flagId == 41)
	{
		returnPos.x = -57.5;
		returnPos.y = 10;
	}

	else if (flagId == 42)
	{
		returnPos.x = -57.5;
		returnPos.y = 20;
	}

	else if (flagId == 43)
	{
		returnPos.x = -57.5;
		returnPos.y = 30;
	}

	else if (flagId == 44)
	{
		returnPos.x = -57.5;
		returnPos.y = -10;
	}

	else if (flagId == 45)
	{
		returnPos.x = -57.5;
		returnPos.y = -20;
	}

	else if (flagId == 46)
	{
		returnPos.x = -57.5;
		returnPos.y = -30;
	}

	else if (flagId == 47)
	{
		returnPos.x = 57.5;
		returnPos.y = 10;
	}

	else if (flagId == 48)
	{
		returnPos.x = 57.5;
		returnPos.y = 20;
	}

	else if (flagId == 49)
	{
		returnPos.x = 57.5;
		returnPos.y = 30;
	}

	else if (flagId == 50)
	{
		returnPos.x = 57.5;
		returnPos.y = -10;
	}

	else if (flagId == 51)
	{
		returnPos.x = 57.5;
		returnPos.y = -20;
	}

	else if (flagId == 52)
	{
		returnPos.x = 57.5;
		returnPos.y = -30;
	}

	else
		assert(0);

	returnPos.x *= reverseValue;
	returnPos.y *= reverseValue;

	return returnPos;
}

Point FieldData::getGoalPos(int goalId) const
{
	Point returnPos;
	int reverseValue = rightSideRate * penaltySideRate;

	if(goalId == 0)
	{
		returnPos.x = -52.5;
		returnPos.y = 0;
	}

	else if(goalId == 1)
	{
		returnPos.x = 52.5;
		returnPos.y = 0;
	}

	else
		assert(0);

	returnPos.x *= reverseValue;
	returnPos.y *= reverseValue;

	return returnPos;
}

Point FieldData::getLinePos(int lineId) const
{
	Point returnPos;
	int reverseValue = rightSideRate * penaltySideRate;

	if(lineId == 0)
	{
		returnPos.x = -52.5;
		returnPos.y = 0;
	}

	else if(lineId == 1)
	{
		returnPos.x = 52.5;
		returnPos.y = 0;
	}

	else if(lineId == 2)
	{
		returnPos.x = 0;
		returnPos.y = 34;
	}

	else if(lineId == 3)
	{
		returnPos.x = 0;
		returnPos.y = -34;
	}

	else
		assert(0);

	returnPos.x *= reverseValue;
	returnPos.y *= reverseValue;

	return returnPos;
}

void FieldData::setRightSideRate(int rightSideRateArg)
{
	rightSideRate = rightSideRateArg;
}

void FieldData::setPenaltySideRate(int penaltySideRateArg)
{
	penaltySideRate = penaltySideRateArg;
}

