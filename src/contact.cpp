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
	for( vector<Contact *>::iterator it = contacts.begin(); 
		it != contacts.end(); ++it ){
		if( (*it)->prune() )
			contacts.erase( it );
	}
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
		p->xc = f->getFaceProjection( e );
		//pu = f->getPenetrationDepth( e );
		// Adding reaction

		// p->fc = epsilon*pu*f->normal;
	}
}

bool SignoriniContact::prune(){
	vector<vector<CollisionInformation>::iterator> toDel;

	for( vector<CollisionInformation>::iterator it = collisions.begin(); 
		it != collisions.end(); ++it ){
		Point *p = (*it).point; // Point at which the collition is hapenning
		Face *f = (*it).face;
		Edge e( p->pre, p );

		if( !f->isPenetrated( e ) ){
			p->state = Free;
			toDel.push_back(it);
		}
	}

	for( vector<CollisionInformation>::iterator it : toDel )
		collisions.erase(it);

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

bool DeformableContact::prune(){
	vector<vector<CollisionInformation>::iterator> toDelA,toDelB;

	for( vector<CollisionInformation>::iterator it = collisionsA.begin(); 
		it != collisionsA.end(); ++it ){
		Point *p = (*it).point; // Point at which the collition is hapenning
		Face *f = (*it).face;
		Edge e( p->pre, p );

		if( !f->isPenetrated( e ) ){
			p->state = Free;
			toDelA.push_back(it);
		}
	}

	for( vector<CollisionInformation>::iterator it = collisionsB.begin(); 
		it != collisionsB.end(); ++it ){
		Point *p = (*it).point; // Point at which the collition is hapenning
		Face *f = (*it).face;
		Edge e( p->pre, p );

		if( !f->isPenetrated( e ) ){
			p->state = Free;
			toDelB.push_back(it);
		}
	}

	for( vector<CollisionInformation>::iterator it : toDelA )
		collisionsA.erase(it);

	for( vector<CollisionInformation>::iterator it : toDelB )
		collisionsB.erase(it);

	return collisionsA.empty() and collisionsB.empty();
}


void DeformableContact::resolve(){
	for( vector<CollisionInformation>::iterator it = collisionsA.begin(); 
		it != collisionsA.end(); ++it ){
		Point *p = (*it).point; // Point at which the collition is hapenning
		Face *f = (*it).face;
		Edge e( p->pre, p );
		p->xc = f->getFaceProjection2( e );
		//pu = f->getPenetrationDepth( e );
		// Adding reaction

		// p->fc = epsilon*pu*f->normal;
	}

	for( vector<CollisionInformation>::iterator it = collisionsB.begin(); 
		it != collisionsB.end(); ++it ){
		Point *p = (*it).point; // Point at which the collition is hapenning
		Face *f = (*it).face;
		Edge e( p->pre, p );
		p->xc = f->getFaceProjection2( e );
		//pu = f->getPenetrationDepth( e );
		// Adding reaction

		// p->fc = epsilon*pu*f->normal;
	}
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
