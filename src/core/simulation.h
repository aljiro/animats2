#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <vector>
#include <memory>

#include "solver.h"
#include "loader.h"
#include "force.h"
#include "../geometry/gobject.h"
#include "../collision/collision.h"
#include "../collision/contact.h"
#include "../views/view.h"

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
	bool pause;
	// Collision manager
	CollisionManager collisionMgr;

	static Simulation* load( char* name );

	// Applies the external forces to the points
	void computeExternalForces();
	// Adds a new view to the simulation
	void addView( View *view );
	// Notifies the listening views of the opdate
	void notifyViews( string msg );
	void reset();
	// Adds force to the force chain
	void addForce( ForceObject *fo );
	// Initialized the shape parameters of the soft bodies
	void initShapes();
	// Registers all the objects that should be taking into account
	// by the collision manager
	void registerObjectsForCollision();
	void close();
	void togglePause();

	RigidBody *addRigidBody( int id, string type );
	SoftBody *addSoftBody( int id );
	vector<SoftBody *>& getSoftBodies();
	vector<RigidBody *>& getRigidBodies();

	void run( int maxSteps = -1 );
	~Simulation();
};

}}

#endif // SIMULATION_H