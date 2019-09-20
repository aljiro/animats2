#include "../include/simulation.h"

using namespace morph::animats;

// Private
Simulation::Simulation( string workingDir ):running(false), workingDir(workingDir){
	forceChain = NULL;
}

// Public 
void Simulation::computeExternalForces(){
	Debug::log(string("Adding contact forces"), LOOP );
	collisionMgr.getContactList()->resolveForces();	

	if( this->forceChain == NULL )
		return;

	Debug::log(string("Resolving the force chain"), LOOP );

	for( GeometricObject *go : this->softBodies )
		this->forceChain->applyForce( go );

	for( GeometricObject *go : this->rigidBodies )
		this->forceChain->applyForce( go );
}

void Simulation::registerObjectsForCollision(){
    Debug::log(string("Registering rigid bodies"));
    collisionMgr.clear();
    
    for( RigidBody *rb : this->rigidBodies )
    {        
        collisionMgr.registerObject(rb);
    }
    
    Debug::log(string("Registering animats"));
    for( SoftBody *sb: this->softBodies ){        
        collisionMgr.registerObject( sb ); 
    }
}

void Simulation::addForce( ForceObject *fo ){
	if( this->forceChain == NULL )
		this->forceChain = fo;
	else
		this->forceChain->add( fo );
}
void Simulation::reset(){

	// for(  SoftBody *sb : softBodies ){
	// 	sb->reset();
	// }

	// for( RigidBody *rb : rigidBodies ){
	// 	rb->reset();
	// }

	initShapes();
	registerObjectsForCollision();
	Debug::log(string("Setting up the views"));	
	for( View *v : views)
		v->setup( *this );
}

vector<SoftBody *>& Simulation::getSoftBodies(){
	return this->softBodies;
}

vector<RigidBody *>& Simulation::getRigidBodies(){
	return this->rigidBodies;
}

void Simulation::close(){
	this->running = false;
}

void Simulation::run( int maxSteps ){
	Debug::log(string("Running the simulation"));
	Solver solver;
	this->running = true;

	while( this->running ){
		collisionMgr.findCollisions( this->step );
		collisionMgr.solveRegions( this->step );
		Debug::log(string("Solver step"), LOOP );		
		solver.step( *this );
		//collisionMgr.pruneContacts();
		Debug::log(string("Notifying views"), LOOP );
		this->notifyViews( string("Simulation step: ") + to_string(this->step) );
		this->step++;

		if( maxSteps > 0 && this->step > maxSteps )
			this->running = false;
	}
}

RigidBody* Simulation::addRigidBody( int id, string type ){
	Debug::log(string("Adding a new rigid body"));
	MeshProvider *mp;
	RigidBody* rb;

	if( type == "plane" ){
		mp = new PlaneMeshProvider();
		rb = new Plane( mp, Floor );
	}else{
		mp = new PlaneMeshProvider();
		rb = new RigidBody( mp );
	}

	
	this->rigidBodies.push_back( rb );
	return rb;
}

void Simulation::initShapes(){
	for(SoftBody *sb : softBodies){

		sb->initShape();
	}
}


SoftBody* Simulation::addSoftBody( int id ){
	Debug::log(string("Adding a new soft body"));
	string path = this->workingDir + string("/mesh.obj");
	SoftBody *s = new SoftBody( new ObjMeshProvider( path.c_str() ) );
	s->setId( id );
	this->softBodies.push_back( s );
	return s;
}

// Static
Simulation* Simulation::load( char *name ){
	XMLLoader loader;

	try{
		Simulation *s = new Simulation( string(name) );
		loader.load( s, name );
		Debug::log(string("Simulation loaded!"));
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
	for( RigidBody *rb : rigidBodies ){
		glDeleteBuffers(1, &(rb->VBO) );
    	glDeleteVertexArrays(1, &(rb->VAO) );
	}

	for( SoftBody *sb : softBodies ){
		glDeleteBuffers(1, &(sb->VBO) );
    	glDeleteVertexArrays(1, &(sb->VAO) );
	}
}
