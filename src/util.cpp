#include "../include/util.h"

using namespace morph::animats;

vec morph::animats::idTx( vec q ){
	return q;
}

vec morph::animats::quadTx( vec q ){
	vec qc = {q(0), q(1), q(2), q(0)*q(0), q(1)*q(1), q(2)*q(2), q(0)*q(1), q(1)*q(2), q(0)*q(2)};
	return qc;
}

Debug& Debug::getInstance(){
	if( Debug::instance == NULL )
		Debug::instance = new Debug();

	return *(Debug::instance);
}

void Debug::registerOrigin( string name ){
	bool found = find(this->origins.begin(), 
		               this->origins.end(), 
		               name) != this->origins.end();

	if( !found )
		this->origins.push_back( name );

}

void Debug::log( std::string msg, Level level, string origin ){
	if( !Debug::debugging )
		return;

	if( level == LOOP && debug_level == GENERAL )
		return;

	if( this->origin != "all" )
		if( this->origin != origin )
			return;

	string pre = "(DEBUG - " + this->origin + ") ";

	if( Debug::type == STD_OUT ){
		std::cout << pre << msg << std::endl;
	}
	else if( Debug::type == FILE ){
		lout << pre << msg << std::endl;
	}
}

// Testing options
string Debug::origin = "all";
Debug* Debug::instance = NULL;
bool Debug::debugging = false;
int Debug::type = Debug::STD_OUT;
std::ofstream Debug::lout("log.txt", ofstream::out);
Level Debug::debug_level = GENERAL;

Debug& morph::animats::debugger = Debug::getInstance();