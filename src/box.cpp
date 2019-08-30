#include "../include/box.h"

using namespace morph::animats;

//Box
Box::Box(){

}

Box::Box( vec min, vec max ){
	
    this->min = min;
    this->max = max;
}

bool Box::collide( Box& b ){
    bool ex = this->max(0) > b.min(0) && this->min(0) < this->max(0);
    bool ey = this->max(1) > b.min(1) && this->min(1) < this->max(1);
    bool ez = this->max(2) > b.min(2) && this->min(2) < this->max(2);

    return ex && ey && ez;
}

void Box::compute( vector<Point*>& points, Box *b ){

	vec pmin = points[0]->x;
	vec pmax = points[1]->x;

	for( Point* pm: points ){
		vec p = pm->x;

		if( p(0) < pmin(0) ) pmin(0) = p(0);
		if( p(1) < pmin(1) ) pmin(1) = p(1);
		if( p(2) < pmin(2) ) pmin(2) = p(2);

		if( p(0) > pmax(0) ) pmax(0) = p(0);
		if( p(1) > pmax(1) ) pmax(1) = p(1);
		if( p(2) > pmax(2) ) pmax(2) = p(2);
	}

	b->min = pmin;
	b->max = pmax;
}