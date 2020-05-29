#ifndef POINT_H
#define POINT_H

#include <armadillo>

using namespace arma;

namespace morph{ namespace animats{

enum PointState {Free, Invalid, Quiescent};

/**
 * Point - Stateful representation of the points.
 */
class Point{
private:
	vec color;
public:
	vec x;
	vec v;
	vec xc; // Reaction
	vec g; // Goal point
	float m;
	vec f;

	PointState state;
	Point *pre;

	explicit Point( double x1, double x2, double x3 );
	explicit Point( vec x, float m = 1.0, vec color = {0.5, 0.5, 0.5} );
	Point( const Point& p );
	//Point& operator=( const Point& p ); // Copy assignment operator

	vec getColor();
	~Point();
};

}}
#endif