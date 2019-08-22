#ifndef VIEW_H
#define VIEW_H

#include "simulation.h"

namespace morph{ namespace animats{

class View{

public:
	virtual void notify( const Simulation& s, std::string message ) = 0;
};

}}