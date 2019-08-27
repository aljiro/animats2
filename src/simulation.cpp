#include "simulation.h"

// Public 
void simulation::computeExternalForces(){
	for( vector<Contact *>::iterator it = this->contacts.begin(); 
		 it != contacts.end(); ++it){
		(*it).resolve();
	}

	for( GeometricObject *go : this->softBodies )
		this->forceChain->apply( go );

	for( GeometricObject *go : this->rigidBodies )
		this->forceChain->apply( go );
}

void Simulation::reset(){

	for(  SoftBody *sb : softBodies ){
		sb->reset();
	}

	for( RigidBody *rb : rigidBodies ){
		rb->reset();
	}
}

vector<SoftBody *>& Simulation::getSoftBodies(){
	return this->softBodies;
}

vector<RigidBody *>& Simulation::getRigidBodies(){
	return this->rigidBodies;
}

void Simulation::run( int maxSteps = -1 ){
	this->reset();
	Solver solver;
	this->running = true;

	while( this->running ){
		this->contacts = collisionMgr.findCollisions();

		solver.step( *this );
		collisionManager.pruneContacts( this->contacts );

		this->notifyViews( *this, "Simulation step" );
		this->step++;

		if( maxSteps > 0 && this->step > maxSteps )
			this->running = false;
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

void Simulation::addView( View *view ){
	if( view != NULL )
		this->views.push_back( view );
}


void Simulation::notifyViews(){
	for( View *view : this->views ){
		view->notify( this );
	}
}

