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

#include <SExpression.h>
#include <iostream>
#include <assert.h>
#include <cstdlib>

using namespace std;

// class SExpElement

SExpElement::SExpElement()
{
}

SExpElement::~SExpElement()
{
}

// class SExpAtomic

SExpAtomic::SExpAtomic(const string &value, unsigned &pos)
{
	assert(pos < value.length());
	bool inQ = false;
	string invalid = " \t()";
	unsigned epos = pos;
	for (; pos < value.length() &&
		(inQ || invalid.find(value[epos]) == string::npos); epos++)
		if (value[epos] == '"')
			inQ = !inQ;
	this->value = value.substr(pos, epos - pos);
	pos = epos - 1;
}

SExpAtomic::~SExpAtomic()
{
}

SExpElementType SExpAtomic::type() const
{
	return ST_ATOMIC;
}

string SExpAtomic::toString() const
{
	return value;
}

const string &SExpAtomic::asString() const
{
	return value;
}

char SExpAtomic::asChar() const
{
	assert(value.length() == 1);
	return value[0];
}

bool SExpAtomic::asBool() const
{
	string tmp = "";
	for (string::const_iterator it = value.begin(); it != value.end(); it++)
		tmp += tolower(*it);
	assert(tmp == "true" || tmp == "1" || tmp == "false" || tmp == "0");
	if (tmp == "true" || tmp == "1")
		return true;
	return false;
}

int SExpAtomic::asInt() const
{
	char *end;
	long val = strtol(value.c_str(), &end, 0);
	assert(end != value.c_str());
	return val;
}

float SExpAtomic::asFloat() const
{
	char *end;
	float val = strtof(value.c_str(), &end);
	assert(end != value.c_str());
	return val;
}


// class SExpression

SExpression::SExpression(const string &value, unsigned &pos)
{
	assert(value[pos] == '(');
	unsigned i = pos;
	while (true)
	{
		i++;
		assert(i < value.length());
		if (value[i] == ')')
			break;
		if (value[i] == '(')
		{
			elements.push_back(new SExpression(value, i));
			const SExpression *se = dynamic_cast<const SExpression *>(elements.back());
			if (se && se->size())
			{
				const SExpAtomic *at = dynamic_cast<const SExpAtomic *>((*se)[0]);
				if (at)
					indices[at->asString()] = elements.size() - 1;
			}
		}
		else if (value[i] == ' ' || value[i] == '\t')
			continue;
		else
			elements.push_back(new SExpAtomic(value, i));
	}
	pos = i;
}

SExpression::~SExpression()
{
	for (vector<SExpElement *>::iterator it = elements.begin();
		it != elements.end(); it++)
		delete *it;
}

string SExpression::toString() const
{
	string res = "(";
	for (vector<SExpElement *>::const_iterator it = elements.begin();
		it != elements.end(); it++)
		res += (*it)->toString() + " ";
	res[res.size() - 1] = ')';
	return res;
}

SExpElementType SExpression::type() const
{
	return ST_EXPRESSION;
}

SExpElement *SExpression::operator[](unsigned index) const
{
	assert(index >= 0 && index < elements.size());
	return elements[index];
}

bool SExpression::hasSExpWithHeader(const string &header) const
{
	return indices.find(header) != indices.end();
}

const SExpression &SExpression::operator[](const string &header) const
{
	map<string, int>::const_iterator it = indices.find(header);
	assert(it != indices.end());
	return *(SExpression *)elements[it->second];
}

int SExpression::size() const
{
	return elements.size();
}

