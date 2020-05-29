#include "../include/loader.h"

using namespace morph::animats;

void XMLLoader::load( Simulation *s, char *dir ){
	
	XMLDocument doc;
	//chdir(dir); // Changes working directory
	string experimentFile = string(dir) + string("./experiment.xml");
	debugger.log(string("Loading experiment from ") + experimentFile, GENERAL, "LOADER");
	doc.LoadFile( experimentFile.c_str() );
	XMLPrinter printer;
	
	XMLNode *root = doc.FirstChild();

	if( root != NULL &&  strcmp(root->ToElement()->Name(), "experiment") != 0 ){
		// Error
		throw "Incorrect format for the experiment";
	}

	XMLNode *it = root->FirstChild();

	while( it != NULL ){
		const char* element = it->ToElement()->Name();

		if( strcmp( element, "plane" ) == 0 ){
			debugger.log(string("Adding a plane"), GENERAL, "LOADER");
			this->addPlane( s, it );
		}
		else if( strcmp( element, "animat" ) == 0 ){
			debugger.log(string("Adding an animat"), GENERAL, "LOADER");
			this->addAnimat( s, it );
		}else if( strcmp( element, "view" ) == 0 ){
			debugger.log(string("Adding a simview"), GENERAL, "LOADER");
			this->addView( s, it );
		}
		else // error
			throw "Unrecognized node in the experiment file.";

		it = it->NextSibling();
	}
}

void XMLLoader::addPlane( Simulation *s, XMLNode* node ){
	// Initialize standard position plane
	RigidBody *p = s->addRigidBody( rand()%2000, "plane" );
	GeometricTransform gt;	
	
	XMLNode *it = node->FirstChild();

	while( it != NULL ){
		XMLElement* element = it->ToElement();
		const char* elementName = element->Name();

		if( strcmp( elementName, "scale") == 0 ){
			vec sc = ep.parseVector( element->GetText() );
			ScaleTransform sct( sc(0), sc(1), 1.0 );
			gt.compose( sct );
		}else if( strcmp( elementName, "position" ) == 0 ){
			vec pos = ep.parseVector( element->GetText() );
			TranslateTransform tt( pos(0), pos(1), pos(2) );
			gt.compose( tt );
		}else if( strcmp( elementName, "rotation" ) == 0 ){

			vec angles = ep.parseVector( element->GetText() );			
			RotateTransform rt( angles(0), angles(1), angles(2));
			gt.compose( rt );
		}else if( strcmp( elementName, "collitionable" ) == 0 ){
			bool c = ep.parseBool( element->GetText() );
			//p.setCollitionable( c );
		}else
			throw "Unrecognized plane options";// exception

		it = it->NextSibling();
	}

	State state;
	state.v = {0,0,0};
	state.T = gt;
	p->init( state, 100000 );	
}

void XMLLoader::addView( Simulation *s, XMLNode * node ){
	debugger.log(string("Initializing the view to be added"), GENERAL, "LOADER");
	SimView *sview = new SimView( *s );
	VideoRecorder *vr = new VideoRecorder(sview);
	XMLElement* element =  node->ToElement();	
	vec p = ep.parseVector( element->GetText() );
	sview->setViewPort(p);
	s->addView( sview );
	s->addView( vr );
}

void XMLLoader::addAnimat( Simulation *s, XMLNode *node ){
	
	
	XMLNode *it = node->FirstChild();	
	int id = node->ToElement()->IntAttribute("id");
	int rate = node->ToElement()->IntAttribute("rate");
	int max_num = node->ToElement()->IntAttribute("max");
	bool c = node->ToElement()->BoolAttribute("visible");

	SoftBody *a = s->addSoftBody( id );
	a->setVisible( c );

	GeometricTransform gt;

	while( it != NULL ){
		XMLElement* element = it->ToElement();		
		const char* elementName = element->Name();

		if( strcmp( elementName, "scale") == 0 ){
			vec sc = ep.parseVector( element->GetText() );
			ScaleTransform sct( sc(0), sc(1), sc(2) );
			gt.compose( sct );
		}else if( strcmp( elementName, "position" ) == 0 ){
			vec pos = ep.parseVector( element->GetText() );
			TranslateTransform tt( pos(0), pos(1), pos(2) );
			gt.compose( tt );
		}else if( strcmp( elementName, "rotate" ) == 0 ){
			vec angles = ep.parseVector( element->GetText() );
			RotateTransform rt( angles(0), angles(1), angles(2) );
			gt.compose( rt );
		}else if( strcmp( elementName, "softness" ) == 0 ){
			double s = ep.parseDouble( element->GetText() );
			
			a->getShape()->setAlpha( s );		
		}else
			throw "Unrecognized plane options";// exception

		it = it->NextSibling();
	}

	State state;
	state.v = {0,0,0};
	state.T = gt;
	a->init( state );
}

// ExperimentParser
vec ExperimentParser::parseVector( const char *element ){
	string item(element);
	regex re("^\\((-*[0-9]+.[0-9]*),(-*[0-9]+.[0-9]*),(-*[0-9]+.[0-9]*)\\)");
	std::smatch match;
	std::regex_search(item, match, re);
	vec v = {0, 0, 0};

	if( match.size() > 1 )
	v = {stod(match.str(1)), stod(match.str(2)), stod(match.str(3))};

	return v;
}
	
bool ExperimentParser::parseBool( const char *element ){
	bool v = strcmp( element, "true" ) == 0;
	return v;
}

int ExperimentParser::parseInt( const char *element ){
	int v = stoi( element );
	return v;
}

double ExperimentParser::parseDouble( const char *element ){
	double v = stod( element );
	return v;
}
