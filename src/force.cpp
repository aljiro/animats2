#include "../include/force.h"

using namespace morph::animats;

ForceObject::ForceObject( ForceObject *fo = NULL ){
	this->nextObject = fo;
}

ForceObject *ForceObject::next(){
	return this->nextObject;
}

void ForceObject::add( ForceObject *fo ){
	if( this->nextObject == NULL ){
		this->nextObject = fo;
	}else
		this->nextObject->add( fo );
}
	
void ForceObject::applyForce( GeometricObject* go ){
	this->apply( go );

	if( this->nextObject != NULL ) this->next()->applyForce( go );
}

// Concrete force objects
GravityForce::GravityForce( ForceObject *fo ):ForceObject(fo){}

void GravityForce::apply( GeometricObject *go ){
	Debug::log( string("Applying gravity!"), LOOP );
	vector<Point *>& points = go->getPoints();

	for( Point *p : points ){
		p->f[0] = 0.0;
		p->f[1] = -5.0;
		p->f[2] = 0.0;
	}	
}

