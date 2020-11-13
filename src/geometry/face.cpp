#include "face.h"

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
	Edge *e = new Edge( this->points[i], this->points[j] );
	this->points[i]->addEdge( e );
	this->points[j]->addEdge( e );
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

// Pointing outwards
void Face::fixNormalOrientation( vector<Point *>& face_points ){
	if( !this->recompute )
		return;
	else
		this->computeNormal();

	vec x0 = this->points[0]->x; 
	vec vv = centroid( face_points );

	vv -= x0;

	// cout << "Fixing normal: " << printvec(x0) << " and " << printvec(this->normal) << endl;

	if( dot(vv, this->normal) > 0 )
		this->normal = -this->normal;
	
}
