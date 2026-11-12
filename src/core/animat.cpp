#include <armadillo>
#include <glm/glm.hpp>
#include <cmath>
#include "animat.h"
#include "shapeconstr.h"
#include "../util/meshutil.h"
#include "../util/openglutils.h"
#include "../util/config.h"

using namespace morph::softmats;

Animat::Animat( float x, float y, float z ){
    init(16);
    type = BodyType::ANIMAT;
    mesh->translate(x, y, z);

    for( Point *p : mesh->getVertices() )
        p->x_c = p->x;

    mesh->computeNormals( false );
    mesh->updateVertexNormals();
    material.matAmb = OpenglUtils::goldAmbient();
    material.matDif = OpenglUtils::goldDiffuse();
    material.matSpe = OpenglUtils::goldSpecular();
    material.matShi = OpenglUtils::goldShininess();
}

void Animat::setConstraints( double stiffness ){
    ShapeMatchingContraint *smc = new ShapeMatchingContraint( stiffness );
    this->addShapeConstraint( smc );
}

void Animat::setMass( double m ){
    double pm = (double)m/mesh->getNumVertices();

    for( Point *p : mesh->getVertices() )
        p->w = 1/pm;
}

void Animat::init( int prec ){
    // SphereMeshProvider mp( SphereMeshProvider::TYPICAL );
    ObjMeshProvider mp( Config::getConfig()->getMeshLocation().c_str() );
    mesh = mp.buildMesh();
    // The collisions rely on the normals pointing out of the body
    mesh->orientOutwards();
}

void Animat::addGroundImpulse( arma::vec f ){
    for( Point* p : getMesh()->getVertices() ){
        if( p->ground_receptor )
            p->fext += f;
    }
}

void Animat::move(  double x, double y, double z ){
    arma::vec dir = {x, y, z};
    dir /= norm(dir);
    // The force is shared among the vertices, so that it does not depend on the mesh
    const double force = 420.0;
    addGroundImpulse( (force/mesh->getNumVertices())*dir );
}
