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