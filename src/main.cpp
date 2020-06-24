/*
 * Soft Animats Simulator
 * v 1.0
 */

#include "../include/simulation.h"
#include "../include/simview.h"
#include "../include/report.h"
#include "time.h"

using namespace morph::animats;


int main( int argc, char** args ){

	srand (time(NULL));

	if (argc < 2) {
        cerr << "\nUsage: ./animats experiment_dir \n\n";
        return -1;
    }

	Simulation *s = Simulation::load( args[1] );
	GravityForce *gf = new GravityForce(NULL);
	s->addForce( gf );
	// s->addView( new ReportView(*s, ReportView::DUMP_POINTS | 
								//    ReportView::DUMP_CONTACTS) );
	s->reset();
	s->run( -1 );
}