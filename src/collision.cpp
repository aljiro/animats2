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
	debugger.log(string("Solving contact solveRegions"), LOOP);
	// for( Contact *c : contacts->getContacts() )
	// 	c->solveContactRegion();
}

void CollisionManager::registerObject(GeometricObject *go){

	vector<int> objIdxs;
	debugger.log(string("Registering objects for collision detection"), LOOP, "COLLISION");
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

	debugger.log(string("Registering faces"), LOOP, "COLLISION");
	
	for( int i = 0; i < go->getFaces().size(); i++ ){
		CFace cf;
		cf.face = gFaces[i];
		cf.go = go;
		cf.aabb = new Box();
		cf.originalIdx = i;
		this->faces.push_back( cf );
	}

	this->indexes[go] = objIdxs;
	this->objects[go] = this->objects.size();
}

void CollisionManager::firstPass( int step ){
	debugger.log(string("Finding collisions, first pass"), LOOP, "COLLISION");

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
	debugger.log(string("Finding collisions, second pass"), GENERAL, "COLLISION");

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
	debugger.log(string("Handling collision"), LOOP, "COLLISION");

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

	debugger.log(string("Storing collision"), LOOP, "COLLISION");

	Point *p = cp.point;
	Edge e( p->pre, p );
	vec pd = {ht.discretize(p->x(0)), ht.discretize(p->x(1)), ht.discretize(p->x(2))};
	
	if( (cf.face->isPenetrated( e ) || 
		 cf.face->penetrates(p)) && 
		 cf.face->isInsideProjection(e) ){
	// Point p penetrates the face cf.face
		i = this->objects[ cp.go ];
		j = this->objects[ cf.go ];

		CollisionInformation ci;
		ci.point = cp.point;
		ci.face = cf.face;
		ci.goPoint = cp.go;
		ci.goFace = cf.go;

		debugger.log(string("Adding contact"), LOOP, "COLLISION");
		// Changing point state
		cp.point->state = Invalid;
		// Adding to the contact list
		this->contacts->add( cp.go, cf.go, ci );
	}
}


void CollisionManager::pruneContacts( ){
	// debugger.log(string("Prunning contacts"), LOOP, "COLLISION");
	this->contacts->pruneContacts();
}


CollisionManager::~CollisionManager(){

}