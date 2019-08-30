#ifndef COLLISION_H
#define COLLISION_H 

#include <vector>
#include <iostream>
#include <armadillo>

#include "point.h"

using namespace std;
using namespace arma;

namespace morph{ namespace animats{


class Box{
public:
    vec min;
    vec max;

    Box();
    Box( vec min, vec max );
    bool collide( Box& b );

    // static
    static void compute( vector<Point*>& points, Box *b );
};

}}

#endif