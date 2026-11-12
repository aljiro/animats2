#include "body.h"
#include "shapeconstr.h"
#include "../util/config.h"

using namespace morph::softmats;

Body::Body(){
    fext = {0.0, 0.0, 0.0};
    mesh = nullptr;
    id = -1;
    stiffness = Config::getConfig()->getStiffness();
}

void Body::setStiffness( double stiffness ){
    this->stiffness = std::max( 0.0, stiffness );
}

double Body::getStiffness(){
    return stiffness;
}

void Body::addShapeConstraint( Constraint *c ){
    if( c != NULL ){
        ((ShapeMatchingContraint *)c)->init( this );
        constraints.push_back( c );
    }
}

std::vector<Constraint *>& Body::getConstraints(){
    return constraints;
}

TriangleMesh* Body::getMesh(){
    return this->mesh;
}

void Body::setMesh( TriangleMesh *mesh ){
    this->mesh = mesh;
}

void Body::setExternalForce( arma::vec f ){
    this->fext = f;
}

void Body::updateReceptors(){
    for( Point* p : getMesh()->getVertices() ){
        p->ground_receptor = false;
    }
}

int Body::getId(){
    return id;
}

void Body::setId( int id ){
    this->id = id;
}

void Body::resetForces(){
    
    for( Point* p : getMesh()->getVertices() ){

        if( p->w > 0 )
            p->fext = this->fext/p->w;
        else
            p->fext = {0.0, 0.0, 0.0};
    }
}

Body::~Body(){
    delete mesh;
    
    for( Constraint *c : constraints )
        delete c;
}