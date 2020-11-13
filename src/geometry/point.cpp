#include "point.h"

using namespace morph::animats;
using namespace arma;

Point::Point( vec x, float m, vec color ):x(x),
									 x0(x),	
								     v(zeros<vec>(3)),
								     x_c(zeros<vec>(3)),
									 v_c(zeros<vec>(3)),
									 v_half(zeros<vec>(3)),
								     g(zeros<vec>(3)),
								     m(m), 
								     f(zeros<vec>(3)),
								     color(color),
								     state(Free),
									 interactions(0){

}

Point::Point( const Point& p ){
	this->color = vec(p.color);
	this->x = vec(p.x);
	this->x0 = vec(p.x0);
	this->v = vec(p.v);
	this->x_c = vec(p.x_c);
	this->v_c = vec(p.v_c);
	this->v_half = vec(p.v_half);
	this->g = vec(p.g);
	this->m = p.m;
	this->state = p.state;
}

vec Point::getColor(){
	return this->color;
}

void Point::addEdge( Edge *e ){
	bool found = false;

	for( Edge *ef : this->edges )
		if( (e->v0 == ef->v0 || e->v1 == ef->v0) && (e->v0 == ef->v1 || e->v1 == ef->v1) )
			found = true;

	if( !found )
		this->edges.push_back( e );
}

Point::~Point(){

}