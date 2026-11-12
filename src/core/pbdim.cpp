#include <armadillo>
#include "point.h"
#include "pbdim.h"
#include "../util/config.h"

using namespace morph::softmats;

PBD::PBD(){
    h = Config::getConfig()->getTimeStep();
}

void PBD::generateConstraints(BodySet *bs, int step ){
    for( Body *b : bs->getBodies() ){
        for( Constraint *c : b->getConstraints() ){
            c->generate( step );
        }
    }

    for( Constraint *c : bs->getConstraints() ){
        c->generate( step );
    }
}

void PBD::projectConstraints( BodySet *bs ){ 
    for( Body *b : bs->getBodies() ){
        for( Constraint *c : b->getConstraints() ){
            c->solve();
        }
    }

    // The collisions go last so that no penetration is left by the other constraints
    for( Constraint *c : bs->getConstraints() ){
        c->solve();
    }
}

void PBD::velocityUpdate( BodySet *bs ){
    for( Body *b : bs->getBodies() ){
        for( Constraint *c : b->getConstraints() ){
            c->updateVelocity();
        }
    }

    for( Constraint *c : bs->getConstraints() ){
        c->updateVelocity();
    }
}

void PBD::loop( BodySet *bs, int step ){
    Config *config = Config::getConfig();

    // Candidate positions from the external forces alone
    for( Body* b : bs->getBodies() ){
        if( b->type == BodyType::GROUND ) continue;

        for( Point* q : b->getMesh()->getVertices() ){
            // The dynamics work on the positions without the dents of the last step
            q->x -= q->dent;
            q->dent.zeros();

            if( q->lock ){
                q->v.zeros();
                q->x_c = q->x;
            }else{
                q->v += h*q->w*q->fext;
                q->x_c = q->x + h*q->v; 
            }
        }
    }

    generateConstraints( bs, step );

    // The constraints are solved together: each iteration brings the candidate
    // positions closer to satisfying all of them at once
    for( int i = 0; i < config->getNumIterations(); ++i )
        projectConstraints( bs );

    // The velocity is whatever takes the points to the corrected positions
    const double keep = std::max( 0.0, 1.0 - h*config->getDamping() );

    for( Body* b : bs->getBodies() ){  
        if( b->type == BodyType::GROUND ) continue;
             
        for( Point* q : b->getMesh()->getVertices() ){
            if( q->lock ) continue;

            q->v = keep*(q->x_c - q->x)/h;                
            q->x = q->x_c; 
        }
    }

    velocityUpdate( bs );

    // The surface is where the dynamics left it plus the dents found by the collisions
    for( Body* b : bs->getBodies() ){  
        if( b->type == BodyType::GROUND ) continue;

        for( Point* q : b->getMesh()->getVertices() )
            q->x += q->dent;

        b->getMesh()->computeNormals( false );
    }
}
