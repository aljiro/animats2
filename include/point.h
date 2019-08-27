#ifndef POINT_H
#define POINT_H

namespace morph{ namespace animats{

class Point{
private:
	vec x;
	vec v;
	float m;
	float f;

	bool move;
	Point *pre;

public:
	explicit Point( vec x, float m = 1.0 ):x(x), v(zeros<vec>(3)), m(m), f(zeros<vec>(3));
	//Point( const Point& p );
	//Point& operator=( const Point& p ); // Copy assignment operator

	~Point();
};

}}
#endif