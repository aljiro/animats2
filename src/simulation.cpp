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

RigidBody* Simulation::addRigidBody( int id, string type ){
	MeshProvider *mp;

	if( type == "plane" )
		mp = new PlaneMeshProvider();
	else
		return NULL;

	RigidBody* rb = new RigidBody( mp );
	this->rigidBodies.push_back( rb );
	return rb;
}


SoftBody* Simulation::addSoftBody( int id ){
	SoftBody *s = new SoftBody( new ObjMeshProvider("./mesh.obj") );
	this->softBodies.push_back( s );
	return s;
}

// Static
Simulation *load( char *name ){
	XMLLoader loader;

	try{
		Simulation *s = loader.load( name );
		return std::shared_ptr<Simulation *>(s);

	}catch(const std::exception& e ){
		return NULL;
	}
	
}

