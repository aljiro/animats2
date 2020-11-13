#include "environment.h"

using namespace morph::animats;

Environment::Environment( string workingDir ):workingDir(workingDir){
    forceChain = NULL;	
}

void Environment::addForce( ForceObject *fo ){
	if( this->forceChain == NULL )
		this->forceChain = fo;
	else
		this->forceChain->add( fo );
}

// Public 
void Environment::computeExternalForces(){
	debugger.log(string("Adding contact forces"), LOOP, "SIMULATION" );

	if( this->forceChain == NULL )
		return;

	debugger.log(string("Resolving the force chain"), LOOP, "SIMULATION" );

	for( GeometricObject *go : this->softBodies )
		this->forceChain->applyForce( go );

	for( GeometricObject *go : this->rigidBodies )
		this->forceChain->applyForce( go );
}

void Environment::initShapes(){
	for(SoftBody *sb : softBodies){
		sb->initShape();
	}
}

vector<SoftBody *>& Environment::getSoftBodies(){
	return this->softBodies;
}

vector<RigidBody *>& Environment::getRigidBodies(){
	return this->rigidBodies;
}

RigidBody* Environment::addRigidBody( int id, string type ){
	debugger.log(string("Adding a new rigid body"), GENERAL, "SIMULATION");
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


SoftBody* Environment::addSoftBody( int id ){
	debugger.log(string("Adding a new soft body"), GENERAL, "SIMULATION");
	string path = this->workingDir + string("/mesh.obj");
	SoftBody *s = new SoftBody( new ObjMeshProvider( path.c_str() ) );
	s->setId( id );
	this->sbQueue.push_back( s );
	return s;
}

SoftBody* Environment::generateBodies( int step ){
	if( sbQueue.empty() ) return NULL;

	SoftBody* sb = NULL;

	if( softBodies.empty() || step % 100 == 0 ){
		sb = sbQueue.front();
		sbQueue.erase(sbQueue.begin());
		sb->initShape();
		softBodies.push_back( sb );
		return sb;
	}

	return sb;
}

void Environment::computeRadius(){

}

Environment::~Environment(){
    for( RigidBody *rb : rigidBodies ){
		glDeleteBuffers(1, &(rb->VBO) );
    	glDeleteVertexArrays(1, &(rb->VAO) );
	}

	for( SoftBody *sb : softBodies ){
		glDeleteBuffers(1, &(sb->VBO) );
    	glDeleteVertexArrays(1, &(sb->VAO) );
	}
}