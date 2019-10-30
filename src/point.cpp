#include "../include/point.h"

using namespace morph::animats;
using namespace arma;

Point::Point( vec x, float m, vec color ):x(x), 
								     v(zeros<vec>(3)),
								     vi(zeros<vec>(3)),
								     ve(zeros<vec>(3)),
								     vc(zeros<vec>(3)),
								     m(m), 
								     f(zeros<vec>(3)),
								     color(color),
								     move(true){
 	this->pre = NULL;

}

Point::Point( const Point& p ){
	this->color = vec(p.color);
	this->x = vec(p.x);
	this->v = vec(p.v);
	this->vi = vec(p.vi);
	this->ve = vec(p.ve);
	this->vc = vec(p.vc);
	this->m = p.m;
}

vec Point::getColor(){
	return this->color;
}

Point::~Point(){

}