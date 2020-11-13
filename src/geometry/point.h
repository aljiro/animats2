#ifndef POINT_H
#define POINT_H

#include <armadillo>
#include <vector>
#include <iostream>
#include "edge.h"

using namespace arma;
using namespace std;

namespace morph{ namespace animats{

enum PointState {Free, Colliding, Proximal};

class Edge;
/**
 * Point - Stateful representation of the points.
 */
class Point{
private:
	vec color;
public:
	vec x;
	vec x0;
	vec v;
	vec x_c; // Candidate pos
	vec v_c; // Candidate vel
	vec v_half;
	vec g; // Goal point
	float m;
	vec f;
	vector<Edge *> edges;
	PointState state;
	int interactions;

	explicit Point( vec x, float m = 1.0, vec color = {0.5, 0.5, 0.5} );
	Point( const Point& p );
	//Point& operator=( const Point& p ); // Copy assignment operator

	void addEdge( Edge *e );
	vec getColor();
	~Point();
};

}}
#endif