#include "../include/face.h"

using namespace morph::animats;
using namespace arma;

Face::Face(Point *p1, Point *p2, Point *p3){
	this->init( p1, p2, p3 );
}

void Face::init( Point *p1, Point *p2, Point *p3 ){
	this->recompute = true;
	this->iscontact = false;
	
	this->points.push_back( p1 );
	this->points.push_back( p2 );
	this->points.push_back( p3 );
	this->normal = zeros<vec>(3);

	addEdge( 0, 1 );
	addEdge( 1, 2 );
	addEdge( 2, 0 );
}

Face::Face( const Face& p ){
	this->init( p.points[0], p.points[1], p.points[2] );
}

Face::~Face(){
	// Destroy everything
}

void Face::setIndexes( int i1, int i2, int i3 ){
	this->indexes.push_back(i1);
	this->indexes.push_back(i2);
	this->indexes.push_back(i3);
}	

void Face::addEdge( int i, int j ){
	Edge e( this->points[i], this->points[j] );
	this->edges.push_back(e);
}

/*
	Computes the normal vector to the face for a particular 
*/
void Face::computeNormal( ){
	//for( int i = 0; i < this->edges.size(); i++ )
	//	cout << "Edge " << i << ": " << this->edges[i].v0 << "->" << this->edges[i].v1 << endl; 

	Point& e1 = *(this->points[0]);
	Point& e2 = *(this->points[1]);
	Point& e3 = *(this->points[2]);

	vec u1 = e1.x - e2.x;
	vec u2 = e1.x - e3.x;

	this->normal = cross( u1, u2 );	
	double n = norm(this->normal);

	this->normal /= n==0?1:norm(this->normal);	
}

/* 
	Predicate type A. Requires:
	1. A point p in the face
	2. An extreme v of the edge in the colliding shape

	The test indicates if one of the extremes of the edge is on the positive 
	half space of the plane that contains the face
*/
bool Face::A( Point& face_point, vec edge_vector ){
	vec u = face_point.x;
	return dot( this->normal, edge_vector - u ) > 0;
}

/*
	Predicate type B. Requires:
	1. An edge in the face with extremes vn, vm
	2. An edge of the incident object with extremes vl, vk
	
*/
bool Face::B( Point& vn, Point& vm, Point& vl, Point& vk ){
	vec e_face = vm.x - vn.x;
	vec e_object = vk.x - vl.x;
	vec v = vm.x - vk.x;

	vec u = cross( e_face, e_object );
	return dot( u, v ) > 0;
}

bool Face::penetrates( Point *p ){

	if( this->recompute ){
		//Debug::log("Computing normal to the face.");
		this->computeNormal();
	}

	if( !(this->points[0]->pre != NULL && 
		this->points[1]->pre != NULL &&
		this->points[2]->pre != NULL && 
		p->pre != NULL) )
		return false;

	vec p1 = this->points[0]->pre->x;
	vec p2 = this->points[1]->pre->x;
	vec p3 = this->points[2]->pre->x;
	vec normal_past = cross( p1 - p2, p2 - p3 );

	if( dot(this->normal, normal_past) <  0 )
		normal_past = -normal_past;

	vec u_past = this->points[0]->pre->x;
	vec u = this->points[0]->x;
	
	double a = dot( this->normal, p->x - u );
	double b = dot( normal_past, p->pre->x - u_past );

	if( ((a > 0  && b < 0) || (a < 0 && b > 0 )) && 
		this->isInside(p->x) ){		
		return true;
	}
	else
		return false;
}

/* 
	Checks the predicates for face piercing.
*/
 bool Face::isPenetrated( const Edge&  e ){

	bool apred_out, apred_in, bpred_out, bpred_in, b, O_out, O_in;
	
	if( this->recompute ){
		this->computeNormal();
	}

	apred_out = ( !this->A( *(this->points[0]), e.v1->x ) ) && 
		       		 this->A( *(this->points[0]), e.v0->x );
    //Debug::log("Computing predicate A_in.");
	apred_in = 	     this->A( *(this->points[0]), e.v1->x ) &&
		     	( !this->A( *(this->points[0]), e.v0->x ) );

	bpred_out = true;
	bpred_in = true;
	
	//Debug::log("Computing predicates type B");
	for( int i = 0; i < this->edges.size(); i++ ){
		b = this->B( *(edges[i].v0), *(edges[i].v1),
					 *(e.v0), *(e.v1) );
		bpred_out = bpred_out && b;
		bpred_in = bpred_in && !b;   
	}

	O_out = apred_out && bpred_out;
	O_in = apred_in && bpred_in;

	bool result = O_out || O_in;
	bool faceEdge = apred_in || apred_out;
	bool edgeEdge = bpred_in || bpred_out;

	return faceEdge; // Returning only the face-edge part of the test
}

vec Face::faceObjectCentroid( vector<Point *>& face_points, bool pre = false ){
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

double Face::getPenetrationDepth( Edge& e ){	
	debugger.log("Computing penetration depth", LOOP, "FACE");

	vec q = this->getFaceProjection( e );
	vec p = e.v1->x;

	double d = norm(p - q);

	return d ; 
}

void Face::sortVertices( vec x[] ){
	debugger.log("Sorting vertices", LOOP, "FACE");

	if( !ltvec(x[0], x[1]) )
		swap( x, 0, 1 );

	if( !ltvec(x[1], x[2]) )
		swap( x, 1, 2 );

	if( !ltvec( x[0], x[1]) )
		swap( x, 0, 1 );	
}

void Face::computeBaseAreas( vec a, vec w, vec x[], double A[] ){
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

void Face::computeComplementaryArea( vec a, vec w, vec u, vec x[], double dW[] ){
	mat W(3,3), U(3,3);
	vec v1, v2;

	// Constructing the areas
	for( int i = 0; i < 3; i++ ){
		debugger.log("Computing A_" + to_string(i), LOOP, "FACE");
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

void Face::computeAreas( vec a, vec w,  double A[] ){
	debugger.log("Computing areas for penetration info", LOOP, "FACE");
	

	vec x[3] = {this->points[0]->x, 
				this->points[1]->x,
				this->points[2]->x};

	sortVertices( x );

	computeBaseAreas( a, w, x, A );
	debugger.log("Finished computing areas", LOOP, "FACE");
}

bool Face::isCoplanar( vec v ){
	double epsilon = 0.01;
	double r = abs(dot(v, this->normal));

	return r < epsilon;
}

vec Face::getPointOfProjection( vec x[], double A[] ){
	vec q = zeros<vec>(3);
	double sA = A[0] + A[1] + A[2];

	for( int i = 0; i < 3; i++ )
			q += (A[i]/sA)*x[i];				

	return q;
}

vec Face::computeFaceProjection( vec a, vec w ) {
	vec x[3] = {this->points[0]->x, 
				this->points[1]->x,
				this->points[2]->x};
	
	double A[3];
	computeAreas( a, w, A );
	// Applying the condition
	debugger.log("Areas: " + to_string(A[0]) + ", " + 
		          to_string(A[1]) + ", " + to_string(A[2]), LOOP, "FACE");

	// if( ((A[0]*A[1]) > 0) && ((A[0]*A[2]) > 0) ){
		// Computing the point
	return getPointOfProjection( x, A );
	// }else{

	// 	cout << "Ups, the projection is not inside the face" << endl;

	// 	cout << "q: " << printvec(getPointOfProjection( x, A )) << endl;
	// 	cout << "a: " << printvec(a) << endl;
	// 	cout << "w: " << printvec(w) << endl;
	// 	for(int i = 0; i<3; i++ )cout << "x" << i << ": " << printvec(x[i]) << endl;
		
	// 	throw FaceException();
	// 	//return e.v0->x;
	// }
	
}

vec Face::tracePointOfContact( Edge& e, vec w, vec u ) {
	// Finds the first point at which the projection falls in the triangle
	// w and u are not unit vectors

	// Validate that they are in the correct side of the hyperplane
	
	int N = 50;
	double tau = 0;
	double h = 1/N;
	double d;
	
	// Point
	vec p0 = e.v0->x;
	// face
	vec x0[3] = {this->points[0]->pre->x, 
				this->points[1]->pre->x,
				this->points[2]->pre->x};

	vec x[3];
	vec p;
	double t_p = -1;

	for( int i = 0; i < N; i++ ){
		// move the points
		for( int j = 0; j < 3; j++ )
			x[j] = x0[j] + u*tau;

		p = p0 + w*tau;
		// compute the distance to the triangle
		d = pointTriangleSignedDistance( p, x );
		// break if close enough
		if( d < 0.05 ){
			t_p = tau;
			break;
		}


		tau += h;
	}

	if( t_p == -1 )
		throw FaceException();

	// Project the point found into the triangle
	vec pp = computeProjectionOnTriangle( p, x );
	// translate to the original position of the triangle
	pp += u*(1 - t_p);

	return pp;
	
}

vec Face::getFaceProjection( Edge& e ){
	debugger.log("Computing face projection point", LOOP, "FACE");
	double A[3];
	double sA;
	vec a = e.v0->x;
	vec q = zeros<vec>(3);

	vec x[3] = {this->points[0]->x, 
				this->points[1]->x,
				this->points[2]->x};

	// Determine whether to use the point vector or the face one
	// Computing the direction vector for the point
	vec w = e.v1->x - e.v0->x;

	// Computing the direction vector for the CoM of the face
	vec cm = faceObjectCentroid( this->points );
	vec cm_pre = faceObjectCentroid( this->points, true );
	vec u = cm - cm_pre;
	cout << "The norm of the movement of the face: " << norm(u) << endl;
	// If both are active, trace the projection matrices until contact
	try{
		if( norm(w) <= 0.01 && norm(u) > 0 ){
			debugger.log("Choosing the face vector to project", LOOP, "FACE");
			// Do it using w		
			// cin.get();
			return computeFaceProjection( a, unitVec(u) );
		}else{ // if( norm(u) <= 0.01 ){
			// Do it using u
			debugger.log("Choosing the vertex vector to project", LOOP, "FACE");
			return computeFaceProjection( a, unitVec(w) );
			// return computeProjectionOnTriangle( a, x );
		}// }else{
			// debugger.log("Tracing the point: The face and the point are moving", LOOP, "FACE");
		// 	cin.get();
		// 	return tracePointOfContact( e, w, u );		
		// }
	}catch(std::exception& ex){
		cout << "Catched exception" << endl;
		throw FaceException();
	}
}

bool Face::isInsideProjection( Edge& e ){
	debugger.log("Checking if the projection is inside", LOOP, "FACE");
	try{
		vec q = getFaceProjection( e );
		return isInside(q);
	}catch( std::exception& e ){
		return false;
	}

}

vec Face::getFaceOrthogonalProjection( Edge& e ){
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


 // Old penetration depth
double Face::getOrthogonalPenetrationDepth( Edge& e ){

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

// Pointing outwards
void Face::fixNormalOrientation( vector<Point *>& face_points ){
	if( !this->recompute )
		return;
	else
		this->computeNormal();

	vec x0 = this->points[0]->x; 
	vec vv = faceObjectCentroid( face_points );

	vv -= x0;

	// cout << "Fixing normal: " << printvec(x0) << " and " << printvec(this->normal) << endl;

	if( dot(vv, this->normal) > 0 )
		this->normal = -this->normal;
	
}

vec Face::getCentroid( ){
	vec p1 = this->edges[0].v0->x;
	vec p2 = this->edges[1].v0->x;
	vec p3 = this->edges[2].v0->x;

	return (p1 + p2 + p3)/3;
}

/*
 * Project the point onto the faces
 */
vec Face::project( Edge& e ){
	double d = getPenetrationDepth( e );
	vec npos = e.v1->x + d*this->normal;
	return npos;
}

bool Face::isInside( vec pos ){
	// Computing the barycentric coordinates
	vec p1 = this->edges[0].v0->x;
	vec p2 = this->edges[1].v0->x;
	vec p3 = this->edges[2].v0->x;

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
