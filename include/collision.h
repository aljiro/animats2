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

using namespace std;
using namespace arma;

namespace morph{ namespace animats{



typedef struct {
	list<int> items;
	int timestamp;
} CHashItem;

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


class CHashTable{

	const unsigned long p1 = 73856093;
	const unsigned long p2 = 19349663;
	const unsigned long p3 = 83492791;
public:
	int n;
	double l; // Cell size
	CHashTable( int n, double l );
	vector<CHashItem> hashes;

	int discretize( double a );
	void discretizeBox( Box *b );
	unsigned int getHash( vec point );
	unsigned int getHashDiscrete( vec p );
	void hashIn( vec point, int index, int step ); 
	CHashItem getItem( unsigned int h );

};

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