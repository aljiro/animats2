#ifndef FACE_H
#define FACE_H

/*
A face DOES contain a reference to the actual points. 
A face is a topological object.
*/ 

#include <iostream>
#include <vector>

#include "point.h"

namespace morph{ namespace animats{

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

	Face(PointMass *p1, PointMass *p2, PointMass *p3);

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
	bool A( PointMass face_point, vec edge_vector );
	/*
		Predicate type B. Requires:
		1. An edge in the face with extremes vn, vm
		2. An edge of the incident object with extremes vl, vk
		
	*/
	bool B( PointMass vn, PointMass vm, PointMass vl, PointMass vk );

	vec project( Edge& e );
	/* 
		Checks the predicates for face piercing.
	*/
	PenetrationInfo isPenetrated( const Edge&  e );

	bool penetrates( PointMass *p );
	vec faceObjectCentroid( vector<PointMass *>& face_points );
	double getPenetrationDepth( Edge& e );
	double getPenetrationDepth2( Edge& e );
	vec getFaceProjection( Edge& e );
	vec getFaceProjection2( Edge& e );
	// Pointing outwards
	void fixNormalOrientation( vector<PointMass *>& face_points );

	vec getCentroid();
	bool isInside( vec pos );
	void addEdge( int i, int j );
	void setIndexes( int i1, int i2, int i3 );

	// Destructor
	~Face();
};

}}

#endif