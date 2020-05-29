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

int point_moving_face_static(){
// Point moving face static
	// Points
	Point *pre = new Point(0.5, -1, 0.25 );
	Point *p = new Point(0.5, 1, 0.25);
	// Edge
	Edge e( pre, p );
	// Face
	Face f( new Point(0,0,0), new Point(1,0,0), new Point(0,0,1) );

	bool r = f.isPenetrated( e );
	bool s = f.isInsideProjection( e );
	vec q = f.getFaceProjection( e );
	cout << "Point moving face static: " << printvec(q) << endl;
	vec rq = {0.5, 0, 0.25};
	if( r && s && norm( q - rq ) < 0.0001 )
		return 0;
	else return -1;

}

int point_moving_face_moving_both(){
// Point moving, face moving. The pre and the post are at
// different sides

	return 0;
}

int point_moving_face_moving_one(){
// Point moving, face moving but point and pre on one side
	return 0;
}

int face_moving_point_static(){
// Face moving point static
	return 0;
}

int point_parallel(){
// Point parallel
	double epsilon = 0.0001;
	// Points
	Point *pre = new Point(0.0, 0.0, -epsilon );
	Point *p = new Point(0.5, 0.25, epsilon );
	// Edge
	Edge e( pre, p );
	// Face
	Face f( new Point(0,0,0), new Point(1,0,0), new Point(0,1,0) );

	bool r = f.isPenetrated( e );
	bool s = f.isInsideProjection( e );
	vec q = f.getFaceProjection( e );
	cout << "Point parallel: " << printvec(q) << endl;
	vec rq = {0.25, 0.125, 0.0};
	if( r && s && norm( q - rq ) < 0.0001 )
		return 0;
	else return -1;
}

int point_cutting_edge(){
// Cutting the edge
	// Points
	Point *pre = new Point(0.5, -1, 0.5 );
	Point *p = new Point(0.5, 1, 0.5);
	// Edge
	Edge e( pre, p );
	// Face
	Face f( new Point(0,0,0), new Point(1,0,0), new Point(0,0,1) );

	bool r = f.isPenetrated( e );
	bool s = f.isInsideProjection( e );
	vec q = f.getFaceProjection( e );
	cout << "Cutting edge: " << printvec(q) << endl;
	vec rq = {0.5, 0, 0.5};
	if( r && s && norm( q - rq ) < 0.0001 )
		return 0;
	else return -1;
	return 0;
}

int point_crossing_vertex(){
// crossing a vertex
	// Points
	Point *pre = new Point(0.0, -1, 0.0 );
	Point *p = new Point(0.0, 1, 0.0);
	// Edge
	Edge e( pre, p );
	// Face
	Face f( new Point(0,0,0), new Point(1,0,0), new Point(0,0,1) );

	bool r = f.isPenetrated( e );
	bool s = f.isInsideProjection( e );
	vec q = f.getFaceProjection( e );
	cout << "Crossing vertex: " << printvec(q) << endl;
	vec rq = {0, 0, 0};
	if( r && s && norm( q - rq ) < 0.0001 )
		return 0;
	else return -1;
	return 0;
}

int test_exhaustive(){
// Tests:
// - isPenetrated
// - isInsideProjection
// - getFaceProjection

	int r[7];
// Normal
	r[0] = point_moving_face_static();
	r[1] = point_moving_face_moving_both();
	r[2] = point_moving_face_moving_one();
	r[3] = face_moving_point_static();

// Pathological
	r[4] = point_parallel();
	r[5] = point_cutting_edge();
	r[6] = point_crossing_vertex();

	for( int i = 0; i < 7; i ++ ) if( r[i] == -1 ) return -1;
	return 0;
}

int main( int argc, char**args ){
	Debug::origin = "FACE";
	Debug::debug_level = LOOP;
	Debug::debugging = true;
	return test_exhaustive();
}