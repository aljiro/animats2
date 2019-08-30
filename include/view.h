#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include "simulation.h"

namespace morph{ namespace animats{

class Simulation;

class View{

public:
	virtual void notify( Simulation& s, std::string message ) = 0;
};

}}

#endif