#include "deformable_model.h"

using namespace morph::animats;

#include "deformable_model.h"

using namespace std;

DeformableModel::DeformableModel( double alpha ){
    this->alpha = alpha;
    this->m = 1.0;    
}

void DeformableModel::init( vector<Point *>& points ){
    for( Point *pt : points )
        pt->x0 = pt->x;
        
    this->precompute( points );
}


arma::vec DeformableModel::computeCM( vector<Point *>& points ){
    int n = points.size();
    arma::vec cm = arma::zeros<arma::vec>(3);

    for( Point *pt : points ){
        cm += pt->x;
    }

    return cm/(float)n;
}

// Simple Model ----------------------
SimpleDeformableModel::SimpleDeformableModel( double alpha ):DeformableModel(alpha){

}

void SimpleDeformableModel::precompute( vector<Point *>& points ){
    this->x0_cm = this->computeCM( points ); 
}


void SimpleDeformableModel::computeMatrices( vector<Point *>& points, arma::vec x_cm ){
    arma::vec p, q;
    arma::cx_mat S;
    int n = 3;
    arma::mat Apq( n, n, arma::fill::zeros );

    for( Point *pt : points ){        
        p = pt->x - x_cm;
        q = pt->x0 - this->x0_cm;
        Apq += this->m*p*q.t();
    }

    S = arma::sqrtmat(Apq.t()*Apq);
    this->T = arma::real(Apq*S.i());
}

void SimpleDeformableModel::computeGoals( vector<Point *>& points ){
    arma::vec x_cm = this->computeCM( points );
    this->computeMatrices( points, x_cm );

    for( Point *p : points ){
        p->g = this->T*(p->x0 - this->x0_cm) + x_cm;
    }
}

// Linear model --------------------------
LinearDeformableModel::LinearDeformableModel( double alpha ):DeformableModel(alpha){
    this->beta = 0.0;
}

void LinearDeformableModel::precompute( vector<Point *>& points ){
    arma::vec q;
    this->x0_cm = this->computeCM( points ); 
    this->Aqqi = arma::zeros<arma::mat>(3, 3);

    for( Point *pt : points ){
        q = pt->x0 - this->x0_cm;
        this->Aqqi += this->m*q*q.t();
    }

    this->Aqqi = arma::inv(this->Aqqi);
}


void LinearDeformableModel::computeMatrices( vector<Point *>& points, arma::vec x_cm ){
    arma::vec p, q;
    arma::cx_mat S;
    arma::mat Apq( 3, 3, arma::fill::zeros );

    for( Point *pt : points ){        
        p = pt->x - x_cm;
        q = pt->x0 - this->x0_cm;
        Apq += this->m*p*q.t();
    }

    S = arma::sqrtmat(Apq.t()*Apq);
    arma::mat R = arma::real(Apq*S.i());
    arma::mat A = Apq*this->Aqqi;
    // A = A/cbrtf(arma::det(A));
    
    this->T = this->beta*A + (1 - this->beta)*R;
    // this->T = this->T/cbrtf(arma::det(this->T));
}

void LinearDeformableModel::computeGoals( vector<Point *>& points ){
    arma::vec x_cm = this->computeCM( points );
    this->computeMatrices( points, x_cm );

    for( Point *p : points ){
        p->g = this->T*(p->x0 - this->x0_cm) + x_cm;
    }
}

