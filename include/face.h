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
#include "util.h"

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

	void sortVertices( vec x[] );
	void computeAreas( vec a, vec w, double A[] );
	vec computeFaceProjection( vec a, vec w ) ;
	vec tracePointOfContact( Edge& e, vec w, vec u ) ;
	void computeBaseAreas( vec a, vec w, vec x[], double A[] );
	void computeComplementaryArea( vec a, vec w, vec u, vec x[], double dW[] );
	vec getPointOfProjection( vec x[], double A[] );

public:
	vector<Edge> edges; // edges sorted counter-clockwise
	vector<Point*> points; // Just the 3 points of the face
	vector<int> indexes;
	vec normal;
	bool recompute;
	bool iscontact;

	Face(Point *p1, Point *p2, Point *p3);
	Face( const Face& p );
	void init( Point *p1, Point *p2, Point *p3 );

	/*
		Computes the normal vector to the face for a particular 
	*/
	void computeNormal( );
	/* 
		Predicate type A. Requires:
		1. A point p in the face
		2. An extreme v of the edge in the colliding shape

		The test indicates if one of the extremes of the edge is on the positive 
		half space of the plane that contains the face
	*/
	bool A( Point& face_point, vec edge_vector );
	/*
		Predicate type B. Requires:
		1. An edge in the face with extremes vn, vm
		2. An edge of the incident object with extremes vl, vk
		
	*/
	bool B( Point& vn, Point& vm, Point& vl, Point& vk );

	vec project( Edge& e );
	/* 
		Checks the predicates for face piercing.
	*/
	bool isPenetrated( const Edge&  e );
	bool penetrates( Point *p );

	vec faceObjectCentroid( vector<Point *>& face_points, bool pre );
	double getPenetrationDepth( Edge& e );
	double getOrthogonalPenetrationDepth( Edge& e );
	vec getFaceProjection( Edge& e ) ;
	vec getFaceOrthogonalProjection( Edge& e );
	// Pointing outwards
	void fixNormalOrientation( vector<Point *>& face_points );

	vec getCentroid();
	bool isInside( vec pos );
	bool isInsideProjection( Edge& e );
	bool isCoplanar( vec v );
	void addEdge( int i, int j );
	void setIndexes( int i1, int i2, int i3 );

	// Destructor
	~Face();
};

}}

#endif