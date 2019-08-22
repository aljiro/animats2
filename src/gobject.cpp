#include "gobject.h"

// Geometric Object
// Constructors and operators
GeometricObject::GeometricObject( MeshProvider *mp ){
	mp->populate(this);
}

GeometricObject::GeometricObject( const GeometricObject& go ){

	
}

GeometricObject& GeometricObject::operator=( const GeometricObject& go ){

}


void GeometricObject::mapTransform( const GeometricTransform& gt ){
	gt.map( this->points );
}

void GeometricObject::reset(){
	this->mapTransform( this->initalState.T );

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

void GeometricObject::init( State initialState, double mass = 1.0 ){      
	int n = this->points.size();
    double pmass = mass/n;

    for( int i = 0; i < n; i++ ){
        Point *p = this->points[i];
        p->mass = pmass;
    }	

    this->initialState = initialState;
    this->reset();
}

vector<Point *>&  GeometricObject::getPoints(){
	return this->points;
}

// SoftBody
SoftBody::SoftBody( MeshProvider* mp, double alpha = 0.2 ): GeometricObject(mp), alpha(alpha){
	this->shape = new DeformableShape<LinearMatchTransform>( alpha, this->points );
}

Shape* SoftBody::getShape(){
	 return this->shape;
}


// Rigid body
RigidBody::RigidBody( MeshProvider *mp ): GeometricObject(mp){}