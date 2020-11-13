#ifndef DEFORMABLE_MODEL_H
#define DEFORMABLE_MODEL_H

#include "point.h"
#include "../utilities/util.h"
#include <armadillo>
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

using namespace arma;
using namespace std;

namespace morph{ namespace animats{

class DeformableModel{
protected:
    
    double m;
    arma::vec x0_cm;
    arma::mat T;
public:
    double alpha;

    DeformableModel( double alpha );
    void init( vector<Point *>& points ); 
    arma::vec computeCM( vector<Point *>& points );
    // Interface
    virtual void computeMatrices( vector<Point *>& points, arma::vec x_cm ) = 0;
    virtual void precompute( vector<Point *>& points ) = 0;
    virtual void computeGoals( vector<Point *>& points ) = 0; 
};

class SimpleDeformableModel:public DeformableModel{
private:   

public:    
    SimpleDeformableModel( double alpha );  
    void computeMatrices( vector<Point *>& points, arma::vec x_cm );
    void precompute( vector<Point *>& points );
    void computeGoals( vector<Point *>& points );    
};

class LinearDeformableModel:public DeformableModel{
private:   
    arma::mat Aqqi;
    double beta;
public:    

    LinearDeformableModel( double alpha );  
    void computeMatrices( vector<Point *>& points, arma::vec x_cm );
    void precompute( vector<Point *>& points );
    void computeGoals( vector<Point *>& points );    
};


} // namespace animats
} // namespace morph

#endif