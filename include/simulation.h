#ifndef SIMULATION_H
#define SIMULATION_H

#include "solver.h"
#include "geometric_object.h"
#include "contact.h"
#include "loader.h"

using namespace std;

namespace morph{ namespace animats{

class Simulation{
private:
	// Objects
	vector<Contact *> contacts;
	vector<SoftBody *> softBodies;
	vector<RigidBody *> rigidBodies;
	ForceObject *forceChain;
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

	void run();
};

}}

#endif