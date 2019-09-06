/*
 * Soft Animats Simulator
 * v 1.0
 */

#include "../include/simulation.h"
#include "../include/simview.h"
#include "time.h"

using namespace morph::animats;


int main( int argc, char** args ){

	srand (time(NULL));

	if (argc < 2) {
        cerr << "\nUsage: ./animats experiment_dir \n\n";
        return -1;
    }

	Simulation *s = Simulation::load( args[1] );
	SimView *sview = new SimView( *s );
	GravityForce *gf = new GravityForce(NULL);
	s->addForce( gf );
	s->addView( sview );
	s->reset();
	s->run( 1000 );
}