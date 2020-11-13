#ifndef UTIL_H
#define UTIL_H

#include <armadillo>
#include <iostream>
#include <vector>
#include <list>
#include "../geometry/point.h"

using namespace std;

namespace morph{
namespace animats{

enum Level{GENERAL, LOOP};

/**
	Debug - General debugging capabilities
 */
class Debug{

private:
	// Class members
	list<string> origins;
	Debug(){}

public:
	// Static members
	static const int STD_OUT = 0;
	static const int FILE = 1;
	
	// Turns on/off the debugging
	static bool debugging;
	// Whether the output should be to a file
	static int type;
	// Stream in case of a file output
	static std::ofstream lout;
	// Level of the debugging can be GENERAL or LOOP
	static Level debug_level;
	// The origin is used to focus on a particular class
	static string origin;
	// Singleton instance
	static Debug* instance;

	// Returns the singleton instance of the logging system
	static Debug& getInstance();

	// Inserts a particular entry in the log
	void log( std::string msg, Level level = GENERAL, string origin="all" );
	// Resgisters an origin (class) into the log system
	// Every class attempting to particular observations should be registered
	// NOT USED SO FAR
	void registerOrigin( string name );

};


// Prints an armadillo vector
inline std::string printvec( arma::vec v ){
	std::stringstream s;
	s << "(" << v(0) << ", "<<v(1)<<", "<<v(2)<<")";
	return s.str();
}

// Prints an armadillo 2D vector
inline std::string printvec2( arma::vec v ){
	std::stringstream s;
	s << "(" << v(0) << ", "<<v(1)<<")";
	return s.str();
}

// Computes the center of mass of a set of points describing a particular
// geometric domain
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

inline vec unitVec( vec u ){
	double nn = norm( u );
	return nn == 0 ? u : u/nn;
}

inline vec computeNormal( vec x[] ){
// Assumes 3 vertices of a triangle in the array
	vec n = cross( x[1] - x[0], x[2] - x[0] );
	return n;
}

inline double anglePointVertex( vec p, vec x[] ){
	vec n = computeNormal( x );
	vec pp0 = p - x[0];
	double cosa = dot(pp0, n)/(norm(pp0)*norm(n));
	return cosa;
}

inline double pointTriangleSignedDistance( vec p, vec x[] ){
	double cosa = anglePointVertex( p, x );
	vec pp0 = p - x[0];
	double d = norm(pp0)*cosa;
	return d;
}

inline vec computeProjectionOnTriangle( vec p, vec x[] ){
	vec n = computeNormal( x );

	if( dot(n, p-x[0]) < 0 )
		n = -n;

	double d = pointTriangleSignedDistance( p, x );
	vec pp0 = -d*n/norm(n);
	vec p0 = p + pp0;
	return p0;
}

inline bool ltvec( vec a, vec b ){
	if( a(0) <= b(0)) 
		return true;
	if (a(1) <= b(1))
		return true;
	if (a(2) <= b(2))
		return true;
	
	return false;
} 

inline void swap( vec x[], int i, int j ){
	vec tmp = x[j];
	x[j] = x[i];
	x[i] = tmp;
}

// Transformations 
vec idTx( vec q );
vec quadTx( vec q );
// Utility
bool allInInterval( vec w, double a, double b );

extern Debug& debugger;

}}

#endif