#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "point.h"
#include <vector>

using namespace arma;
using namespace std;

namespace morph{ namespace animats{

class GeometricTransform{
protected:
	mat M;
public:
	GeometricTransform();
	void map( vector<Point *>&  points );
	/* 
		Computes the composition:
	   	gt0.compose( gt1 ) => gt0 = gt1*gt0
	*/	
	void compose( GeometricTransform& GT );
};

class ScaleTransform : public GeometricTransform{
public:
	ScaleTransform( double sx, double sy, double sz);
};

class RotateTransform : public GeometricTransform{
public: 
	RotateTransform( double alpha, double beta, double gamma );
};

class TranslateTransform : public GeometricTransform{
public: 
	TranslateTransform( double tx, double ty, double tz );
};

}}

#endif