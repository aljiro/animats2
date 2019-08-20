#include "gobject.h"

// Geometric Object
GeometricObject::GeometricObject(){

}

void GeometricObject::mapTransform( const GeometricTransform& gt ){
	gt.map( this->points );
}

void GeometricObject::reset(){
	this->mapTransform( this->initalState.T );

	for( Point* p : this->points )
		p->v = this->initialState.v;
}

// SoftBody
Shape* SoftBody::getShape(){

}