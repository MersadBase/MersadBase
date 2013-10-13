/*
 * DelaunayTriangulation.h
 *
 *  Created on: 17/08/2012
 *      Author: amin
 */

#ifndef DELAUNAYTRIANGULATION_H_
#define DELAUNAYTRIANGULATION_H_

# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <ctime>
# include <vector>
# include <Logger.h>
# include <Triangle.h>
# include <Types.h>

using namespace std;

//
//  Delaunay-tree
//
//  Copyright (c) 1993  by  INRIA Prisme Project
//  2004 route des Lucioles,
//  BP 93 06902 Sophia Antipolis Cedex
//  All rights reserved.
//  Olivier.Devillers@sophia.inria.fr
//  (33) 93 65 77 63
//  Fax (33) 93 65 76 43
//  http://www.inria.fr:/prisme/personnel/devillers/devillers.html
//
//
//  GEOMETRIC OBJECT :
//
//    The Delaunay tree is a randomized geometric data structure computing the
//    Delaunay triangulation
//    This structure holds insertion and queries. Deletions are not supported
//    in this version.
//
//  Reference:
//
//    Jean-Daniel Boissonnat, Monique Teillaud,
//    On the randomized construction of the Delaunay tree,
//    Theoretetical Computer Science,
//    Volume 112, pages339-354, 1993.
//
//    Olivier Devillers, Stefan Meiser, Monique Teillaud,
//    Fully dynamic Delaunay triangulation in logarithmic expected
//    time per operation,
//    Computational Geometry: Theory and Applications,
//    Volume 2, Number 2, pages 55-80, 1992.
//
//    Olivier Devillers,
//    Robust and efficient implementation of the Delaunay tree,
//   INRIA Research Report 1619, 1992.
//

class Delaunay_tree;
class DT_node;
class DT_list;
class point;
typedef unsigned char ind; // used for flag and ind in array

class Delaunay_tree
{
private:
	int nb;
	DT_node* root;

public:
	Delaunay_tree(); // initialization as empty
	~Delaunay_tree(); // destructor
	Delaunay_tree& addNode(point*); // insertion
	void output(); // the Delaunay triangulation
	vector<Triangle> out;
};

class point
{
private:
	double x;
	double y;
public:
	point(double xx, double yy)
	{
		x = xx;
		y = yy;
	}
	double X(void)
	{
		return x;
	}
	double Y(void)
	{
		return y;
	}
	void lineto(point *p);
	friend point operator +(point, point);
	friend point operator -(point, point);
	friend double operator *(point, point);
	friend double operator ^(point, point);
};

class DT_flag
{
private:
	ind f;
	DT_flag(void)
	{
		f = (ind) 0;
	}
	void infinite(int i)
	{
		f |= (ind) i;
	}
	void last_finite(void)
	{
		f |= 8;
	}
	void kill(void)
	{
		f |= 16;
	}
	ind is_infinite(void)
	{
		return f & 7;
	}
	ind is_last_finite(void)
	{
		return f & 8;
	}
	ind is_dead(void)
	{
		return f & 16;
	}
	ind is_degenerate(void)
	{
		return f & 32;
	}
public:
	friend class DT_node;
	friend class Delaunay_tree;
};

class DT_list
{
private:
	DT_list *next;
	DT_node *key;
	DT_list(DT_list* l, DT_node* k)
	{
		next = l;
		key = k;
	}
	~DT_list(void);
public:
	friend class DT_node;
	friend class Delaunay_tree;
};

class DT_node
{
private:
	DT_flag flag;
	unsigned int nb;
	point* vertices[3];
	DT_node* neighbors[3];
	DT_list* sons;
	DT_node(void); // initialize the root
	DT_node(DT_node*, ind); // initialize nowhere
	DT_node(DT_node*, point*, ind);
	ind conflict(point *);
	DT_node* find_conflict(point* p);
	void output(vector<Triangle> &out);
	ind cw_neighbor_ind(point *p)
	{
		return ((p == vertices[0]) ? 2 : ((p == vertices[1]) ? 0 : 1));
	}
	ind neighbor_ind(DT_node *n)
	{
		return ((neighbors[0] == n) ? 0 : ((neighbors[1] == n) ? 1 : 2));
	}
public:
	friend class Delaunay_tree;
};

#endif /* DELAUNAYTRIANGULATION_H_ */
