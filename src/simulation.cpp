#include "simulation.h"

// Private


// Public 

void simulation::computeExternalForces(){
	for( vector<Contact *>::iterator it = this->contacts.begin(); 
		 it != contacts.end(); ++it){
		(*it).resolve();
	}

	this->forceChain->apply( this );
}

void Simulation::reset(){

	for(  SoftBody *sb : softBodies ){
		sb->reset();
	}

	for( RigidBody *rb : rigidBodies ){
		rb->reset();
	}

}

void Simulation::run(){
	this->reset();
	Solver solver;

	while( this->running ){
		this->contacts = collisionMgr.findCollisions();

		solver.step( *this );
		this->notifyViews( *this, "Simulation step" );

		collisionManager.pruneContacts( this->contacts );
		this->step++;
	}
}

// Static
Simulation *load( char *name ){
	XMLLoader loader;

	try{
		Simulation *s = loader.load(name);
		return std::shared_ptr<Simulation *>(s);

	}catch(const std::exception& e ){

	}
	
}

