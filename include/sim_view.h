#ifndef SIMULATION_H
#define SIMULATION_H

#include "view.h"

class SimView : public View{
private:
	
public:
	void init();

	void notify( const Simulation* s, std::string message );
};

#endif