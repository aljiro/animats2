#include "../include/loader.h"

using namespace morph::animats;

void XMLLoader::load( Simulation *s, char *dir ){
	
	XMLDocument doc;
	//chdir(dir); // Changes working directory
	string experimentFile = string(dir) + string("./experiment.xml");
	Debug::log(string("Loading experiment from ") + experimentFile);
	doc.LoadFile( experimentFile.c_str() );
	XMLPrinter printer;
	
	XMLNode *root = doc.FirstChild();

	if( root != NULL &&  strcmp(root->ToElement()->Name(), "experiment") != 0 ){
		// Error
		Debug::log("Incorrect format.");
	}

	XMLNode *it = root->FirstChild();

	while( it != NULL ){
		const char* element = it->ToElement()->Name();

		if( strcmp( element, "plane" ) == 0 ){
			Debug::log(string("Adding a plane"));
			this->addPlane( s, it );
		}
		else if( strcmp( element, "animat" ) == 0 ){
			Debug::log(string("Adding an animat"));
			this->addAnimat( s, it );
		}else if( strcmp( element, "view" ) == 0 ){
			Debug::log(string("Adding a simview"));
			this->addView( s, it );
		}
		else // error
			Debug::log("Unrecognized node in the experiment file.");

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
			Debug::log(string("Rotating rigid: ")+printvec(angles));
			gt.compose( rt );
		}else if( strcmp( elementName, "collitionable" ) == 0 ){
			bool c = ep.parseBool( element->GetText() );
			//p.setCollitionable( c );
		}else
			Debug::log("Unrecognized plane options");// exception

		it = it->NextSibling();
	}

	State state;
	state.v = {0,0,0};
	state.T = gt;
	p->init( state, 100000 );	
}

void XMLLoader::addView( Simulation *s, XMLNode * node ){
	Debug::log(string("Initializing the view to be added"));
	SimView *sview = new SimView( *s );
	XMLElement* element =  node->ToElement();	
	vec p = ep.parseVector( element->GetText() );
	Debug::log(string("Setting viewport"));
	sview->setViewPort(p);
	s->addView( sview );
}

void XMLLoader::addAnimat( Simulation *s, XMLNode *node ){
	
	Debug::log(string("Obtaining first child"));
	XMLNode *it = node->FirstChild();	
	Debug::log(string("Obtaining attribute"));	
	int id = node->ToElement()->IntAttribute("id");
	int rate = node->ToElement()->IntAttribute("rate");
	int max_num = node->ToElement()->IntAttribute("max");
	bool c = node->ToElement()->BoolAttribute("visible");

	Debug::log(string("Adding animat to the enviroment"));
	SoftBody *a = s->addSoftBody( id );
	a->setVisible( c );
	Debug::log(string("Starting iteration"));

	GeometricTransform gt;

	while( it != NULL ){
		XMLElement* element = it->ToElement();		
		const char* elementName = element->Name();

		if( strcmp( elementName, "scale") == 0 ){
			vec sc = ep.parseVector( element->GetText() );
			Debug::log(string("Scaling: ")+printvec(sc));
			ScaleTransform sct( sc(0), sc(1), sc(2) );
			gt.compose( sct );
		}else if( strcmp( elementName, "position" ) == 0 ){
			vec pos = ep.parseVector( element->GetText() );
			Debug::log(string("Translating: ")+printvec(pos));
			TranslateTransform tt( pos(0), pos(1), pos(2) );
			gt.compose( tt );
		}else if( strcmp( elementName, "rotate" ) == 0 ){
			vec angles = ep.parseVector( element->GetText() );
			Debug::log(string("Rotating: ")+printvec(angles));
			RotateTransform rt( angles(0), angles(1), angles(2) );
			gt.compose( rt );
		}else if( strcmp( elementName, "softness" ) == 0 ){
			double s = ep.parseDouble( element->GetText() );
			
			a->getShape()->setAlpha( s );		
		}else
			Debug::log("Unrecognized plane options");// exception

		it = it->NextSibling();
	}

	State state;
	state.v = {0,0,0};
	state.T = gt;
	a->init( state );
	cout << "Animat added: " << a->getPoints().size() << " points, " << a->getFaces().size() << " faces. " << endl;
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
