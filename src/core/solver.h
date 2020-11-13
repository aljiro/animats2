#ifndef SOLVER_H
#define SOLVER_H

#include "environment.h"
#include "../geometry/gobject.h"
#include "../geometry/point.h"
#include "../utilities/util.h"

namespace morph{ namespace animats{
class Simulation;

/**
 * Solver - Integrates the equations of motion for all of the bodies
 */
class Solver{
private:
	Environment* environment;
	
	double h0;
	double t;
	// Integrates the equations of motion for one soft body
	void integrateSoftBody( SoftBody *go );
	void computeGoals();
public:
	double h;
	explicit Solver( Environment* environment, double h = 0.01 );
	// Produces one step of the simulation
	void stepMaterial( );	
	void partialStep( double hc );
	void stepCollisions( );
};

}}
#endif // SOLVER_H