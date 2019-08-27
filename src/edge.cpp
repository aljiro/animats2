#include "edge.h"

Edge::Edge(  PointMass* v0, PointMass* v1 ){
	this->v0 = v0;
	this->v1 = v1;
}

PointMass* Edge::getHead(){
	vec u = this->v1->position - this->v0->position;

	if( dot(u, this->v1->velocity) > 0 ) 
		return this->v1;
	else
	