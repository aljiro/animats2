#ifndef UTIL_H
#define UTIL_H

#include <armadillo>
#include <iostream>
#include <vector>
#include "point.h"

namespace morph{
namespace animats{
enum Level{GENERAL, LOOP};

class Debug{

private:
	Debug(){}
public:
	static const int STD_OUT = 0;
	static const int FILE = 1;
	

	static bool debugging;
	static int type;
	static std::ofstream lout;
	static Level debug_level;

	static void log( std::string msg, Level level = GENERAL );

};


inline std::string printvec( arma::vec v ){
	std::stringstream s;
	s << "(" << v(0) << ", "<<v(1)<<", "<<v(2)<<")";
	return s.str();
}

inline std::string printvec2( arma::vec v ){
	std::stringstream s;
	s << "(" << v(0) << ", "<<v(1)<<")";
	return s.str();
}

inline vec computeCenterOfMass( const std::vector<Point *>& points ){
	vec cg = zeros<vec>(3);
    double M = 0;
    Point *p;

    for(int i=0; i < points.size(); i++){
        p = points[i];
        //Debug::log(string("Point position: ") + printvec(p.originalPosition));
        cg += p->m*p->x;
        M += p->m;
    }

    cg /= M;
    return cg;
}

vec idTx( vec q );

vec quadTx( vec q );

}}

#endif