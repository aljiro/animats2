/*
 * Soft Animats Simulator
 * v 1.0
 */

#include "simulation.h"




int main( int argc, char** args ){

	if (argc < 2) {
        cerr << "\nUsage: ./animats experiment_file \n\n";
        return -1;
    }

	Simulation *s = Simulation.load( args[1] );
	s.run();
}