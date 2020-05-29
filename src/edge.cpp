#include "../include/edge.h"

using namespace morph::animats;
using namespace arma;

Edge::Edge( Point* v0, Point* v1 ){
	this->v0 = v0;
	this->v1 = v1;
}

double Edge::length(){
	return norm(this->v1->x - this->v0->x);
}

Point* Edge::getHead(){
	vec u = this->v1->x - this->v0->x;

	if( dot(u, this->v1->v) > 0 ) 
		return this->v1;
	else	
		return this->v0;
}