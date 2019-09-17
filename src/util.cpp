#include "../include/util.h"

using namespace morph::animats;

vec morph::animats::idTx( vec q ){
	return q;
}

vec morph::animats::quadTx( vec q ){
	vec qc = {q(0), q(1), q(2), q(0)*q(0), q(1)*q(1), q(2)*q(2), q(0)*q(1), q(1)*q(2), q(0)*q(2)};
	return qc;
}

void Debug::log( std::string msg, Level level ){
	if( level == LOOP && debug_level == GENERAL )
		return;

	if( Debug::debugging ){
		if( Debug::type == STD_OUT ){
			std::cout << ">>" << msg << std::endl;
		}
		else if( Debug::type == FILE ){
			lout << ">>" << msg << std::endl;
		}
	}
}

bool Debug::debugging = false;
int Debug::type = Debug::STD_OUT;
std::ofstream Debug::lout;
Level Debug::debug_level = GENERAL;