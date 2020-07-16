#include "computationalg.h"

using namespace arma;
using namespace morph::animats;

void morph::animats::sortVertices( vec x[] ){

	if( !ltvec(x[0], x[1]) )
		swap( x, 0, 1 );

	if( !ltvec(x[1], x[2]) )
		swap( x, 1, 2 );

	if( !ltvec( x[0], x[1]) )
		swap( x, 0, 1 );	
}

void morph::animats::computeAreas( Face* f, vec a, vec w, double A[] ){
	vec x[3] = {f->points[0]->x, 
				f->points[1]->x,
				f->points[2]->x};

	sortVertices( x );

	computeBaseAreas( a, w, x, A );
	debugger.log("Finished computing areas", LOOP, "FACE");
}

void morph::animats::computeBaseAreas( vec a, vec w, vec x[], double A[] ){
	mat M(3,3);
	vec v1, v2;

	// Constructing the areas
	for( int i = 0; i < 3; i++ ){
		debugger.log("Computing A_" + to_string(i), LOOP, "FACE");
		v1 = x[(i + 1)%3] - a;
		v2 = x[(i + 2)%3] - a;
		M(0, 0) = v1[0];
		M(0, 1) = v1[1];
		M(0, 2) = v1[2];
		M(1, 0) = v2[0];
		M(1, 1) = v2[1];
		M(1, 2) = v2[2];
		M(2, 0) = w[0];
		M(2, 1) = w[1];
		M(2, 2) = w[2];
		A[i] = det(M)*0.5;
	}
}

void morph::animats::computeComplementaryArea( vec a, vec w, vec u, vec x[], double dW[] ){
	mat W(3,3), U(3,3);
	vec v1, v2;

	// Constructing the areas
	for( int i = 0; i < 3; i++ ){
		v1 = x[(i + 1)%3] - a;
		v2 = x[(i + 2)%3] - a;
		// W
		W(0, 0) = v1[0];
		W(0, 1) = v1[1];
		W(0, 2) = v1[2];
		W(1, 0) = u[0];
		W(1, 1) = u[1];
		W(1, 2) = u[2];
		W(2, 0) = w[0];
		W(2, 1) = w[1];
		W(2, 2) = w[2];
		// U
		U(0, 0) = u[0];
		U(0, 1) = u[1];
		U(0, 2) = u[2];
		U(1, 0) = v2[0];
		U(1, 1) = v2[1];
		U(1, 2) = v2[2];
		U(2, 0) = w[0];
		U(2, 1) = w[1];
		U(2, 2) = w[2];

		dW[i] = (det(W) + det(U))*0.5;
	}
}

/* 
    Predicate type A. Requires:
    1. A point p in the face
    2. An extreme v of the edge in the colliding shape

    The test indicates if one of the extremes of the edge is on the positive 
    half space of the plane that contains the face
*/
bool morph::animats::A( Point& face_point, vec edge_vector, vec normal ){
	vec u = face_point.x;
	return dot( normal, edge_vector - u ) > 0;
}

/*
    Predicate type B. Requires:
    1. An edge in the face with extremes vn, vm
    2. An edge of the incident object with extremes vl, vk
    
*/
bool morph::animats::B( Point& vn, Point& vm, Point& vl, Point& vk ){
	vec e_face = vm.x - vn.x;
	vec e_object = vk.x - vl.x;
	vec v = vm.x - vk.x;

	vec u = cross( e_face, e_object );
	return dot( u, v ) > 0;
}

	/* 
		Checks the predicates for face piercing.
		(cf.face->isPenetrated( e ) || 
		 cf.face->penetrates(p)) && 
		 cf.face->isInsideProjection(e)
	*/


vec morph::animats::faceObjectCentroid( vector<Point *>& face_points, bool pre ){
	vec vv = zeros<vec>(3);
	vec x;

	for( int i = 0; i <face_points.size(); i++ ){
		if( pre && face_points[i]->pre != NULL )
			x = face_points[i]->pre->x;
		else
			x = face_points[i]->x;

		vv += x;
	}

	vv /= face_points.size();
	return vv;
}

bool morph::animats::isCoplanar( vec v, vec normal ){
	double epsilon = 0.01;
	double r = abs(dot(v, normal));

	return r < epsilon;
}

vec morph::animats::getPointOfProjection( vec x[], double A[] ){
	vec q = zeros<vec>(3);
	double sA = A[0] + A[1] + A[2];

	for( int i = 0; i < 3; i++ )
			q += (A[i]/sA)*x[i];				

	return q;
}

vec morph::animats::computeFaceProjection( vec x[], vec a, vec w ) {
	
	double A[3];
	computeAreas( a, w, A );
	// Applying the condition
	
	if( ((A[0]*A[1]) > 0) && ((A[0]*A[2]) > 0) ){
		// Computing the point
		return getPointOfProjection( x, A );
	}else{

		cout << "Ups, the projection is not inside the face" << endl;

		cout << "q: " << printvec(getPointOfProjection( x, A )) << endl;
		cout << "a: " << printvec(a) << endl;
		cout << "w: " << printvec(w) << endl;
		for(int i = 0; i<3; i++ )cout << "x" << i << ": " << printvec(x[i]) << endl;
		
		return e.v0->x;
	}
	
}

bool morph::animats::isInsideProjection( Edge& e ){
	debugger.log("Checking if the projection is inside", LOOP, "FACE");
	try{
		vec q = getFaceProjection( e );
		return true;
	}catch( std::exception& e ){
		return false;
	}

}
	
vec morph::animats::getFaceOrthogonalProjection( Edge& e ){
	if( this->recompute ){
		this->computeNormal();
	}
	
	vec u = this->normal/norm(this->normal);
	mat P = eye(3,3) - u*u.t();

	//if( dot( u0, this->normal ) < 0 )
		return  P*( e.v1->x - this->getCentroid()) + this->getCentroid();
	//else
	//	return 0;
}

double morph::animats::getOrthogonalPenetrationDepth( Edge& e ){

	if( this->recompute ){
		this->computeNormal();
	}

	vec u = this->normal/norm(this->normal);
	mat P = u*u.t();

	//if( dot( u0, this->normal ) < 0 )
	vec px = P*( e.v1->x - this->getCentroid() );
	return norm(px);
	//else
	//	return 0;

}

/**
 * getCentroid - DEPRECATED 
 */
vec morph::animats::getCentroid(){
	vec p1 = this->edges[0].v0->x;
	vec p2 = this->edges[1].v0->x;
	vec p3 = this->edges[2].v0->x;

	return (p1 + p2 + p3)/3;
}
/**
 * isInside - Tests if a given point is inside the face
 */
bool morph::animats::isInside( Face* f, vec pos ){
	// Computing the barycentric coordinates
	vec p1 = f->edges[0].v0->x;
	vec p2 = f->edges[1].v0->x;
	vec p3 = f->edges[2].v0->x;

	vec u0 = p2 - p1;
	vec u1 = p3 - p1;
	vec u2 = pos - p1;

	double d11 = dot( u1, u1 );
	double d00 = dot( u0, u0 );
	double d02 = dot( u0, u2 );
	double d01 = dot( u0, u1 );
	double d12 = dot( u1, u2 );


	double dT = d00*d11 - d01*d01;
	double lambda2 = (d11*d02 - d01*d12)/dT;
	double lambda3 = (d00*d12 - d01*d02)/dT;
	double lambda1 = 1 - lambda2 - lambda3;

	return lambda1 >= 0 && lambda2 >= 0 && lambda3 >= 0;
}