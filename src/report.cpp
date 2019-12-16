#include "../include/report.h"

using namespace morph::animats;

ReportView::ReportView( Simulation& s, unsigned short options ){
	this->options = options;
}

// Interface
void ReportView::notify( Simulation& s, std::string message ){
	if( options & ReportView::DUMP_POINTS )
		this->dumpPoints( s );

	if( options & ReportView::DUMP_CONTACTS )
		this->dumpContactArea( s );
}
	
void ReportView::setup( Simulation& s ){
	fpoints.open("points.csv", ios::out | ios::trunc );
}
	
void ReportView::dumpPoints( Simulation& s ){
	cout << "Dumping points!" << endl;

	for( SoftBody *b : s.getSoftBodies() ){
		fpoints << b->getId() << ",";

		for( int i = 0; i < b->getPoints().size()-1; i++ ){
			Point *p = b->getPoints()[i];
			fpoints << norm(p->v) << ",";
		}

		if( !b->getPoints().empty() )
			fpoints << norm(b->getPoints().back()->x) << endl;
	}
}

void ReportView::dumpContactArea( Simulation& s ){

}

ReportView::~ReportView(){
	fpoints.close();
}