#include "shape.h"

// MatchTransforms
void MatchTransform::precompute( vector<Point *>& points ){
	mat M = zeros<mat>(3,3);
    vec q;
    Debug::log(string("Precomputing Aqq. "));

    for( Point *p : points ){
        q = p->originalPosition - computeCenterOfMass( points );
    	q = this->dataTransformation(q);
        M += p->m*(q*q.t());
    }

    this->Aqq = pinv(M);
}

mat MatchTransform::getApq( vector<Point *>& shapePoints, vector<Point *>& points ){
    
    assert( shapePoints.size() == points.size() );

    mat Apq = zeros<mat>(3,3);
    vec q, p, cm, original_cm;
    cm = computeCenterOfMass( points );
    original_cm = computeCenterOfMass( shapePoints );

    for( int i = 0; i < points.size(); i++ ){
        q = this->dataTransformation(shapePoints[i]->x - original_cm);
        p = this->dataTransformation(points[i]->x - cm);
        Apq += points[i]->m*(p*q.t());
    }

    return Apq;
}

// Linear Match transform
vec LinearMatchTransform::dataTransformation( vec x ){
	return x;
}

void LinearMatchTransform::getTransform( vector<Point *>& shapePoints, vector<Point *>& points ){
	assert( shapePoints.size() == points.size() );

	mat U, V, R, Apq, Aqq, A;
	mat T(3,3, fill::eye);
    vec s;
    Apq = this->getApq( shapePoints, points );    

    try{
        svd( U, s, V, Apq );
        R = U*V.t(); // Apq*sqrt(Apq.t()*Apq)
        A = Apq*cluster.Aqq;
        double da = det(A);

        if( da < 0 )
        {
            cout << "ERROR: determinant < 0" << endl;
            cin.get();
        }

        A = A/pow( da, 1.0/3.0 );
        T = beta*A + (1 - beta)*R;
    }catch(const std::exception& e){
        cout << "Error: unable to compute the transformation: "<< e.what();
    }

    return T;
}

// Quadratic transform
vec QuadraticMatchTransform::dataTransformation( vec q ){
	vec qc = {q(0), q(1), q(2), q(0)*q(0), q(1)*q(1), q(2)*q(2), q(0)*q(1), q(1)*q(2), q(0)*q(2)};
	return qc;
}

void QuadraticMatchTransform::getTransform( vector<Point *>& points ){

}


// Shape
explicit DeformableShape::DeformableShape( double alpha, 
										   std::vector<Point *>& points ):
										   alpha(alpha){
	this->transform = new T( points );
	init( points );
}

void DeformableShape::init( std::vector<Point *>& points ){
// Set the basic precomputations and asign the orignal shape
	this->originalShape.clear();

	for( Point *p : points ){
		Point* q = new Point( *p ); // Invoking the copying constructor
		this->originalShape.push_back( q );
	}
}

void DeformableShape::getGoals( std::vector<Point *>& points ){
	vector<vec> goals;
	vec cm = computeCenterOfMass( points );
	int i;

	for( i = 0; i < points.size(); i++ ){
		vec cm0 = this->getCenterOfMass();
		vec x0 = this->originalShape[i]
		mat T = transform.getTransform( points );
		vec g = T*(x0 - cm0) + cm;
		goals.push_back( g );
	}

	return std::shared_ptr<vector<vec>>(&goals);
}

void DeformableShape::setAlpha( double alpha ){
	this->alpha = alpha;
}