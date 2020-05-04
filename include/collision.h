#ifndef COLLISION_H
#define COLLISION_H 

#include <vector>
#include <iostream>
#include <armadillo>
#include <map>
#include <list>
#include <time.h>

#include "box.h"
#include "contact.h"
#include "gobject.h"
#include "face.h"
#include "util.h"
#include "chashtable.h"

using namespace std;
using namespace arma;

namespace morph{ namespace animats{

typedef struct {
	Point* point;
	GeometricObject* go;
	int originalIdx;
} CPoint;

typedef struct {
	Face* face;
	GeometricObject* go;
	Box *aabb;
	int originalIdx;
} CFace;

/**
 CollisionManager - Manages the collisions in space.
 */

class CollisionManager{
private:
	vector<CPoint> points;
	vector<CFace> faces;
	map<GeometricObject*, vector<int>> indexes;
	map<GeometricObject*, int> objects;
	// Hash tables
	CHashTable ht;
	// Contacts
	ContactList *contacts;

	// Methods
	void handleCollisions( CFace cf, CHashItem chi, int step );
	void storeCollision( CFace& cf, CPoint& cp );
	void evaluateContacts( CFace cf, int step );
	void firstPass( int step ); // Hashes points and computes aabb
	void secondPass( int step ); // Check the faces and handles collisions
public:
	explicit CollisionManager();

	void clear();
	void registerObject( GeometricObject *go );
	void findCollisions( int step );
	void pruneContacts();
	void solveRegions( int step );

	ContactList *getContactList();

	~CollisionManager();
};

}}
#endif