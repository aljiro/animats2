#ifndef GEOMETRIC_OBJECT_H
#define GEOMETRIC_OBJECT_H

#include "shape.h"
#include "geometry.h"

namespace morph{ namespace animats{

typedef struct{
	vec v;
	GeometricTransform T;
} State;

class GeometricObject{
private:
	vector<Point *> points;
	State initialState;
public:

	GeometricObject( const GeometricObject& go ); // Copy constructor
	GeometricObject& operator=( const GeometricObject& go ); // Copy assignment operator

	void mapTransform( const GeometricTransform& gt );
	void reset();

};

class SoftBody : public GeometricObject{
private:
	Shape *shape;

public:
	SoftBody( MeshProvider* mp, double alpha, double mass )
	Shape* getShape();
};

class RigidBody : public GeometricObject{

};

}}

#endif