/*
 * StaticFormation.cpp
 *
 *  Created on: 22/08/2012
 *      Author: amin
 */

#include "StaticFormation.h"

#define INVERT_Y 0

StaticFormation::StaticFormation(std::string fileName)
{
	if (fileName != "")
	{
		ifstream fin(fileName.c_str());
		string str = "";
		while (str != "Samples") //reading roles and other useless stuff
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
			if (INVERT_Y)
				node.pos.y *= -1;
			for (int j = 0; j < 11; j++)
			{
				fin >> tof >> node.data[j].x >> node.data[j].y;
				if (INVERT_Y)
					node.data[j].y *= -1;
			}
			nodes.push_back(node);
		}
	}

}

const Point StaticFormation::getPosition(unsigned number, unsigned formationNum) const
{
	if (formationNum >= nodes.size())
		return Point(0, 0);
	return nodes[formationNum].data[number];
}

