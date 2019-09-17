#include "../include/collision.h"

using namespace morph::animats;

CollisionManager::CollisionManager():ht(5000,0.2){
	this->contacts = new ContactList();
}

void CollisionManager::clear(){
	// TO-DO: Memory management?
	indexes.clear();
	objects.clear();
	points.clear();
	faces.clear();

	contacts->clear();
}

ContactList *CollisionManager::getContactList(){
	return contacts;
}

void CollisionManager::findCollisions( int step ){
	// Hash the points      
	firstPass( step );
	// Scan the faces    
	secondPass( step );
}

void CollisionManager::solveRegions( int step ){
	Debug::log(string("Solving contact solveRegions"), LOOP);
	for( Contact *c : contacts->getContacts() )
		c->solveContactRegion();

}

void CollisionManager::registerObject(GeometricObject *go){

	vector<int> objIdxs;
	Debug::log(string("Registering objects for collision detection"));
	vector<Point*> gPoints = go->getPoints();
	vector<Face*> gFaces = go->getFaces();
	// Put all the pointers to points in the corresponding vector
	for( int i = 0; i < gPoints.size(); i++ ){
		
		objIdxs.push_back( this->points.size() );

		CPoint cp;
		cp.point = gPoints[i];
		cp.go = go;
		cp.originalIdx = i;
		this->points.push_back( cp );
	}

	// cout << this->points.size() <<  " points registered. "<<endl;
	Debug::log(string("Registering faces"));
	
	for( int i = 0; i < go->getFaces().size(); i++ ){
		CFace cf;
		cf.face = gFaces[i];
		cf.go = go;
		cf.aabb = new Box();
		cf.originalIdx = i;
		this->faces.push_back( cf );
	}

	// cout << this->faces.size() << " faces registered " << endl;
	this->indexes[go] = objIdxs;
	this->objects[go] = this->objects.size();
}

void CollisionManager::firstPass( int step ){
	Debug::log(string("Finding collisions, first pass"), LOOP);
	for( int i = 0; i < this->points.size(); i++ ){
		// Add hash
		CPoint cp = this->points[i];
		Point *p = cp.point;
		vector<int> idxs = indexes[cp.go]; 
		this->ht.hashIn( cp.point->x, i, step );
	}

	// Compute the bounding boxes
	for( CFace cf : this->faces ){
		Box::compute( cf.face->points, cf.aabb );
	}
}

void CollisionManager::secondPass( int step ){
	Debug::log(string("Finding collisions, second pass"), LOOP);

	for( int i = 0; i < this->faces.size(); i++ ){		
		CFace cf = this->faces[i];
		this->evaluateContacts( cf, step );
	}
}

void CollisionManager::evaluateContacts( CFace cf, int step ){
	unsigned int h;
	// Discretizing the AABB	
	this->ht.discretizeBox(cf.aabb);	

	// Hashing the cells
	for( int kx = (int)cf.aabb->min(0); kx <= cf.aabb->max(0); kx++ ){
		for( int ky = (int)cf.aabb->min(1); ky <= cf.aabb->max(1); ky++ ){
			for( int kz = (int)cf.aabb->min(2); kz <= cf.aabb->max(2); kz++ )
			{
				vec p = {kx, ky, kz};				
				h = ht.getHashDiscrete( p );
				CHashItem chi = ht.getItem( h );

				if( chi.timestamp == step ){					
					this->handleCollisions( cf, chi, step );
				}
			}
		}
	}
}

void CollisionManager::handleCollisions( CFace cf, CHashItem chi, int step ){
	Debug::log(string("Handling collision"), LOOP);

	for( list<int>::iterator it = chi.items.begin(); it != chi.items.end(); ++it ){

		CPoint cp = this->points[*it];
		Point *p = cp.point;		
		
		if( cp.go == cf.go ) continue;
		if( p->pre == NULL ) continue;

		this->storeCollision( cf, cp );
	}
}

void CollisionManager::storeCollision( CFace& cf, CPoint& cp ){
	int i, j;

	Debug::log(string("Storing collision"), LOOP);

	Point *p = cp.point;
	Edge e( p->pre, p );
	vec pd = {ht.discretize(p->x(0)), ht.discretize(p->x(1)), ht.discretize(p->x(2))};
	
	// if( p->pre->x(1)*p->x(1) < 0 ){
	// 	cout << "Pre:" <<printvec(p->pre->x) << ", pos: " << printvec(p->x) <<"Is penetrated: " << (cf.face->isPenetrated( e ) || 
	// 	 cf.face->penetrates(p)) << ", Is inside: " <<
	// 	 (cf.face->isInside(cf.face->getFaceProjection(e)))  <<endl;
	// }

	if( (cf.face->isPenetrated( e ) || 
		 cf.face->penetrates(p)) && 
		 cf.face->isInside(cf.face->getFaceProjection(e)) ){
	// Point p penetrates the face cf.face
		i = this->objects[ cp.go ];
		j = this->objects[ cf.go ];

		CollisionInformation ci;
		ci.point = cp.point;
		ci.face = cf.face;
		ci.goPoint = cp.go;
		ci.goFace = cf.go;

		Debug::log(string("Adding contact"), LOOP);
		this->contacts->add( cp.go, cf.go, ci );
	}
}


void CollisionManager::pruneContacts( ){
	Debug::log(string("Prunning contacts"), LOOP);
	for( Contact *c : contacts->getContacts() ){
		c->prunePoints();
	}	
}


CollisionManager::~CollisionManager(){

}

// CHashTable
CHashTable::CHashTable( int n, double l ):hashes(n){
	this->n = n;
	this->l = l;
	this->hashes.reserve(n);
}

int CHashTable::discretize( double a ){
	int d = floor((a)/this->l);
	return d;
}

unsigned int CHashTable::getHash( vec point ){
	
	int x = discretize(point(0));
	int y = discretize(point(1));
	int z = discretize(point(2));
	vec p = {x, y, z};
	return this->getHashDiscrete(p);
	
}

unsigned int CHashTable::getHashDiscrete( vec p ){
	unsigned long h;
	unsigned long x = p(0);
	unsigned long y = p(1);
	unsigned long z = p(2);

	unsigned long t1 = (x*this->p1);
	unsigned long t2 = (y*this->p2) ;
	unsigned long t3 = (z*this->p3);

	h = (t1 ^ t2 ^ t3)%((unsigned long)this->n);

	return (unsigned int)h;
}

void CHashTable::hashIn( vec point, int index, int step ){
	unsigned int h = getHash( point );

	if( this->hashes[h].timestamp != step ){
		this->hashes[h].items.clear();
		this->hashes[h].timestamp = step;
	}

	this->hashes[h].items.insert( this->hashes[h].items.end(), index );
}

void CHashTable::discretizeBox( Box *b ){
	//cout << "Discretizing box - min: " << printvec(b->min) << ", max: "<<printvec(b->max)<<endl;
	b->min(0) = discretize(b->min(0));
	b->min(1) = discretize(b->min(1));
	b->min(2) = discretize(b->min(2));

	b->max(0) = discretize(b->max(0));
	b->max(1) = discretize(b->max(1));
	b->max(2) = discretize(b->max(2));
}

CHashItem CHashTable::getItem( unsigned int h ){
	return hashes[h];
}

