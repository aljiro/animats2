#include "simulation.h"

using namespace morph::animats;

// Private
Simulation::Simulation( string workingDir ):running(false), pause(true), workingDir(workingDir){
	this->environment = new Environment( workingDir );
	this->solver = new Solver(this->environment, 0.01);
	this->collisionMgr = new CollisionManager( this->solver );
}


void Simulation::registerObjectsForCollision(){
    debugger.log(string("Registering rigid bodies"), GENERAL, "SIMULATION");
    collisionMgr->clear();
    
    for( RigidBody *rb : environment->getRigidBodies() ){        
        collisionMgr->registerObject(rb);
    }
    
    // debugger.log(string("Registering animats"), GENERAL, "SIMULATION");
    // for( SoftBody *sb: environment->getSoftBodies() ){        
    //     collisionMgr->registerObject( sb ); 
    // }
}

void Simulation::reset(){

	// for(  SoftBody *sb : softBodies ){
	// 	sb->reset();
	// }

	// for( RigidBody *rb : rigidBodies ){
	// 	rb->reset();
	// }
	environment->initShapes();
	registerObjectsForCollision();
	debugger.log(string("Setting up the views"), GENERAL, "SIMULATION");

	for( View *v : views)
		v->setup( *this );
}

void Simulation::setEnvironment( Environment* environment ){
	this->environment = environment;
}

void Simulation::close(){
	this->running = false;
}

void Simulation::togglePause(){
	this->pause = !this->pause;
}

void Simulation::run( int maxSteps ){
	debugger.log(string("Running the simulation"), GENERAL, "SIMULATION");
	
	this->running = true;
	int pass = 0;
	SoftBody* sb;

	while( this->running ){
		if( !this->pause ){
			sb = environment->generateBodies( step );
			if( sb != NULL ){
				collisionMgr->registerObject( sb ); 
			}
			// Finding candidate positions from the shape matching
			// Step 1
			solver->stepMaterial();
			// // Step 4
			// cout << "Processing proximities" << endl;
			// collisionMgr.processProximities( this->step );			
			// cout << "Normalizing interaction" << endl;
			// collisionMgr.normalizeInteractions();
			// cout << "Processing collisions" << endl;
			
			pass = 1;
			collisionMgr->findCollisions( this->step );
			cout << "Collisions first pass found: " << collisionMgr->collisions->count() << endl;
			// cin.get();
			while( collisionMgr->hasCollisions() ){

				while( collisionMgr->hasCollisions() )
					collisionMgr->resolveNext();

				collisionMgr->findCollisions( this->step );
				cout << "Collisions " << ++pass << " pass found: " << collisionMgr->collisions->count() << endl;
				// cin.get();
			}
			
			// collisionMgr.getContactList()->resolveForces();	
			// Step 5-6: compute final collisions and update velocities
			// Step 7 Update velocities of non colising points!
			solver->stepCollisions();
			
			this->step++;
		}

		this->notifyViews( string("Simulation step: ") + to_string(this->step) );
		// if( this->step > 20 ) cin.get();
		if( maxSteps > 0 && this->step > maxSteps )
			this->running = false;
	}
}

// Static
Simulation* Simulation::load( char *name ){
	XMLLoader loader;

	try{
		Simulation *s = new Simulation( string(name) );
		loader.load( s, name );
		debugger.log(string("Simulation loaded!"), GENERAL, "SIMULATION");
		return s;
	}catch(const std::exception& e ){
		return NULL;
	}
	
}

void Simulation::addView( View *view ){
	if( view != NULL )
		this->views.push_back( view );
}


void Simulation::notifyViews( string msg){
	for( View *view : this->views ){
		view->notify( *this, msg );
	}
}

Simulation::~Simulation(){
	
}
