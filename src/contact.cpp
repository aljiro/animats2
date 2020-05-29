#include "../include/contact.h"

using namespace morph::animats;

// Contact list
ContactList::ContactList(){

}

void ContactList::resolveForces(){
	for( vector<Contact *>::iterator it = contacts.begin(); 
		 it != contacts.end(); ++it){
		(*it)->resolve();
	}
}

Contact *ContactList::findContact( GeometricObject *A, GeometricObject *B ){
	for( Contact *fc : contacts )
		if( (fc->A == A && fc->B == B) ||
			(fc->B == A && fc->A == B))
			return fc;

	return NULL;
}

Contact *ContactList::addNewContact( GeometricObject *A, GeometricObject *B ){
	if( A == NULL || B == NULL )
		return NULL;

	Contact *c;

	if( (A->type == RIGID || A->type == PLANE) && B->type == SOFT )
		c = new SignoriniContact( (SoftBody *)B, (RigidBody*) A );
	else if( A->type == SOFT && (B->type == RIGID || B->type == PLANE) )
		c = new SignoriniContact( (SoftBody *)A, (RigidBody*) B );
	else if( A->type == SOFT && B->type == SOFT )
		c = new DeformableContact( (SoftBody *)A, (SoftBody *)B );
	else
		c = new RigidContact((RigidBody*)A, (RigidBody*) B );

	this->contacts.push_back( c );
	return c;
}

void ContactList::add( GeometricObject *A, GeometricObject *B, CollisionInformation ci ){
	Contact *c = findContact(A, B);

	if( c == NULL ){
		c = addNewContact( A, B );
	}

	c->addCollision( ci );
}

void ContactList::pruneContacts(){
	vector<Contact *> nContacts;

	for( vector<Contact *>::iterator it = contacts.begin(); 
		it != contacts.end(); ++it ){
		if( !(*it)->prune() )
			nContacts.push_back( (*it) );
	}

	contacts = nContacts;
}

void ContactList::clear(){
	contacts.clear();
}

vector<Contact *>& ContactList::getContacts(){
	return contacts;
}

// Contacts
Contact::Contact( GeometricObject* A, GeometricObject* B ):A(A),B(B){

}
	
Contact::~Contact(){

}

// ----------------------------------------------------------------
// Signorini contacts
// ----------------------------------------------------------------

SignoriniContact::SignoriniContact( SoftBody *sb, RigidBody *rb ):Contact( sb, rb ){

}

void SignoriniContact::addCollision( CollisionInformation ci ){	
	bool found = false;

	for ( CollisionInformation stored : collisions )
		if( stored.point == ci.point && stored.face == ci.face )
			found = true;

	if( !found )
		this->collisions.push_back(ci);
}


void SignoriniContact::resolve(){
	debugger.log(string("Resolving collision: ") + 
		         to_string(collisions.size()) + "collisions", 
		         LOOP, "CONTACT");

	for( vector<CollisionInformation>::iterator it = collisions.begin(); 
		it != collisions.end(); ++it ){
		Point *p = (*it).point; // Point at which the collition is hapenning
		Face *f = (*it).face;
		Edge e( p->pre, p );
		
		if( f->isInsideProjection(e) )
			p->xc = p->pre->x;//f->getFaceProjection( e );
		// else
		// 	p->xc = p->x;

		p->v = zeros<vec>(3);
		
		//pu = f->getPenetrationDepth( e );
		// Adding reaction

		// p->fc = epsilon*pu*f->normal;
	}
}

bool SignoriniContact::prune(){
	vector<int> toDel;
	vector<CollisionInformation> newCollisions;

	for( int i = 0; i < collisions.size(); i++ ){
		CollisionInformation ci = collisions[i];
		Point *p = ci.point; // Point at which the collition is hapenning
		Face *f = ci.face;
		Edge e( p->pre, p );

		if( !f->isPenetrated( e ) ){
			p->state = Free;
		}else
			newCollisions.push_back(ci);
	}

	collisions = newCollisions;

	return collisions.empty();	
}

// -----------------------------------------------------------------
// DeformableContact
// -----------------------------------------------------------------

DeformableContact::DeformableContact( 
				   SoftBody *A, SoftBody *B):Contact( A, B ){
	sepPlane = NULL;
}

void DeformableContact::addCollision( CollisionInformation ci ){
	bool found = false;

	for ( CollisionInformation stored : collisionsA )
		if( stored.point == ci.point && stored.face == ci.face )
			found = true;

	for ( CollisionInformation stored : collisionsB )
		if( stored.point == ci.point && stored.face == ci.face )
			found = true;

	if( found ) return;

	if( ci.goPoint == A )
		collisionsA.push_back( ci );
	else
		collisionsB.push_back( ci );

	this->react = false;
}

bool DeformableContact::prune( int collection ){
	vector<int> toDel;
	vector<CollisionInformation> newCollisions;
	int n = collection == 0? collisionsA.size() : collisionsB.size();

	for( int i = 0; i < n; i++ ){
		CollisionInformation ci = collection == 0? collisionsA[i] : collisionsB[i];
		Point *p = ci.point; // Point at which the collition is hapenning
		Face *f = ci.face;
		Edge e( p->pre, p );

		// if( !f->isPenetrated( e ) || e.length() < 0.01 ){
		// 	p->state = Free;
		// }else
			newCollisions.push_back(ci);
	}

	if( collection == 0 )
		collisionsA = newCollisions;
	else
		collisionsB = newCollisions;

	return newCollisions.empty();	
	// return (collection == 0? collisionsA:collisionsB).empty();
}

bool DeformableContact::prune(){
	return this->prune(0) and this->prune(1);
}


void DeformableContact::resolve( int collection ){

	for( vector<CollisionInformation>::iterator it = (collection == 0 ? collisionsA:collisionsB).begin(); 
		it != (collection == 0 ? collisionsA:collisionsB).end(); ++it ){
		Point *p = (*it).point; // Point at which the collition is hapenning
		Face *f = (*it).face;
		Edge e( p->pre, p );
		debugger.log("Resolving contact", LOOP, "CONTACT");
		cout << "Contact details: " << endl;
		cout << "Face: " <<  printvec(f->points[0]->x) << ": " << 
		        printvec(f->points[1]->x) << ":" << printvec(f->points[2]->x) << endl;
		cout << "Point: pre: " << printvec(p->pre->x) << ", p: " << printvec(p->x) << endl;
		// if( f->isInsideProjection(e) )
			// p->xc = f->getFaceProjection( e );
		// else
			p->xc = p->pre->x;

		p->v = zeros<vec>(3);
	}
	// cin.get();

}

void DeformableContact::resolve(){
	resolve( 0 );
	resolve( 1 );
}


// Rigid contact
RigidContact::RigidContact( RigidBody *A, RigidBody *B ):Contact( A, B ){

}

void RigidContact::addCollision( CollisionInformation ci ){

}

void RigidContact::resolve(){

}

bool RigidContact::prune(){
	return true;
}
