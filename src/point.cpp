#include "../include/point.h"

using namespace morph::animats;
using namespace arma;

Point::Point( vec x, float m, vec color ):x(x), 
								     v(zeros<vec>(3)), 
								     m(m), 
								     f(zeros<vec>(3)),
								     color(color),
								     move(true){

}

vec Point::getColor(){
	return this->color;
}

Point::~Point(){

}