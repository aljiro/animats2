#include <iostream>
#include <vector>
#include <armadillo>
#include "util.h"

#include "../geometry/point.h"
#include "../geometry/edge.h"
#include "../geometry/face.h"

using namespace arma;

namespace morph{
namespace animats{

void sortVertices( vec x[] );

void computeAreas( Face* f, vec a, vec w, double A[] );

void computeBaseAreas( vec a, vec w, vec x[], double A[] );

void computeComplementaryArea( vec a, vec w, vec u, vec x[], double dW[] );

/* 
    Predicate type A. Requires:
    1. A point p in the face
    2. An extreme v of the edge in the colliding shape

    The test indicates if one of the extremes of the edge is on the positive 
    half space of the plane that contains the face
*/
bool A( Point& face_point, vec edge_vector, vec normal );

/*
    Predicate type B. Requires:
    1. An edge in the face with extremes vn, vm
    2. An edge of the incident object with extremes vl, vk
    
*/
bool B( Point& vn, Point& vm, Point& vl, Point& vk );

	/* 
		Checks the predicates for face piercing.
		(cf.face->isPenetrated( e ) || 
		 cf.face->penetrates(p)) && 
		 cf.face->isInsideProjection(e)
	*/

vec faceObjectCentroid( vector<Point *>& face_points, bool pre );

bool isCoplanar( vec v, vec normal );

vec getPointOfProjection( vec x[], double A[] );

vec computeFaceProjection( vec a, vec w );

bool isInsideProjection( Edge& e );
	
vec getFaceOrthogonalProjection( Edge& e );

double getOrthogonalPenetrationDepth( Edge& e );

/**
 * getCentroid - DEPRECATED 
 */
vec getCentroid();
/**
 * isInside - Tests if a given point is inside the face
 */
bool isInside( Face* f, vec pos );

}}