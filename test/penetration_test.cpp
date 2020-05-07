#include "point.h"
#include "face.h"
#include "edge.h"

#include <armadillo>

using namespace arma;
using namespace morph::animats;
using namespace std;

int main( int argc, char**args ){
	vec x1 = {0,0,0};
	vec x2 = {0,0,1};
	vec x3 = {1,0,1};

	Point *p1 = new Point( x1 );
	Point *p2 = new Point( x2 );
	Point *p3 = new Point( x3 );
	Face *f = new Face( p1, p2, p3 );

	vec y1 = {0.5, 1, 0.7};
	vec y2 = {0.5, -1, 0.7};

	Point *q1 = new Point( y1 );
	Point *q2 = new Point( y2 );
	Edge e( q1, q2 );

	bool r = f->isPenetrated( e );

	cout << "Test result: " << r << endl;

	if( r )
		return 0;
	else
		return -1;
}