#include "point.h"
#include "face.h"
#include "edge.h"

#include <armadillo>

using namespace arma;
using namespace morph::animats;
using namespace std;

int test1(){
	vec x1 = {0,0,0};
	vec x2 = {0,0,1};
	vec x3 = {1,0,1};

	Point *p1 = new Point( x1 );
	Point *p2 = new Point( x2 );
	Point *p3 = new Point( x3 );
	Face *f = new Face( p1, p2, p3 );

	vec v0 = {0.5, -1, 0.7};
	vec v1 = {0.5, 1, 0.7};

	Point *r0 = new Point( v0 );
	Point *r1 = new Point( v1 );
	Edge e( r0, r1 );

	bool r = f->isPenetrated( e );
	vec q = f->getFaceProjection( e );
	vec q_real = {0.5, 0, 0.7};
	cout << "Test result: " << r << endl;
	q.print();

	if( r  && norm(q - q_real) < 0.001)
		return 0;
	else
		return -1;
}

int test2(){
	vec x1 = {0,0,0};
	vec x2 = {0,1,0};
	vec x3 = {1,0,0};

	Point *p1 = new Point( x1 );
	Point *p2 = new Point( x2 );
	Point *p3 = new Point( x3 );
	Face *f = new Face( p1, p2, p3 );

	vec v0 = {0.5, 0.3, -1};
	vec v1 = {0.5, 0.3, 1};

	Point *r0 = new Point( v0 );
	Point *r1 = new Point( v1 );
	Edge e( r0, r1 );

	bool r = f->isPenetrated( e );
	vec q = f->getFaceProjection( e );
	vec q_real = {0.5, 0.3, 0.0};
	cout << "Test result: " << r << endl;
	q.print();

	if( r  && norm(q - q_real) < 0.001)
		return 0;
	else
		return -1;
}

int main( int argc, char**args ){
	Debug::origin = "FACE";
	Debug::debug_level = LOOP;
	Debug::debugging = true;
	return test2();
}