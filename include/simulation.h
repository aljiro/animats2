#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <vector>

#include "solver.h"
#include "gobject.h"
#include "collision.h"
#include "contact.h"
#include "loader.h"
#include "force.h"
#include "view.h"

using namespace std;

namespace morph{ namespace animats{

class Simulation{
private:
	// Objects
	// Holds the active contacts in the simulation
	vector<Contact *> contacts;
	// Softbodies and rigid bodies are 
	// potentially treated differently
	vector<SoftBody *> softBodies;
	vector<RigidBody *> rigidBodies;
	// Force objects.
	// TO-DO: not sure if I will keep this way of doing it
	ForceObject *forceChain;
	// Views to be notified at each step
	vecotr<View *> views;

	// State variables
	bool running;
	unsigned int step;

	Simulation():running(true){}

public:

	static Simulation* load( char* name );

	void computeExternalForces();
	void addView( View *view );
	void notifyViews();
	void reset();

	RigidBody *addRigidBody( int id, string type );
	SoftBody *addSoftBody( int id );
	vector<SoftBody *>& getSoftBodies();
	vector<RigidBody *>& getRigidBodies();

	void run( int maxSteps = -1 );
};

}}

#endif