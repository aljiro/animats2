#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <vector>
#include <memory>
#include "../views/view.h"
#include "solver.h"
#include "loader.h"
#include "environment.h"
#include "../geometry/gobject.h"
#include "../collision/collision.h"
#include "../collision/cmgr.h"
#include "../collision/contact.h"


using namespace std;

namespace morph{ namespace animats{
class View;

class Simulation{
private:
		
	// State variables
	bool running;
	string workingDir;
	Solver* solver;
	Simulation( string workingDir );
public:
// Views to be notified at each step
	vector<View *> views;
	Environment *environment;
	unsigned int step;
	bool pause;
	// Collision manager
	CollisionManager *collisionMgr;

	static Simulation* load( char* name );	
	// Adds a new view to the simulation
	void addView( View *view );
	// Notifies the listening views of the opdate
	void notifyViews( string msg );
	void setEnvironment( Environment* environment );
	void reset();	
	// Registers all the objects that should be taking into account
	// by the collision manager
	void registerObjectsForCollision();
	void close();
	void togglePause();
	void run( int maxSteps = -1 );
	~Simulation();
};

}}

#endif // SIMULATION_H