#ifndef LOADER_H
#define LOADER_H

#include "simulation.h"

#include <vector>
#include <iostream>
#include <armadillo>
#include <string>
#include <regex>

#include "tinyxml2.h"

using namespace arma;
using namespace tinyxml2;

namespace morph{
namespace Animats{

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
	virtual void load( Simulation* s, char *name ) = 0
};

class XMLLoader : public Loader{
public:
	void addPlane( XMLNode * );
	void addAnimat( XMLNode * );

	void load( Simulation *s, char *name );
};

class JSONLoader : public Loader{
public:
	void load( Simulation* s, char *name );
};

}}
#endif