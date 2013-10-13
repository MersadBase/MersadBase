/*
 * StaticFormation.h
 *
 *  Created on: 22/08/2012
 *      Author: amin
 */

#ifndef STATICFORMATION_H_
#define STATICFORMATION_H_

#include <Types.h>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <DelaunayTriangulation.h>
#include <Logger.h>


class StaticFormation
{
protected:
	vector<FormationNode> nodes;
public:
	StaticFormation(std::string fileName);
	const Point getPosition(unsigned number,unsigned formationNum)const; //number=[0,11)
};

#endif /* STATICFORMATION_H_ */
