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

#ifndef __S_EXPRESSION_H
#define __S_EXPRESSION_H

#include <string>
#include <vector>
#include <map>

enum SExpElementType
{
	ST_ATOMIC,
	ST_EXPRESSION
};

class SExpElement
{
protected:
public:
	SExpElement();
	virtual ~SExpElement();

	virtual SExpElementType type() const = 0;
	virtual std::string toString() const = 0;
};

class SExpAtomic: public SExpElement
{
protected:
	std::string value;

public:
	SExpAtomic(const std::string &value, unsigned &pos);
	virtual ~SExpAtomic();

	virtual SExpElementType type() const;
	virtual std::string toString() const;

	const std::string &asString() const;
	char asChar() const;
	bool asBool() const;
	int asInt() const;
	float asFloat() const;
};

class SExpression: public SExpElement
{
protected:
	std::vector<SExpElement *> elements;
	std::map<std::string, int> indices;

public:
	SExpression(const std::string &value, unsigned &pos);
	virtual ~SExpression();

	virtual SExpElementType type() const;
	virtual std::string toString() const;

	SExpElement *operator[](unsigned index) const;
	bool hasSExpWithHeader(const std::string &header) const;
	const SExpression &operator[](const std::string &header) const;
	int size() const;
};

#endif // __S_EXPRESSION_H
