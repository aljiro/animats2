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

void ContactList::clear(){
	contacts.clear();
}

vector<Contact *> ContactList::getContacts(){
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

bool SignoriniContact::isResting(){
	bool resting = true;

	if( collisions.size() == 0 )
		resting = false;

	for( vector<CollisionInformation>::iterator it = collisions.begin(); 
			it != collisions.end(); ++it ){
		Point *p = (*it).point;
		Face *f = (*it).face;

		if( norm(p->v) > 0.5 ){
			cout << "Because: " << norm(p->v) << endl;
			resting = false;
		}
	}

	cout << "RESTING? "<< resting << endl;

	return resting;
}

void SignoriniContact::addCollision( CollisionInformation ci ){
	
	bool found = false;

	for ( CollisionInformation stored : collisions )
		if( stored.point == ci.point && stored.face == ci.face )
			found = true;

	if( !found )
		this->collisions.push_back(ci);
}

void SignoriniContact::solveContactRegion(){
	Debug::log(string("Solving signorini contact region"), LOOP);
// Move each point to the face
	for( CollisionInformation ci : this->collisions ){
		Point *p = ci.point;
		Face *f = ci.face;

		// if( !p->move )
		//  	continue;

		p->move = false; // This point won't move anymore until free
		Edge e( p->pre, p );

		//p->vi = p->x - f->getFaceProjection2( e );
		p->x = f->getFaceProjection2( e );
		p->v = zeros<vec>(3);
	}

}

void SignoriniContact::resolve(){
	Debug::log(string("Resolving collision"), LOOP);
	cout << collisions.size() << " collisions so far " << endl;
	// if( !this->isResting() ){
	vec delta = zeros<vec>(3);

	for( vector<CollisionInformation>::iterator it = collisions.begin(); 
		it != collisions.end(); ++it ){
		Point *p = (*it).point; // Point at which the collition is hapenning
		Face *f = (*it).face;
		GeometricObject *go = (*it).goPoint;

		// Adding reaction
	}

	cout << "Finish resolve"<<endl;

	// }
	//cin.get();
}

void SignoriniContact::prunePoints(){
	Debug::log(string("Prunning signirini contact"), LOOP);
	vector<int> toDel;

	cout << "Number of contact points: " << collisions.size()<<endl;

	for( int i = 0; i < collisions.size(); i++ ){

		Point *p = collisions[i].point;
		Face *f = collisions[i].face;

		// double gp = dot( p->g)
		// Normal pressure
		double vel = dot(p->v, f->normal);
		double acc = dot(p->v - p->pre->v, f->normal);

		cout << "Actual velocity: " << printvec(p->v) << endl;
		// Debug::log(string("Checking prunning condition"), LOOP);
		if(  vel >= 0.0 ){//} && norm(p->v) > 0.01 ){
			cout << "Prunning point!" << endl;
			Debug::log(string("Prunning condition satisfied"), LOOP);
			p->move = true;
			//p->vc = zeros<vec>(3);
			Debug::log(string("Erasing contact"), LOOP);
			toDel.push_back(i);
			Debug::log(string("Done"), LOOP);
		}
	}

	cout << "Contacts to erase: " << toDel.size() << " of " << collisions.size() << endl;
	vector<CollisionInformation> cis;
	int p = 0;
	int k = 0;

	for( int i = 0; i < collisions.size(); i++ ){
		p = (k < toDel.size())? toDel[k] : -1;
		
		if( i == p ){
			k++;
			continue;
		}

		cis.push_back( collisions[i] );
	}

	collisions = cis;
	cout << "Finish prunning" << endl;
	cout << "After erasing: " << collisions.size() << endl;
	// cin.get();
	Debug::log(string("Contacts prunned"), LOOP);
}

// -----------------------------------------------------------------
// DeformableContact
// -----------------------------------------------------------------

DeformableContact::DeformableContact( 
				   SoftBody *A, SoftBody *B):Contact( A, B ){
	sepPlane = NULL;
}

bool DeformableContact::isResting(){
	return false;
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

bool DeformableContact::isContactRegionValid(){
	bool valid = true;
	
	for( CollisionInformation ci : collisionsA ){
		Point *p = ci.point;
		Face *f = ci.face;
		Edge e( p->pre, p );

		// if( !f->isInside(p->x)){
		if( f->getPenetrationDepth2(e) > 0.01 ){
			
			valid = false;
			break;
		}
	}

	for( CollisionInformation ci : collisionsB ){
		Point *p = ci.point;
		Face *f = ci.face;
		Edge e( p->pre, p );

		if( f->getPenetrationDepth2(e) > 0.01 ){
			
			valid = false;
			break;
		}
	}

	return valid;
}

void DeformableContact::solveContactRegion(){
	Debug::log(string("Solving deformable contact"));
	
	cout << "Solving contact region" << endl;

	while( !isContactRegionValid() ){
		// doprojections
		for( CollisionInformation ci : collisionsA ){
			Point *p = ci.point;
			Face *f = ci.face;
			Edge e( p->pre, p );

			p->move = false;
			p->x = f->getFaceProjection2(e);
		}

		
		for( CollisionInformation ci : collisionsB ){
			Point *p = ci.point;
			Face *f = ci.face;
			Edge e( p->pre, p );

			p->move = false;
			p->x = f->getFaceProjection2(e);
		}		

	}
}

void DeformableContact::computeSeparatingPlane(){
	double Delta = 0;
	Face *fDelta = NULL;
	vec dir = zeros<vec>(3);

	// Get separating hyperplane and delta
	for( CollisionInformation ci : collisionsA ){
		Point *p = ci.point;
		Face *f = ci.face;
		Edge e( p->pre, p );

		double h = f->getPenetrationDepth( e );

		if( h > Delta ){
			Delta = h;
			fDelta = f;
			dir = p->x - p->pre->x;
		}
	}

	// Get separating hyperplane and delta
	for( CollisionInformation ci : collisionsB ){
		Point *p = ci.point;
		Face *f = ci.face;
		Edge e( p->pre, p );

		double h = f->getPenetrationDepth( e );

		if( h > Delta ){
			Delta = h;
			fDelta = f;
			dir = p->x - p->pre->x;
		}
	}

	if( fDelta == NULL ) return;

	cout << "Creating plane"<< endl;
	dir = dir/norm(dir);
	sepPlane = new Face( *fDelta );
	sepPlane->computeNormal();
	vec cmA = computeCenterOfMass( A->getPoints() );
	vec d = cmA - sepPlane->getCentroid();

	if( dot( sepPlane->normal, d ) < 0 )
		sepPlane->normal = -sepPlane->normal;

}
	

void DeformableContact::resolve(){
// Computing the response force for each coll
		double Cr =  0.5;
		double m1 = 0;
		double m2 = 0;
		double j;
		vec v2 = zeros<vec>(3);
		vec v1 = zeros<vec>(3);
		vec n;

	for( CollisionInformation ci : collisionsA ){
		Face *f = ci.face;
		Point *p = ci.point;
		m1 = p->m;
		m2 = f->points[0]->m;
		v2 = (f->points[0]->v + f->points[1]->v + f->points[2]->v)/3;
		v1 = p->v;
		n = f->normal;

		j = (1.0 + Cr)*dot(v1 - v2, n)/(1/m1 + 1/m2);
		p->pre->v = zeros<vec>(3);
		p->v = j*n/m1;
	}

	for( CollisionInformation ci : collisionsB ){
		Face *f = ci.face;
		Point *p = ci.point;
		m1 = p->m;
		m2 = f->points[0]->m;
		v2 = (f->points[0]->v + f->points[1]->v + f->points[2]->v)/3;
		v1 = p->v;
		n = f->normal;

		j = (1.0 + Cr)*dot(v1 - v2, n)/(1/m1 + 1/m2);

		p->pre->v = zeros<vec>(3);
		p->v = j*n/m1;
	}

	if( !react )
		return;

	if( collisionsA.size() == 0 || collisionsB.size() == 0 )
		return;

	
	m1 = 0;
	m2 = 0;
	v2 = zeros<vec>(3);
	v1 = zeros<vec>(3);
	computeSeparatingPlane();

	n = sepPlane->normal/norm(sepPlane->normal);
	
	for( Point *p : A->getPoints() ){
		v1 += p->v/A->getPoints().size();
		m1 += p->m;
	}

	for( Point *p : B->getPoints() ){
		v2 += p->v/B->getPoints().size();
		m2 += p->m;
	}

	j = -(1.0 + Cr)*dot(v1 - v2, n);
	cout << "Applying impulse: " << j << endl;

	for( Point *p : A->getPoints() ){
	// for(CollisionInformation ci : collisionsA){
	// 	Point *p = ci.point;
		p->pre->v = zeros<vec>(3);
		p->v = j*n;
	}

	for( Point *p : B->getPoints() ){
	// for(CollisionInformation ci : collisionsB){
	// 	Point *p = ci.point;
		p->pre->v = zeros<vec>(3);	
		p->v = -j*n;
	}

	

	

}

void DeformableContact::pruneCollisionList( vector<CollisionInformation>& collisions, GeometricObject *go ){
	vector<vector<CollisionInformation>::iterator> toDel;
	double vel;
	double acc;

	for( vector<CollisionInformation>::iterator it = collisions.begin(); 
		it != collisions.end(); ++it ){
		
		Point *p = (*it).point;
		Face *f = (*it).face;
		// Normal pressure
		f->computeNormal();
		vec cmA = computeCenterOfMass( go->getPoints() );
		vec d = cmA - f->getCentroid();

		if( dot( f->normal, d ) < 0 )
			f->normal = -f->normal;

		Edge e( p->pre, p );
		
		// if( dot(p->x - p->pre->x, f->normal) > 0 )
		// 	f->normal = -f->normal;

		vel = dot(p->v, f->normal);
		acc = dot(p->v - p->pre->v, f->normal);
	
		if( abs(vel) > 0.2 )
			this->react = false;
		
		// Debug::log(string("Checking prunning condition"), LOOP);
		if( f->getPenetrationDepth2(e) > 0.01 || ( acc >= 0.01) ){
			Debug::log(string("Prunning condition satisfied"), LOOP);
			cout << "Prunning: " << acc << endl;
			 //cin.get();
			p->move = true;
			Debug::log(string("Erasing contact"), LOOP);
			toDel.push_back(it);
			Debug::log(string("Done"), LOOP);
		}
		else{
			vec u = f->normal/norm(f->normal);
			mat Pr = eye(3,3) - u*u.t();
			p->pre->v = Pr*p->pre->v;
			p->v = 0.1*Pr*p->v;
		}
	}

	for( vector<CollisionInformation>::iterator it: toDel )
		collisions.erase(it);

	toDel.clear();
}

void DeformableContact::prunePoints(){
	Debug::log(string("Prunning deformable contact"), LOOP);

	if( collisionsA.size() > 3 || collisionsB.size() > 3 )
		this->react = true;
	
	pruneCollisionList( collisionsA, A );
	pruneCollisionList( collisionsB, B );	

}

// Rigid contact
RigidContact::RigidContact( RigidBody *A, RigidBody *B ):Contact( A, B ){

}

void RigidContact::addCollision( CollisionInformation ci ){

}

void RigidContact::solveContactRegion(){

}

void RigidContact::resolve(){

}

bool RigidContact::isResting(){
	return false;
}

void RigidContact::prunePoints(){

}