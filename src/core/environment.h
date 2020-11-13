#ifndef ENVIROMENT_H
#define ENVIROMENT_H

#include <iostream>
#include <vector>
#include <queue>
#include "../geometry/gobject.h"
#include "force.h"

using namespace std;

namespace morph{ namespace animats{
class Environment{
private:
    // Objects
	// Softbodies and rigid bodies are 
	// potentially treated differently
	
	vector<SoftBody *> softBodies;
	vector<RigidBody *> rigidBodies;
	double r;
// Force objects.
	// TO-DO: not sure if I will keep this way of doing it
	ForceObject *forceChain;
	string workingDir;
public:
	vector<SoftBody *> sbQueue;
    Environment( string workingDir );
	// Applies the external forces to the points
	void computeExternalForces();
	// Initialized the shape parameters of the soft bodies
	void initShapes();
	// Adds force to the force chain
	void addForce( ForceObject *fo );
    RigidBody *addRigidBody( int id, string type );
	SoftBody *addSoftBody( int id );
	vector<SoftBody *>& getSoftBodies();
	vector<RigidBody *>& getRigidBodies();
	SoftBody* generateBodies( int step );
	void computeRadius();
	~Environment();
};
}}

#endif