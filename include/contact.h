#ifndef CONTACT_H
#define CONTACT_H 

#include "gobject.h"
#include <vector>
#include <iostream>

using namespace std;

namespace morph{ namespace animats{

class ContactList;

typedef struct {
	double depth;
	vec faceProjection;
	Point *point;
	Face *face;
	GeometricObject *goPoint;
	GeometricObject *goFace;
	int pindex;
} CollisionInformation;

class Contact{
protected:
	
	
public:
	GeometricObject *A;
	GeometricObject *B;

	explicit Contact(  GeometricObject* A, GeometricObject* B  );

	virtual void addCollision( CollisionInformation ci ) = 0;
	virtual void resolve() = 0;
	virtual bool prune() = 0;
	~Contact();

	friend ContactList;
};


/**
 * The Signorini contact consist of a SoftBody A and a RigidBody B
 */
class SignoriniContact: public Contact{

private:
	
public:
	vector<CollisionInformation> collisions;
	
	SignoriniContact( SoftBody *sb, RigidBody *rb );
	void addCollision( CollisionInformation ci );
	void resolve();
	bool prune();
};

class DeformableContact: public Contact{
	private:
	vector<CollisionInformation> collisionsA;
	vector<CollisionInformation> collisionsB;
	vector<double> u_A;
	vector<double> u_B;
	double delta;
	Face *sepPlane;
	bool react;
public:
	DeformableContact( SoftBody *A, SoftBody *B);
	void addCollision( CollisionInformation ci );
	void resolve();
	bool prune();
};

class RigidContact: public Contact{
public:
	RigidContact( RigidBody *A, RigidBody *B );
	void addCollision( CollisionInformation ci );
	void resolve();
	bool prune();
};

/**
 * ContactList - Stores the list of all contacts in the simulation
 */
class ContactList{
private:
	vector<Contact *> contacts;
public:
	ContactList();

	Contact *findContact( GeometricObject *A, GeometricObject *B );
	Contact *addNewContact( GeometricObject *A, GeometricObject *B );
	void add( GeometricObject *A, GeometricObject *B, CollisionInformation ci );
	void clear();
	vector<Contact *>& getContacts();
	void resolveForces();
	void pruneContacts();
};

}}

#endif