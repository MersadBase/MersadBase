/*
 * TriFormation.cpp
 *
 *  Created on: 17/08/2012
 *      Author: amin
 */

#include "TriFormation.h"

using namespace std;

#define INVERT_Y 0

TriFormation::TriFormation(std::string fileName)
{
	if (fileName != "")
	{
		Delaunay_tree DT;
		ifstream fin(fileName.c_str());
		string str = "";
		while (str != "Samples")//reading roles and other useless stuff
			fin >> str;
		int n;
		int tof; //for reading rubbish
		fin >> tof >> n;
		for (int i = 0; i < n; i++)
		{
			fin >> str >> str >> str; //reading ------- X -------
			fin >> str; //reading "Ball"
			FormationNode node;
			fin >> node.pos.x >> node.pos.y;
			if(INVERT_Y)
				node.pos.y *= -1;
			for (int j = 0; j < 11; j++)
			{
				fin >> tof >> node.data[j].x >> node.data[j].y;
				if(INVERT_Y)
					node.data[j].y *= -1;
			}
			nodes.push_back(node);
			DT.addNode(new point(node.pos.x, node.pos.y));
		}
		DT.output();
		vector<Triangle> &out = DT.out;
		for (unsigned i = 0; i < out.size(); i++)
		{
			TriTriangle temp;
			temp.a = findNode(out[i].getA());
			temp.b = findNode(out[i].getB());
			temp.c = findNode(out[i].getC());
			triangles.push_back(temp);
		}
	}
}

unsigned TriFormation::findNode(Point a) const
{
	for (unsigned i = 0; i < nodes.size(); i++)
	{
		if (nodes[i].pos == a)
			return i;
	}
	return -1;
}

unsigned TriFormation::findClosestNode(Point a) const
{
	unsigned best = 0;
	for (unsigned i = 1; i < nodes.size(); i++)
	{
		if (nodes[i].pos.getDistance(a) < nodes[best].pos.getDistance(a))
			best = i;
	}
	return best;
}

unsigned TriFormation::findTriangle(Point a) const
{
	for (unsigned i = 0; i < triangles.size(); i++)
	{
		if (isIn(i, a))
			return i;
	}
	return -1;
}

const Point TriFormation::getPosition(unsigned number, Point ballPos) const //number=[0,11)
{
	if(INVERT_Y)
		ballPos.y*=-1;
	unsigned i = findTriangle(ballPos);
	if (i < triangles.size())
	{
		float disA = nodes[triangles[i].a].pos.getDistance(ballPos);
		float disB = nodes[triangles[i].b].pos.getDistance(ballPos);
		float disC = nodes[triangles[i].c].pos.getDistance(ballPos);
		Point posA = nodes[triangles[i].a].data[number];
		Point posB = nodes[triangles[i].b].data[number];
		Point posC = nodes[triangles[i].c].data[number];
		Point avg = ((posA * (disB + disC)) + (posB * (disA + disC))
				+ (posC * (disA + disB))) / (2 * (disA + disB + disC));
		return avg;
	}
	else
	{ //didn't find any triangle which ballPos is in it so i'm gonna find the closest vertex
		return nodes[findClosestNode(ballPos)].data[number];
	}
}

bool TriFormation::isIn(unsigned i, const Point &p) const
{
	Triangle tri(nodes[triangles[i].a].pos, nodes[triangles[i].b].pos,
			nodes[triangles[i].c].pos);
	if (tri.isInTriangle(p) && tri.getArea() > 0)
		return true;
	return false;
}

