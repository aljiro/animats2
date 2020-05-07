#ifndef SOLVER_H
#define SOLVER_H

#include "simulation.h"
#include "gobject.h"
#include "util.h"
#include "point.h"

namespace morph{ namespace animats{
class Simulation;

/**
 * Solver - Integrates the equations of motion for all of the bodies
 */
class Solver{
private:
	double h;
	double t;
	// Integrates the equations of motion for one soft body
	void integrateSoftBody( SoftBody *go );
	void computeGoals( Simulation& s );
public:
	explicit Solver( double h = 0.01 );
	// Produces one step of the simulation
	void step( Simulation& s );	
};

}}
#endif // SOLVER_H