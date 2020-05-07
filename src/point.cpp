#include "../include/point.h"

using namespace morph::animats;
using namespace arma;

Point::Point( vec x, float m, vec color ):x(x), 
								     v(zeros<vec>(3)),
								     xc(zeros<vec>(3)),
								     g(zeros<vec>(3)),
								     m(m), 
								     f(zeros<vec>(3)),
								     color(color),
								     state(Free){
 	this->pre = NULL;

}

Point::Point( const Point& p ){
	this->color = vec(p.color);
	this->x = vec(p.x);
	this->v = vec(p.v);
	this->xc = vec(p.xc);
	this->g = vec(p.g);
	this->m = p.m;
	this->state = p.state;
}

vec Point::getColor(){
	return this->color;
}

Point::~Point(){

}