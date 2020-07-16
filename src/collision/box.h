#ifndef BOX_H
#define BOX_H 

#include <vector>
#include <iostream>
#include <armadillo>

#include "../geometry/point.h"

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
    static void compute( vector<Point*>& points, Box *b ){

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
};

}}

#endif