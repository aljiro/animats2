#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <vector>
#include <memory>

#include "solver.h"
#include "gobject.h"
#include "collision.h"
#include "contact.h"
#include "loader.h"
#include "force.h"
#include "view.h"

using namespace std;

namespace morph{ namespace animats{

class View;

class Simulation{
private:
	// Objects
	// Softbodies and rigid bodies are 
	// potentially treated differently
	vector<SoftBody *> softBodies;
	vector<RigidBody *> rigidBodies;
	// Force objects.
	// TO-DO: not sure if I will keep this way of doing it
	ForceObject *forceChain;
	// Views to be notified at each step
	vector<View *> views;	

	// State variables
	bool running;
	unsigned int step;
	string workingDir;


	Simulation( string workingDir );

public:

	// Collision manager
	CollisionManager collisionMgr;

	static Simulation* load( char* name );

	void computeExternalForces();
	void addView( View *view );
	void notifyViews( string msg );
	void reset();
	void addForce( ForceObject *fo );
	void initShapes();
	void registerObjectsForCollision();
	void close();

	RigidBody *addRigidBody( int id, string type );
	SoftBody *addSoftBody( int id );
	vector<SoftBody *>& getSoftBodies();
	vector<RigidBody *>& getRigidBodies();

	void run( int maxSteps = -1 );
	~Simulation();
};

}}

#endif