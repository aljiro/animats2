
#include "force.h"


ForceObject::ForceObject( ForceObject *fo ){
	this->nextObject = fo;
}

ForceObject *ForceObject::next(){
	return this->nextObject;
}
	
void ForceObject::applyForce( GeometricObject* go ){
	this->apply( ps );

	if( this->nextObject != NULL ) this->next()->applyForce( ps );
}

// Concrete force objects
GravityForce::GravityForce( ForceObject *fo ):ForceObject(fo){}

void GravityForce::apply( GeometricObject *go ){
	vector<Point *>& points = go->getPoints();

	for( Point *p : points ){
		p->f[0] += 0.0;
		p->f[1] += -10.0;
		p->f[2] += 0.0;
	}	
}

