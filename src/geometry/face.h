#ifndef FACE_H
#define FACE_H

/*
A face DOES contain a reference to the actual points. 
A face is a topological object.
*/ 

#include <iostream>
#include <vector>
#include <armadillo>
#include <exception>

#include "point.h"
#include "edge.h"
#include "../utilities/util.h"
#include "../utilities/computationalg.h"

using namespace std;
using namespace arma;

namespace morph{ namespace animats{

class FaceException : public exception{

	virtual const char* what() const throw(){
		return "An error happened in the face";
	}
};

class Face{

private:
	mat C;

public:
	vector<Edge> edges; // edges sorted counter-clockwise
	vector<Point*> points; // Just the 3 points of the face
	vector<int> indexes;
	vec normal;
	bool recompute;
	bool iscontact;

	Face(Point *p1, Point *p2, Point *p3);
	Face( const Face& p );
	/*
	 * Construction methods
	 */
	void init( Point *p1, Point *p2, Point *p3 );
	void addEdge( int i, int j );
	void setIndexes( int i1, int i2, int i3 );

	void fixNormalOrientation( vector<Point *>& face_points );
	/*
    Computes the normal vector to the face for a particular 
	*/
	void computeNormal( );
	bool isPenetrated( const Edge&  e );
	bool penetrates( Point *p );

	/*
	 * Interface methods
	 */
	bool isEdgeCrossing( const Edge& e );
	vec getFaceProjection( Edge& e ) ;
	double getPenetrationDepth( Edge& e );

	// Destructor
	~Face();
};

}}

#endif