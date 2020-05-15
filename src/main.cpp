/*
 * Soft Animats Simulator
 * v 1.0
 */

#include "../include/simulation.h"
#include "../include/simview.h"
#include "../include/report.h"
#include "../include/contactview.h"
#include "time.h"

using namespace morph::animats;

int processArgs( int argc, char** args ){
	if (argc < 2) {
        cerr << "\nUsage: ./animats experiment_dir [options]\n\n";
        return -1;
    }

    bool error = false;

    for( int i = 2; i < argc; i++ ){
    	try{
    		if( std::string(args[i]) == "--debug" )
    			Debug::debugging = std::string(args[++i])=="true";

    		if( std::string(args[i]) == "--debug-origin")
    			Debug::origin = std::string(args[++i]);

    		if( std::string(args[i]) == "--debug-loops")
    			Debug::debug_level = std::string(args[++i])=="true"? LOOP : GENERAL;
    	}catch(const std::exception& e){
    		error = true;
    		break;
    	}

    }

    if( error ){
    	debugger.log("Error parsing the parameters", GENERAL, "main");
    	return -1;
    }

    return 0;
}

int main( int argc, char** args ){

	srand (time(NULL));

	if( processArgs( argc, args ) != 0 )
		return -1;
	
	Simulation *s = Simulation::load( args[1] );
	GravityForce *gf = new GravityForce(NULL);
	s->addForce( gf );
	//s->addView( new ContactView( *s ) );
	//s->addView( new ReportView(*s, ReportView::DUMP_POINTS | 
	//							   ReportView::DUMP_CONTACTS) );
	s->reset();
	s->run( -1 );
}