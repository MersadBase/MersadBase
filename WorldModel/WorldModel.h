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
 *  	and is modified by: Pooria Kaviani, Mohammad Zareie, Mostafa Rokooey,
 *  						Sassan Haradji, Darioush Jalali, Mehrdad Bakhtiari
 *
 *  Released on Sunday 4 July 2010, 12 Tir 1389 by Mersad RoboCup Team.
 *  For more information please read README file.
*/

#ifndef __WORLD_MODEL_H
#define __WORLD_MODEL_H

#include <string>
#include <SExpression.h>
#include <FastIC.h>
#include <WorldModelHear.h>

#ifndef NULL
#	define NULL 0
#endif // NULL

class Form;

class WorldModel: public WorldModelHear
{
protected:
	BallStatus ballStatus;
	FastIC fastIC;
    bool fastICInitialized;

public:
	WorldModel();
	WorldModel(const WorldModel &worldModelCopy);
	virtual ~WorldModel();

	virtual void updateAfterSenseBody();

	void updateFastIC();
	const FastIC &getGlobalFastIC() const;
};

#endif // __WORLD_MODEL_H
