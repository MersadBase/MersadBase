/*
 * DelaunayTriangulation.cpp
 *
 *  Created on: 17/08/2012
 *      Author: amin
 */

#include "DelaunayTriangulation.h"

using namespace std;

inline point operator +(point a, point b)
{
	return point(a.x + b.x, a.y + b.y);
}

inline point operator -(point a, point b)

{
	return point(a.x - b.x, a.y - b.y);
}

inline double operator *(point a, point b)

{
	return a.x * b.x + a.y * b.y;
}
//****************************************************************************80

inline double operator ^(point a, point b)

{
	return a.x * b.y - a.y * b.x;
}

DT_list::~DT_list(void)
{
	DT_list *p, *q;

	if (!next)
		return;

	for (p = this, q = this->next; q;)
	{
		p->next = NULL;
		delete p;
		p = q;
		q = q->next;
	}
}

ind DT_node::conflict(point *p)
{
	switch (flag.is_infinite())
	{
	case 4:
		return 0;
	case 3:
		return 1;
	case 2:
		return ((*p - *vertices[0]) * (*vertices[1] + *vertices[2]) >= 0);
	case 1:
		return ((flag.is_last_finite()) ?
				(((*p - *vertices[2]) ^ (*vertices[2] - *vertices[0])) >= 0) :
				(((*p - *vertices[0]) ^ (*vertices[0] - *vertices[1])) >= 0));
	case 0:
		double x, y;
		double x0, y0;
		double x1, y1;
		double x2, y2;
		double z1, z2;
		double alpha, beta, gamma;
		x = p->X();
		y = p->Y();
		x0 = vertices[0]->X();
		y0 = vertices[0]->Y();
		x1 = vertices[1]->X();
		y1 = vertices[1]->Y();
		x2 = vertices[2]->X();
		y2 = vertices[2]->Y();
		x1 -= x0;
		y1 -= y0;
		x2 -= x0;
		y2 -= y0;
		x -= x0;
		y -= y0;
		z1 = (x1 * x1) + (y1 * y1);
		z2 = (x2 * x2) + (y2 * y2);
		alpha = (y1 * z2) - (z1 * y2);
		beta = (x2 * z1) - (x1 * z2);
		gamma = (x1 * y2) - (y1 * x2);
		return ((alpha * x) + (beta * y) + gamma * ((x * x) + (y * y)) <= 0);
	}
	return 0;
}

void point::lineto(point *p)
{
	LOG << "l(" << this->x << "," << this->y << "," << p->x << "," << p->y
			<< ",1);";
}

DT_node::DT_node(void)
{
	vertices[0] = new point(1.0, 0.0);
	vertices[1] = new point(-0.5, 0.8660254);
	vertices[2] = new point(-0.5, -0.8660254);

	flag.infinite(3);
	nb = 0;
	sons = NULL;
}

DT_node::DT_node(DT_node *root, ind i)
{
	vertices[0] = root->vertices[0];
	vertices[1] = root->vertices[1];
	vertices[2] = root->vertices[2];
	flag.infinite(4);
	nb = 0;
	sons = NULL;
	neighbors[i] = root;
	root->neighbors[i] = this;
}
//****************************************************************************80

DT_node::DT_node(DT_node *f, point *c, ind i)
{
	switch (f->flag.is_infinite())
	{
	case 0:
		flag.infinite(0);
		break;
	case 1:
		if (f->flag.is_last_finite())
			flag.infinite((i == 1) ? 0 : 1);
		else
			flag.infinite((i == 2) ? 0 : 1);
		if (flag.is_infinite())
		{
			if (f->flag.is_last_finite())
			{
				if (i == 0)
					flag.last_finite();
			}
			else
			{
				if (i == 1)
					flag.last_finite();
			}
		}
		break;
	case 2:
		flag.infinite((i == 0) ? 2 : 1);
		if (i == 1)
			flag.last_finite();
		break;
	case 3:
		flag.infinite(2);
		break;
	}
	nb = 0;
	sons = NULL;
	f->sons = new DT_list(f->sons, this);
	f->neighbors[i]->sons = new DT_list(f->neighbors[i]->sons, this);
	f->neighbors[i]->neighbors[f->neighbors[i]->neighbor_ind(f)] = this;
	vertices[0] = c;
	neighbors[0] = f->neighbors[i];
	switch (i)
	{
	case 0:
		vertices[1] = f->vertices[1];
		vertices[2] = f->vertices[2];
		break;
	case 1:
		vertices[1] = f->vertices[2];
		vertices[2] = f->vertices[0];
		break;
	case 2:
		vertices[1] = f->vertices[0];
		vertices[2] = f->vertices[1];
		break;
	}
}

DT_node* DT_node::find_conflict(point *p)
{
	DT_list* l;
	DT_node* n;

	if (!conflict(p))
	{
		return NULL;
	}

	if (!flag.is_dead())
	{
		return this;
	}

	for (l = sons; l; l = l->next)
	{
		if (l->key->nb != nb)
		{
			l->key->nb = nb;
			n = l->key->find_conflict(p);
			if (n)
			{
				return n;
			}
		}
	}

	return NULL;
}

void DT_node::output(vector<Triangle> &out)
{
	DT_list* l;

	if (flag.is_dead())
	{
		for (l = sons; l; l = l->next)
			if (l->key->nb != nb)
			{
				l->key->nb = nb;
				l->key->output(out);
			}
		return;
	}

	if (!flag.is_infinite())
	{
		out.push_back(
				Triangle(Point(vertices[0]->X(), vertices[0]->Y()),
						Point(vertices[1]->X(), vertices[1]->Y()),
						Point(vertices[2]->X(), vertices[2]->Y())));
//		vertices[0]->lineto(vertices[1]);
//		vertices[0]->lineto(vertices[2]);
//		vertices[1]->lineto(vertices[2]);
	}
}

void Delaunay_tree::output()
{
	root->nb = ++nb;
	root->output(out);
}

Delaunay_tree::Delaunay_tree(void)
{
	nb = 0;
	root = new DT_node();
	new DT_node(root, 0);
	new DT_node(root, 1);
	new DT_node(root, 2);
	root->neighbors[0]->neighbors[1] = root->neighbors[1];
	root->neighbors[0]->neighbors[2] = root->neighbors[2];
	root->neighbors[1]->neighbors[0] = root->neighbors[0];
	root->neighbors[1]->neighbors[2] = root->neighbors[2];
	root->neighbors[2]->neighbors[0] = root->neighbors[0];
	root->neighbors[2]->neighbors[1] = root->neighbors[1];
}

Delaunay_tree::~Delaunay_tree(void)
{
	nb++;
}

Delaunay_tree &Delaunay_tree::addNode(point *p)

{
	DT_node* n;
	DT_node* created;
	DT_node* last;
	DT_node* first;
	point* q;
	point* r;
	ind i;

	root->nb = ++nb;

	if (!(n = root->find_conflict(p)))
	{
		return *this;
	}
	for (i = 0; (int) i < 3 - (int) n->flag.is_infinite(); i++)
		if ((p->X() == n->vertices[i]->X()) && (p->Y() == n->vertices[i]->Y()))
			return *this;
	n->flag.kill();
	q = n->vertices[0];
	while (n->neighbors[i = n->cw_neighbor_ind(q)]->conflict(p))
	{
		n = n->neighbors[i];
		n->flag.kill();
	}

	first = last = new DT_node(n, p, i);
	r = n->vertices[(((int) i) + 2) % 3];
	while (1)
	{
		i = n->cw_neighbor_ind(r);
		if (n->neighbors[i]->flag.is_dead())
		{
			n = n->neighbors[i];
			continue;
		}
		if (n->neighbors[i]->conflict(p))
		{
			n = n->neighbors[i];
			n->flag.kill();
			continue;
		}
		break;
	}
	while (1)
	{

		created = new DT_node(n, p, i);
		created->neighbors[2] = last;
		last->neighbors[1] = created;
		last = created;
		r = n->vertices[(((int) i) + 2) % 3]; // r turn in n ccw
		if (r == q)
			break;
		// we will turn cw around r, till next triangle is not in conflict
		while (1)
		{
			i = n->cw_neighbor_ind(r);
			if (n->neighbors[i]->flag.is_dead())
			{
				n = n->neighbors[i];
				continue;
			}
			if (n->neighbors[i]->conflict(p))
			{
				n = n->neighbors[i];
				n->flag.kill();
				continue;
			}
			break;
		}
	}
	first->neighbors[2] = last;
	last->neighbors[1] = first;
	return *this;
}
