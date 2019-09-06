#ifndef SOLVER_H
#define SOLVER_H

#include "simulation.h"
#include "gobject.h"
#include "util.h"
#include "point.h"

namespace morph{ namespace animats{

class Simulation;

class Solver{
private:
	double h;
	double t;
public:
	explicit Solver( double h = 0.01 );
	void step( Simulation& s );
};


}}

#endif