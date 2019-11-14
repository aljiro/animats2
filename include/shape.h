#ifndef SHAPE_H
#define SHAPE_H

#include "point.h"
#include "util.h"
#include <armadillo>
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

using namespace arma;
using namespace std;

namespace morph{ namespace animats{

/*
 * Transforms
 */

class MatchTransform{
protected:
    mat Aqq;
    double beta;

	virtual vec dataTransformation( vec x ) = 0;
    void precompute( const vector<Point *>& points );
    mat getApq( vector<Point *>& shapePoints, vector<Point *>& points, vec (*f)(vec) );
    mat getR( const mat& Apq );

public:
    explicit MatchTransform( const vector<Point *>& points, double beta );
    virtual mat getTransform( vector<Point *>& shapePoints, vector<Point *>& points ) = 0;
};

// Main subclasses

class LinearMatchTransform : public MatchTransform{
protected:
    vec dataTransformation( vec x );
public: 
    LinearMatchTransform(const vector<Point *>& points, double beta);
    mat getTransform( vector<Point *>& shapePoints, vector<Point *>& points );
};

class QuadraticMatchTransform : public MatchTransform{
protected:
    vec dataTransformation( vec x );
public:
    QuadraticMatchTransform(const vector<Point *>& points, double beta);
    mat getTransform( vector<Point *>& shapePoints, vector<Point *>& points );
};

/*
 * Shape specification : main interface
 */

template<class T> class DeformableShape{
	private:
        double alpha;
		T* transform;  
        vector<Point *> originalShape;      
	public:
        
        void init( vector<Point *>& points );
		explicit DeformableShape( double alpha, std::vector<Point *>& points );
		vector<vec> getGoals( vector<Point*>& points );
        void setGoals( vector<Point*>& points );
        void setAlpha( double alpha );
        double getAlpha();
};

} // namespace animats
} // namespace morph

#endif