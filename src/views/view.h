#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include "../core/simulation.h"

namespace morph{ namespace animats{

class Simulation;

class View{

public:
	virtual void notify( Simulation& s, std::string message ) = 0;
	virtual void setup( Simulation& s ) = 0;
};

}}

#endif