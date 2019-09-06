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

// Signorini
SignoriniContact::SignoriniContact( SoftBody *sb, RigidBody *rb ):Contact( sb, rb ){
}

void SignoriniContact::addCollision( CollisionInformation ci ){
	// TO-DO: check if already exists?
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

		if( !p->move )
			continue;

		p->move = false; // This point won't move anymore until free
		Edge e( p->pre, p );
		p->x = f->getFaceProjection( e );		
	}
}

void SignoriniContact::resolve(){
	Debug::log(string("Resolving collision"), LOOP);

	// for( vector<CollisionInformation>::iterator it = collisions.begin(); 
	// 	it != collisions.end(); ++it ){
	// 	Point *p = (*it).point;
	// 	Face *f = (*it).face;
	// 	p->v = -0.8*dot(p->v, f->normal)*f->normal;
	// }

	// for( Point *p : A->getPoints() ){
	// 	p->v = -0.2*p->v;
	// }
}

void SignoriniContact::prunePoints(){
	Debug::log(string("Prunning signirini contact"), LOOP);
	vector<vector<CollisionInformation>::iterator> toDel;

	for( vector<CollisionInformation>::iterator it = collisions.begin(); 
		it != collisions.end(); ++it ){
		Point *p = (*it).point;
		Face *f = (*it).face;

		// Debug::log(string("Checking prunning condition"), LOOP);
		if( dot( p->v, f->normal ) > 0 && norm(p->v)  > 0.01 ){
			Debug::log(string("Prunning condition satisfied"), LOOP);
			p->move = true;
			Debug::log(string("Erasing contact"), LOOP);
			toDel.push_back(it);
			Debug::log(string("Done"), LOOP);
		}
		// else
		// 	p->v = zeros<vec>(3);

	}

	cout << "Contacts to erase: " << toDel.size() << " of " << collisions.size() << endl;

	for( vector<CollisionInformation>::iterator it: toDel )
		collisions.erase(it);

	cout << "After erasing: " << collisions.size() << endl;
	//cin.get();
	Debug::log(string("Contacts prunned"), LOOP);
}

// DeformableContact
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
}

void DeformableContact::solveContactRegion(){
	Debug::log(string("Solving deformable contact"));
	// create hyperplane
	double hMax = 0;
	Face *fMax = NULL;
	vec dir = zeros<vec>(3);

	for( CollisionInformation ci : collisionsA ){
		Point *p = ci.point;
		Face *f = ci.face;
		Edge e( p->pre, p );

		double h = f->getPenetrationDepth( e );

		if( h > hMax ){
			hMax = h;
			fMax = f;
			dir = p->x - p->pre->x;
		}
	}

	if( fMax == NULL ) return;
	cout << "Creating plane"<< endl;
	dir = dir/norm(dir);
	sepPlane = new Face( new Point(fMax->points[0]->x + dir*hMax/2),
					 new Point(fMax->points[1]->x + dir*hMax/2),
					 new Point(fMax->points[2]->x + dir*hMax/2));

	cout <<"Copying edges"<<endl;

	for( Edge eg : fMax->edges)
		sepPlane->edges.push_back(eg);
	sepPlane->indexes = fMax->indexes;
	sepPlane->computeNormal();

	vec cmA = computeCenterOfMass( A->getPoints() );
	vec d = cmA - sepPlane->getCentroid();

	if( dot( sepPlane->normal, d ) < 0 )
		sepPlane->normal = -sepPlane->normal;

	cout << "moving the points" << endl;
	// Move all the points to satisfy inequalities
	for( CollisionInformation ci : collisionsA ){
		Point *p = ci.point;

		if( !p->move )
			continue;

		p->move = false; // This point won't move anymore until free
		Edge e( p->pre, p );
		cout << "Before projection: " << printvec(p->x) << endl;
		p->x = sepPlane->getFaceProjection( e );
		cout << "After projection: " << printvec(p->x) << endl;
		//cin.get();
	}

	// Move all the points to satisfy inequalities
	for( CollisionInformation ci : collisionsB ){
		Point *p = ci.point;

		if( !p->move )
			continue;

		p->move = false; // This point won't move anymore until free
		Edge e( p->pre, p );
		p->x = sepPlane->getFaceProjection( e );
	}

	// for( CollisionInformation ci : collisionsB ){
	// 	Point *p = ci.point;

	// 	if( !p->move )
	// 		continue;

	// 	p->move = false; // This point won't move anymore until free
	// 	Edge e( p->pre, p );
	// 	p->x = fMax->getFaceProjection( e );		
	// }
}
	

void DeformableContact::resolve(){

}

void DeformableContact::prunePoints(){
	Debug::log(string("Prunning deformable contact"), LOOP);
	vector<vector<CollisionInformation>::iterator> toDel;

	for( vector<CollisionInformation>::iterator it = collisionsA.begin(); 
		it != collisionsA.end(); ++it ){
		Point *p = (*it).point;
		Face *f = (*it).face;

		// Debug::log(string("Checking prunning condition"), LOOP);
		if( dot( p->v, sepPlane->normal ) > 0  ){
			Debug::log(string("Prunning condition satisfied"), LOOP);
			p->move = true;
			Debug::log(string("Erasing contact"), LOOP);
			toDel.push_back(it);
			Debug::log(string("Done"), LOOP);
		}
		else
			p->v = zeros<vec>(3);

	}

	cout << "Contacts to erase: " << toDel.size() << " of " << collisionsA.size() << endl;

	for( vector<CollisionInformation>::iterator it: toDel )
		collisionsA.erase(it);

	toDel.clear();

	for( vector<CollisionInformation>::iterator it = collisionsB.begin(); 
		it != collisionsB.end(); ++it ){
		Point *p = (*it).point;
		Face *f = (*it).face;

		// Debug::log(string("Checking prunning condition"), LOOP);
		if( dot( p->v, sepPlane->normal ) < 0  ){
			Debug::log(string("Prunning condition satisfied"), LOOP);
			p->move = true;
			Debug::log(string("Erasing contact"), LOOP);
			toDel.push_back(it);
			Debug::log(string("Done"), LOOP);
		}
		else
			p->v = zeros<vec>(3);

	}

	cout << "Contacts to erase: " << toDel.size() << " of " << collisionsB.size() << endl;

	for( vector<CollisionInformation>::iterator it: toDel )
		collisionsB.erase(it);

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

void RigidContact::prunePoints(){

}