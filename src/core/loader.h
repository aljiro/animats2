#ifndef LOADER_H
#define LOADER_H

#include "simulation.h"
#include "../utilities/util.h"
#include "../views/simview.h"

#include <vector>
#include <iostream>
#include <armadillo>
#include <string>
#include <regex>

#include "tinyxml2.h"

using namespace arma;
using namespace tinyxml2;

namespace morph{
namespace animats{

class Simulation;

class ExperimentParser{
public:
	vec parseVector( const char * );
	bool parseBool( const char * );
	int parseInt( const char * );
	double parseDouble( const char * );
};

class Loader{
protected:
	ExperimentParser ep;
public:
	virtual void load( Simulation* s, char *name ) = 0;
};

class XMLLoader : public Loader{
private:
	void addPlane( Simulation *s, XMLNode * );
	void addAnimat( Simulation *s, XMLNode * );
	void addView( Simulation *s, XMLNode * );

public:
	void load( Simulation *s, char *name );
};

class JSONLoader : public Loader{
public:
	void load( Simulation* s, char *name );
};

}}
#endif