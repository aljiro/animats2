#ifndef GEOMETRIC_OBJECT_H
#define GEOMETRIC_OBJECT_H

#include "shape.h"
#include "geometry.h"
#include "meshes.h"

namespace morph{ namespace animats{

typedef struct{
	vec v;
	GeometricTransform T;
} State;

class GeometricObject{
private:
	vector<Point *> points;
	vector<Face *> faces;
	vector<Edge> edges;

	State initialState;
public:
	GeoemtricObject( MeshProvider* mp );
	GeometricObject( const GeometricObject& go ); // Copy constructor
	GeometricObject& operator=( const GeometricObject& go ); // Copy assignment operator

	void mapTransform( const GeometricTransform& gt );
	void reset();
	void addPoint( vec position );
    void addFace( int i1, int i2, int i3 );
    void addFace( int i1, int i2, int i3, vec normal );

    virtual void init( State initialState, double mass = 1.0 );

};

class SoftBody : public GeometricObject{
private:
	Shape<LinearMatchTransform> *shape;
	double alpha;

public:
	SoftBody( MeshProvider* mp, double mass = 1, double alpha = 0.2 );
	Shape* getShape();
};

class RigidBody : public GeometricObject{

public:
	RigidBody( MeshProvider* mp );
};

}}

#endif