#ifndef REPORT_H
#define REPORT_H

#include <iostream>
#include <fstream>
#include <vector>
#include "../core/simulation.h"
#include "../collision/contact.h"
#include "../geometry/gobject.h"
#include "view.h"

using namespace std;

namespace morph{ namespace animats{

class ReportView : public View{
private:
	unsigned short options;
	fstream fpoints;
	fstream fcontacts;
public:
	static const unsigned short DUMP_POINTS = 0x0001;
	static const unsigned short DUMP_CONTACTS = 0x0002;

	ReportView( Simulation& s, unsigned short options );
	void notify( Simulation& s, std::string message );
	void setup( Simulation& s );

	void dumpPoints( Simulation& s );
	void dumpContactArea( Simulation& s );

	~ReportView();
};

}}

#endif