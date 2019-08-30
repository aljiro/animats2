#include "../include/contact.h"

using namespace morph::animats;

Contact::Contact(){

}
	
void Contact::resolve(){

}


Contact::~Contact(){
	
}


// Signorini
void SignoriniContact::solveContactRegion(){
	// Determine hyperplane

	// Move the points of the softbdy until they satisfy the inequalities
	while( satisfied )
	{
		updateDisplacements()
		satisfied = evaluateRegion();
	}
}

void SignoriniContact::resolve(){

}

// DeformableContact
