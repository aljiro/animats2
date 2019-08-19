#ifndef LOADER_H
#define LOADER_H

#include "util.h"
#include "enviroment.h"
#include "body.h"
#include "brain.h"


#include <vector>
#include <iostream>
#include <armadillo>
#include <string>
#include <regex>

#include "tinyxml2.h"

using namespace arma;
using namespace std;
using namespace tinyxml2;
using namespace morph;

class ExperimentParser{
public:
	vec parseVector( const char * );
	bool parseBool( const char * );
	int parseInt( const char * );
	double parseDouble( const char * );
};

class Loader{
private:
	Enviroment *enviroment;	
	ExperimentParser ep;
public:
	Loader( Enviroment *e );
	
	void addPlane( XMLNode * );
	void addAnimat( XMLNode * );

	virtual void load( char *dir ) = 0
	virtual ~Loader();
};


#endif