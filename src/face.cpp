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

vec Face::faceObjectCentroid( vector<Point *>& face_points ){
	vec vv = zeros<vec>(3);

	for( int i = 0; i <face_points.size(); i++ )
		vv += face_points[i]->x;

	vv /= face_points.size();
	return vv;
}

double Face::getPenetrationDepth( Edge& e ){	

	vec v = e.v1->x - e.v0->x;
	// if( dot(v, this->normal) >= 0){

	// 	this->normal = -this->normal;
	// }

	vec n = this->normal;

	vec f_cm = (this->points[0]->x +
			   this->points[1]->x + 
			   this->points[2]->x)/3.0;
	vec a = e.v0->x - f_cm;
	vec b = e.v1->x - f_cm;

	double tp = -(dot(n, a))/(dot(n, b-a));
	vec g = b*tp + a*(1-tp);
	g += f_cm;
	double d = norm( e.v1->x - g );

	return d ; 
}

vec Face::getFaceProjection( Edge& e ){
	vec v = e.v1->x - e.v0->x;
	// if( dot(v, this->normal) >= 0){

	// 	this->normal = -this->normal;
	// }

	vec n = this->normal;
	vec f_cm = (this->points[0]->x +
			   this->points[1]->x + 
			   this->points[2]->x)/3.0;
	vec fp_cm;

	if( this->points[0]->pre != NULL ){
		fp_cm = (this->points[0]->pre->x +
			   this->points[1]->pre->x + 
			   this->points[2]->pre->x)/3.0;
	}else
		fp_cm = f_cm;

	vec a = e.v0->x - f_cm;
	vec b = e.v1->x - f_cm;
	vec g;

	if( norm(e.v1->x - e.v0->x) < 0.001 ){
		g = b + f_cm;
	}else{
		double tp = -(dot(n, a))/(dot(n, b-a));
		//tp = 0;
		g = b*tp + a*(1-tp);
		g += f_cm;
	}

	

	return g; 
}

vec Face::getFaceProjection2( Edge& e ){
	
	vec u = this->normal/norm(this->normal);
	mat P = eye(3,3) - u*u.t();


	//if( dot( u0, this->normal ) < 0 )
		return  P*( e.v1->x - this->getCentroid()) + this->getCentroid();
	//else
	//	return 0;
}


 // Old penetration depth
double Face::getPenetrationDepth2( Edge& e ){

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
