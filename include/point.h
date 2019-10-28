#ifndef POINT_H
#define POINT_H

#include <armadillo>

using namespace arma;

namespace morph{ namespace animats{

class Point{
private:
	vec color;
public:
	vec x;
	vec v;
	vec vi;
	vec ve;
	float m;
	vec f;

	bool move;
	Point *pre;

	explicit Point( vec x, float m = 1.0, vec color = {0.5, 0.5, 0.5} );
	Point( const Point& p );
	//Point& operator=( const Point& p ); // Copy assignment operator

	vec getColor();

	~Point();
};

}}
#endif