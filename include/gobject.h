#ifndef GEOMETRIC_OBJECT_H
#define GEOMETRIC_OBJECT_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <stdexcept>

#include "shape.h"
#include "geometry.h"
#include "meshes.h"
#include "face.h"

namespace morph{ namespace animats{

enum BodyType{RIGID, SOFT, PLANE};
enum PlanePosition{ Floor, Roof, East, West, Front, Back };

typedef struct{
	vec v;
	GeometricTransform T;
} State;

class MeshProvider;

class GeometricObject{
protected:
	
	vector<Face *> faces;
	vector<Edge> edges;

	State initialState;
	vector<Point *> points;

public:
	BodyType type;
	// Graphic objects
	GLuint VBO;
	GLuint VAO;
	GLuint normalBuffer;

	GeometricObject( MeshProvider* mp );
	GeometricObject( const GeometricObject& go ); // Copy constructor
	GeometricObject& operator=( const GeometricObject& go ); // Copy assignment operator

	void mapTransform( GeometricTransform& gt );
	void reset();
	void addPoint( vec position );
    void addFace( int i1, int i2, int i3 );
    void addFace( int i1, int i2, int i3, vec normal );

    virtual void init( State initialState, double mass = 1.0 );

    vector<Point *>& getPoints();
    vector<Face *>& getFaces();
	vector<Edge>& getEdges();

	friend class MeshProvider;
	friend class SimView;
};

class SoftBody : public GeometricObject{
private:
	DeformableShape<LinearMatchTransform> *shape;
	double alpha;

public:
	SoftBody( MeshProvider* mp, double mass = 1, double alpha = 0.2 );
	DeformableShape<LinearMatchTransform>* getShape();
	void initShape( );
};

class RigidBody : public GeometricObject{

public:
	RigidBody( MeshProvider* mp );
};

class Plane : public RigidBody{
private:
	PlanePosition position;
public:
	Plane( MeshProvider* mp, PlanePosition pos );
	void fixNormal();
};

}}

#endif