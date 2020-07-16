#include "report.h"

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
	if( options & ReportView::DUMP_POINTS )
		fpoints.open("points.csv", ios::out | ios::trunc );

	if( options & ReportView::DUMP_CONTACTS )
		fcontacts.open("contacts.csv", ios::out | ios::trunc );
}
	
void ReportView::dumpPoints( Simulation& s ){
	cout << "Dumping points" << endl;

	for( SoftBody *b : s.getSoftBodies() ){
		fpoints << b->getId() << ",";

		for( int i = 0; i < b->getPoints().size()-1; i++ ){
			Point *p = b->getPoints()[i];
			fpoints << norm(p->v) << ",";
		}

		if( !b->getPoints().empty() )
			fpoints << norm(b->getPoints().back()->v) << endl;
	}
}

void ReportView::dumpContactArea( Simulation& s ){
	cout << "Dumping contacts" << endl;
	ContactList *cl = s.collisionMgr.getContactList();
	double area;

	cout << ">>>>> Number of contacts: " << cl->getContacts().size() << endl;

	for( Contact *c : cl->getContacts() ){
		cout << ">> Size of the collisions: " << ((SignoriniContact *)c)->collisions.size() << endl;
		area = ((SignoriniContact *)c)->collisions.size();
		fcontacts << area << endl;
	}

	cin.get();
}

ReportView::~ReportView(){
	fpoints.close();
	fcontacts.close();
}