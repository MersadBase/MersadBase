/*
 * TriFormation.h
 *
 *  Created on: 17/08/2012
 *      Author: amin
 */

#ifndef TRIFORMATION_H_
#define TRIFORMATION_H_

#include <Types.h>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <DelaunayTriangulation.h>
#include <Logger.h>

using namespace std;

struct TriTriangle
{
	unsigned a,b,c;
};

class TriFormation
{
protected:
	vector<FormationNode> nodes;
	vector<TriTriangle> triangles;
public:
	TriFormation(std::string fileName);
	unsigned findNode(Point p)const;
	unsigned findClosestNode(Point p)const;
	unsigned findTriangle(Point p)const;
	const Point getPosition(unsigned number, Point ballPos)const; //number=[0,11)
	bool isIn(unsigned i,const Point &p)const;
};

#endif /* TRIFORMATION_H_ */
