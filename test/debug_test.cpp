#include "util.h"

using namespace morph::animats;

int main(){
	Debug::origin = "all";
	Debug::debugging = true;
	Debug::debug_level = GENERAL;

	std::string text;
	std::stringstream buffer;
	std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

	Debug& d = Debug::getInstance();

	// General printing
	d.log( "l1", GENERAL );
	text = buffer.str();

	cout.rdbuf( old );
	cout << text;
	
	if( text == "" )
		return -1;
	
	return 0;
}