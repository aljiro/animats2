#include "shapeconstr.h"
#include "../util/config.h"

using namespace morph::softmats;

arma::vec ShapeMatchingContraint::computeCM(){
    arma::vec cm = arma::zeros<arma::vec>(3);
    double msum = 0;
    std::vector<Point *>& vert = body->getMesh()->getVertices(); 

    for( unsigned int i = 0; i < vert.size(); ++i ){
        cm += mass[i]*vert[i]->x_c;
        msum += mass[i];
    }    

    return cm/msum;
}

void ShapeMatchingContraint::precompute( ){
    this->Aqqi = arma::zeros<arma::mat>(3, 3);

    for( unsigned int i = 0; i < shape.size(); ++i )
        this->Aqqi += mass[i]*shape[i]*shape[i].t();

    this->Aqqi = arma::pinv(this->Aqqi);
}

void ShapeMatchingContraint::init( Body *b ){
    this->body = b;       
    shape.clear();
    mass.clear();

    this->x0_cm = arma::zeros<arma::vec>(3);
    double msum = 0;

    for( Point* p : b->getMesh()->getVertices() ){
        // Locked points still take part in the shape, with their own mass
        const double m = p->w > 0 ? 1.0/p->w : 0.0;
        mass.push_back( m );
        this->x0_cm += m*p->x;
        msum += m;
    }

    this->x0_cm /= msum;

    for( Point* p : b->getMesh()->getVertices() )
        shape.push_back( p->x - this->x0_cm );

    this->T = arma::eye<arma::mat>(3, 3);
    this->precompute();
}

void ShapeMatchingContraint::computeMatrices( const arma::vec& x_cm ){
    arma::mat Apq( 3, 3, arma::fill::zeros );
    std::vector<Point *>& vert = body->getMesh()->getVertices(); 
    
    for( unsigned int i = 0; i < vert.size(); ++i )
        Apq += mass[i]*(vert[i]->x_c - x_cm)*shape[i].t();

    // Rotation part of Apq through its singular value decomposition, which
    // stays well defined when the body is flattened or inverted
    arma::mat U, V;
    arma::vec s;

    if( !arma::svd( U, s, V, Apq ) )
        return; // Keep the previous transformation

    arma::mat R = U*V.t();

    if( arma::det(R) < 0 ){
        U.col(2) *= -1.0;
        R = U*V.t();
    }

    if( this->beta > 0 ){
        arma::mat A = Apq*this->Aqqi;
        const double volume = arma::det(A);

        // Preserve the volume of the linear deformation
        if( volume > 1e-12 )
            A /= std::cbrt( volume );
        else
            A = R;

        this->T = this->beta*A + (1 - this->beta)*R;
    }else
        this->T = R;
}

void ShapeMatchingContraint::generate( int step ){
// Do nothing
}

void ShapeMatchingContraint::reset(){
// ?
}

void ShapeMatchingContraint::solve(){
    const arma::vec x_cm = this->computeCM();
    this->computeMatrices( x_cm );
    std::vector<Point *>& vert = body->getMesh()->getVertices(); 

    // The stiffness is defined per time step, so that the behaviour does
    // not depend on the number of solver iterations
    const int iterations = Config::getConfig()->getNumIterations();
    const double k = 1.0 - std::pow( 1.0 - alpha, 1.0/iterations );

    for( unsigned int i = 0; i < vert.size(); ++i ){        
        if( vert[i]->lock ) continue;

        const arma::vec g = this->T*shape[i] + x_cm;
        vert[i]->x_c += k*(g - vert[i]->x_c);
    }
}

ShapeMatchingContraint::ShapeMatchingContraint( double alpha ){
    this->alpha = std::min( 1.0, std::max( 0.0, alpha ) );
    this->beta = 0.0;
}

void ShapeMatchingContraint::updateVelocity(){
    // nothing
}

ShapeMatchingContraint::~ShapeMatchingContraint(){

}
