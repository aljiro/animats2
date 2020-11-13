#ifndef CMGR_H
#define CMGR_H 

#include <vector>
#include <iostream>
#include <armadillo>
#include <map>
#include <list>
#include <time.h>

#include "box.h"
// #include "contact.h"
#include "chashtable.h"
#include "collision.h"
#include "../core/solver.h"
#include "../geometry/gobject.h"
#include "../geometry/face.h"
#include "../utilities/util.h"

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
	Solver* solver;
	

	// Methods
	void handleCollisions( CFace cf, CHashItem chi, int step, bool collisions );
	void solveCollision( CFace& cf, CPoint& cp );
	void solveProximity( CFace& cf, CPoint& cp );
	void evaluateContacts( CFace cf, int step, bool collisions );	
	// Hashes points and computes aabb
	void firstPass( int step ); 
	// Check the faces and handles collisions
	void secondPass( int step, bool collisions = false ); 
	void updatePointSpeed( Point *p, vec imp );
public:
// ContactList *contacts;
	CollisionList *collisions;
	explicit CollisionManager( Solver* solver );

	void clear();
	void normalizeInteractions();
	void registerObject( GeometricObject *go );
	void processProximities( int step );
	void findCollisions( int step );
	bool hasCollisions();
	void resolveNext( );
	void solveRegions( int step );
	
	// ContactList *getContactList();

	~CollisionManager();
};

}}
#endif //CMGR_H
