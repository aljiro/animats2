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
	GeometricObject *A;
	GeometricObject *B;
	
public:

	explicit Contact(  GeometricObject* A, GeometricObject* B  );

	virtual void addCollision( CollisionInformation ci ) = 0;
	virtual void prunePoints() = 0;
	virtual void solveContactRegion() = 0;
	virtual void resolve() = 0;
	virtual bool isResting() = 0;
	~Contact();

	friend ContactList;
};

class SignoriniContact: public Contact{
/*
 * The Signorini contact consist of a SoftBody A and a RigidBody B
 * The solution of the collision region is perform by projection onto the given face
 */
private:
	vector<CollisionInformation> collisions;
public:
	SignoriniContact( SoftBody *sb, RigidBody *rb );
	void addCollision( CollisionInformation ci );
	void prunePoints();
	void solveContactRegion();
	void resolve();
	bool isResting();
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
	// void updateDisplacements();
	// bool evaluateRegion();
	void addCollision( CollisionInformation ci );
	void prunePoints();
	void computeSeparatingPlane();
	void pruneCollisionList( vector<CollisionInformation>& collisions, GeometricObject *go );
	bool isContactRegionValid();
	void solveContactRegion();
	void resolve();
	bool isResting();
};

class RigidContact: public Contact{
public:
	RigidContact( RigidBody *A, RigidBody *B );
	void addCollision( CollisionInformation ci );
	void prunePoints();
	void solveContactRegion();
	void resolve();
	bool isResting();

};

class ContactList{
private:
	vector<Contact *> contacts;
public:
	ContactList();

	Contact *findContact( GeometricObject *A, GeometricObject *B );
	Contact *addNewContact( GeometricObject *A, GeometricObject *B );
	void add( GeometricObject *A, GeometricObject *B, CollisionInformation ci );
	void clear();
	vector<Contact *> getContacts();
	void resolveForces();
};

}}

#endif