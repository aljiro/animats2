#include "cmgr.h"

using namespace morph::animats;

CollisionManager::CollisionManager( Solver* solver ):ht(5000,0.1){
	// this->contacts = new ContactList();
	this->collisions = new CollisionList();
	this->solver = solver;
}

void CollisionManager::clear(){
	// TO-DO: Memory management?
	indexes.clear();
	objects.clear();
	points.clear();
	faces.clear();
	// contacts->clear();
}

// ContactList *CollisionManager::getContactList(){
// 	return contacts;
// }

void CollisionManager::processProximities( int step ){
	// Hash the points      
	firstPass( step );
	// Scan the faces    
	secondPass( step, false );
}

void CollisionManager::findCollisions( int step ){
	// Hash the points      
	firstPass( step );
	// Scan the faces    
	secondPass( step, true );
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

void CollisionManager::secondPass( int step, bool collisions ){
	debugger.log(string("Finding collisions, second pass"), GENERAL, "COLLISION");

	for( int i = 0; i < this->faces.size(); i++ ){		
		CFace cf = this->faces[i];
		this->evaluateContacts( cf, step, collisions );
	}
}

void CollisionManager::evaluateContacts( CFace cf, int step, bool collisions ){
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
					this->handleCollisions( cf, chi, step, collisions );
				}
			}
		}
	}

}

void CollisionManager::handleCollisions( CFace cf, CHashItem chi, int step, bool collisions ){
	debugger.log(string("Handling collision"), LOOP, "COLLISION");

	for( list<int>::iterator it = chi.items.begin(); it != chi.items.end(); ++it ){
		CPoint cp = this->points[*it];
		Point *p = cp.point;		
		
		if( cp.go == cf.go ) continue;

		if( collisions )
			this->solveCollision( cf, cp );
		else
			this->solveProximity(cf, cp);
	}
}

bool CollisionManager::hasCollisions(){
	// cout << "Has collisions: " << this->collisions->isEmpty() << endl;
	return !this->collisions->isEmpty();
}


void CollisionManager::resolveNext(){
	
	Collision *c = this->collisions->pop();
	
	if( solver->h > 0.001 )
		solver->partialStep( c->hc*0 );
	
	// if( c->ctype == 0 )
	// 	cout << "p.vhalf: " << printvec(((FPCollision*)c)->p->v_half) << ", p.vhalfs: " << 
	// 		printvec(((FPCollision*)c)->f->points[0]->v_half) << ", " << 
	// 		printvec(((FPCollision*)c)->f->points[1]->v_half) << ", " <<
	// 		printvec(((FPCollision*)c)->f->points[2]->v_half) << endl;
	// else if( c->ctype == 1 )
	// 	cout << "e0.vhalfs: " << printvec(((EECollision*)c)->e1->v0->v_half) << ", " << printvec(((EECollision*)c)->e1->v1->v_half) <<
    //             ", ef.vhalfs: " << printvec(((EECollision*)c)->e2->v0->v_half) << ", " << printvec(((EECollision*)c)->e2->v1->v_half) << endl;

	c->resolve();
	// cout << "After: " << endl;

	// if( c->ctype == 0 )
	// 	cout << "p.vhalf: " << printvec(((FPCollision*)c)->p->v_half) << ", p.vhalfs: " << 
	// 		printvec(((FPCollision*)c)->f->points[0]->v_half) << ", " << 
	// 		printvec(((FPCollision*)c)->f->points[1]->v_half) << ", " <<
	// 		printvec(((FPCollision*)c)->f->points[2]->v_half) << endl;
	// else if( c->ctype == 1 )
	// 	cout << "e0.vhalfs: " << printvec(((EECollision*)c)->e1->v0->v_half) << ", " << printvec(((EECollision*)c)->e1->v1->v_half) <<
    //             ", ef.vhalfs: " << printvec(((EECollision*)c)->e2->v0->v_half) << ", " << printvec(((EECollision*)c)->e2->v1->v_half) << endl;
	// this->collisions->discount(c->hc);
	delete(c);
	
	// cin.get();
}

void CollisionManager::normalizeInteractions(){
	for( int i = 0; i < this->points.size(); i++ ){
		if( this->points[i].point->interactions > 0 ){
			this->points[i].point->v_half /= (float)this->points[i].point->interactions;
			this->points[i].point->interactions = 0;
		}
	}
}

void CollisionManager::solveCollision( CFace& cf, CPoint& cp ){
	int i, j;
	Point *p = cp.point;
	Face *f = cf.face;
	vec pd = {ht.discretize(p->x(0)), ht.discretize(p->x(1)), ht.discretize(p->x(2))};
	// 	Check for face-point collisions
	vec w;
	double hc;
	bool colliding = isColliding( f, p, &w, &hc, this->solver->h );
	// if( isContact( f, p ) && !colliding )
	// {
	// 	cout << "!!!!!! Contact not colliding " << endl;
		
	// }
	if( colliding ){
		//save
		// cout << "saving collision: " << collisions->count() << endl;		
		collisions->push( new FPCollision( hc, f, p, w ) );		
	}
	
	for( Edge *ep : p->edges ){
		for( Edge *ef : f->edges ){	
			colliding = isColliding( ep, ef, &hc, this->solver->h );
			// if( isContact(ep, ef) ){
			// 	cout << "Contact EE!!!!" << endl;
				
			// }
			if( colliding ){
				collisions->push( new EECollision( hc, ep, ef ));
			}
		} // For
	}
}

void CollisionManager::solveProximity( CFace& cf, CPoint& cp ){

	debugger.log(string("Solving proximities"), LOOP, "COLLISION");
	Face *f = cf.face;
	Point *p = cp.point;
	
	// Check proximity
	 if( isProximal( f, p ) ){

		vector<vec> p_imp = getInelasticImpulses( f, p, NULL );

		if( cf.go->type != PLANE ){
			for( int i = 0; i < 3; i++ ){
				f->points[i]->state = Proximal;
				f->points[i]->v_half += p_imp[i];
				f->points[i]->interactions++;
			}
		}

		if( cp.go->type != PLANE ){
			p->v_half += p_imp[3];
			p->state = Proximal;
			p->interactions++;
		}		
	}

	bool prox = false;
	for( Edge *ep : p->edges ){
		for( Edge *ef : f->edges ){	
			if( isProximal( ep, ef ) ){
				prox = true;
				// cout << "Getting impulses" << endl;
				vector<vec> p_imp = getInelasticImpulses( ep, ef );
				// for( vec v : p_imp)
				// 	cout << "Impulse: " << printvec(v) << endl;
				// cout << "Interactions: " << ep->v0->interactions << ", " << ep->v1->interactions << " ; " << ef->v0->interactions << ", " << ef->v1->interactions << endl;

				if( cp.go->type != PLANE ){
					ep->v0->state = Proximal;
					ep->v0->v_half += p_imp[0];
					ep->v0->interactions++;
					ep->v1->state = Proximal;
					ep->v1->v_half += p_imp[1];
					ep->v1->interactions++;
				}
				if( cf.go->type != PLANE ){
					ef->v0->state = Proximal;
					ef->v0->v_half += p_imp[2];
					ef->v0->interactions++;
					ef->v1->state = Proximal;
					ef->v1->v_half += p_imp[3];
					ef->v1->interactions++;
				}	
					
			}
		}//for
	}

	// if( prox ){ cout << "Finished adding impulses" << endl;
	// 	cin.get();}			
}


CollisionManager::~CollisionManager(){

}