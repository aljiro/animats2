#include "gobject.h"

using namespace morph::animats;

// Geometric Object
// Constructors and operators
GeometricObject::GeometricObject( MeshProvider *mp ):dx(zeros<vec>(3)){
	this->id = rand();
	this->visible = true;
	mp->populate(this);
}

GeometricObject::GeometricObject( const GeometricObject& go ){

	
}

GeometricObject& GeometricObject::operator=( const GeometricObject& go ){

}

void GeometricObject::setId( int id ){
    this->id = id;
}

int GeometricObject::getId( ){
	return this->id;
}

void GeometricObject::setVisible( bool v ){
	this->visible = v;
}
    

bool GeometricObject::isVisible(){
	return this->visible;

}


void GeometricObject::mapTransform( GeometricTransform& gt ){
	gt.map( this->points );

	for( Face *f : faces){
		if( f->recompute )
			continue;
		
		f->normal = gt.mapVec(f->normal);

		if( this->type == PLANE )
			((Plane *)this)->fixNormal();
	}

}

void GeometricObject::reset(){
	// WARNING: RELOAD MESH!
	this->mapTransform( this->initialState.T );

	for( Point* p : this->points )
		p->v = this->initialState.v;
}

void GeometricObject::addPoint(vec position){
	Point *p1 = new Point( position );
	this->points.push_back(p1);
}

void GeometricObject::addFace( int i1, int i2, int i3 ){	
	Face *f = new Face( this->points[i1], this->points[i2], this->points[i3] );		
	f->setIndexes( i1, i2, i3 );
	this->faces.push_back( f );
}

void GeometricObject::addFace( int i1, int i2, int i3, vec normal ){
	Face *f = new Face( this->points[i1], this->points[i2], this->points[i3] );		
	f->setIndexes( i1, i2, i3 );
	this->faces.push_back( f );
	f->normal = normal;
}

void GeometricObject::init( State initialState, double mass ){      
	debugger.log(string("Initializing geometric object"), GENERAL, "GEOMETRICOBJECT");
	int n = this->points.size();
    // double pmass = mass/n;
    double pmass = mass;

    for( int i = 0; i < n; i++ ){
        this->points[i]->m = pmass;
    }	

    this->initialState = initialState;
    this->reset();
}

vector<Point *>&  GeometricObject::getPoints(){
	return this->points;
}

vector<Face *>& GeometricObject::getFaces(){
	return this->faces;
}

vector<Edge>& GeometricObject::getEdges(){
	return this->edges;
}

// SoftBody
SoftBody::SoftBody( MeshProvider* mp, double mass, double alpha ): GeometricObject(mp), alpha(alpha){
	this->type = SOFT;
	this->shape = new DeformableShape<LinearMatchTransform>( alpha, this->points );
}

DeformableShape<LinearMatchTransform>* SoftBody::getShape(){
	 return this->shape;
}

void SoftBody::initShape(){
	this->shape->init( this->points );

}


// Rigid body
RigidBody::RigidBody( MeshProvider *mp ): GeometricObject(mp){
	this->type = RIGID;
}

// Plane
Plane::Plane( MeshProvider* mp, PlanePosition pos ):RigidBody(mp), position(pos){
	this->type = PLANE;
}

void Plane::fixNormal(){
	vec dir;

	switch( position ){
		case Floor:
			dir = {0, 1, 0};
			break;
		case Roof:
			dir = {0, -1, 0};
			break;
		default:
			throw std::invalid_argument( "Unsupported plane orientation" );
	}

	for( Face *f : faces ){
		if( dot(f->normal, dir) < 0 )
			f->normal = -f->normal;
	}
} 