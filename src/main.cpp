/*
 * Soft Animats Simulator
 * v 1.0
 */

#include "simulation.h"
#include "simview.h"


int main( int argc, char** args ){

	if (argc < 2) {
        cerr << "\nUsage: ./animats experiment_dir \n\n";
        return -1;
    }

	Simulation *s = Simulation.load( args[1] );
	SimView *sview = new SimView();
	s->addView( sview );

	s.run( 100 );
}