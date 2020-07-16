#include "face.h"

using namespace morph::animats;
using namespace arma;

Face::Face(Point *p1, Point *p2, Point *p3){
	this->init( p1, p2, p3 );
}

bool Face::isPenetrated( const Edge&  e ){

	bool apred_out, apred_in, bpred_out, bpred_in, b, O_out, O_in;
	
	if( this->recompute ){
		this->computeNormal();
	}

	apred_out = ( !A( *(this->points[0]), e.v1->x, this->normal ) ) && 
		       		 A( *(this->points[0]), e.v0->x, this->normal );
    //Debug::log("Computing predicate A_in.");
	apred_in = 	     A( *(this->points[0]), e.v1->x, this->normal ) &&
		     	( !A( *(this->points[0]), e.v0->x, this->normal ) );

	bpred_out = true;
	bpred_in = true;
	
	//Debug::log("Computing predicates type B");
	for( int i = 0; i < this->edges.size(); i++ ){
		b = B( *(edges[i].v0), *(edges[i].v1),
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
		isInside(this, p->x) ){		
		return true;
	}
	else
		return false;
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

double Face::getPenetrationDepth( Edge& e ){	
	debugger.log("Computing penetration depth", LOOP, "FACE");

	vec q = this->getFaceProjection( e );
	vec p = e.v1->x;

	double d = norm(p - q);

	return d ; 
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
		}else {//if( norm(u) <= 0.01 ){
			// Do it using u
			debugger.log("Choosing the vertex vector to project", LOOP, "FACE");
			// return computeFaceProjection( a, unitVec(w) );
			return computeProjectionOnTriangle( a, x );
		}// }else{
			// debugger.log("Tracing the point: The face and the point are moving", LOOP, "FACE");
		// 	cin.get();
			// return tracePointOfContact( e, w, u );		
		// }
	}catch(std::exception& ex){
		cout << "Catched exception" << endl;
		throw FaceException();
	}
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
