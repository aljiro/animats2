#include "shape.h"

// MatchTransforms
MatchTransform::MatchTransform( const vector<Point *>& points, double beta ):beta(beta){
    this->precompute( points );
}

void MatchTransform::precompute( const vector<Point *>& points ){
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

mat MatchTransform::getApq( vector<Point *>& shapePoints, vector<Point *>& points, const vec (*tx)(vec) ){
    
    assert( shapePoints.size() == points.size() );

    mat Apq = zeros<mat>(3,3);
    vec q, p, cm, original_cm;
    cm = computeCenterOfMass( points );
    original_cm = computeCenterOfMass( shapePoints );

    for( int i = 0; i < points.size(); i++ ){
        q = (*tx)(shapePoints[i]->x - original_cm);
        p = (*tx)(points[i]->x - cm);
        Apq += points[i]->m*(p*q.t());
    }

    return Apq;
}

mat MatchTransform::getR( const mat& Apq ){
    svd( U, s, V, Apq );
    mat R = U*V.t(); 
    return R;
}

// Linear Match transform
LinearMatchTransform::LinearMatchTransform( 
                    const vector<Point *>& points, 
                    double beta ):MatchTransform( points, beta ){}

vec LinearMatchTransform::dataTransformation( vec x ){
	return x;
}

void LinearMatchTransform::getTransform( vector<Point *>& shapePoints, 
                                         vector<Point *>& points ){
	assert( shapePoints.size() == points.size() );

	mat U, V, R, Apq, Aqq, A;
	mat T(3,3, fill::eye);
    vec s;
    Apq = this->getApq( shapePoints, point, this->dataTransformation );    

    try{
        R = getR( Apq ); // Apq*sqrt(Apq.t()*Apq)
        A = Apq*this->Aqq;
        double da = det(A);

        if( da < 0 )
        {
            cout << "ERROR: determinant < 0" << endl;
            cin.get();
        }

        A = A/pow( da, 1.0/3.0 );
        T = this->beta*A + (1 - this->beta)*R;
    }catch(const std::exception& e){
        cout << "Error: unable to compute the transformation: "<< e.what();
    }

    return T;
}

// Quadratic transform
QuadraticMatchTransform::QuadraticMatchTransform( 
                        const vector<PointMass *>& points, 
                        double beta):MatchTransform( points, beta ){}


vec QuadraticMatchTransform::dataTransformation( vec q ){
	vec qc = {q(0), q(1), q(2), q(0)*q(0), q(1)*q(1), q(2)*q(2), q(0)*q(1), q(1)*q(2), q(0)*q(2)};
	return qc;
}

void QuadraticMatchTransform::getTransform( vector<Point *>& points ){
    mat U, V, Rl, R, Apq, Aqq, A, Apq_linear;
    mat T(3,3, fill::eye);
    vec s;
    Apq = this->getApq( points, this->dataTransformation ); 
    Apq_linear = this->getApq( points, []( vec& x ){ return x; } );

    try{
        Rl = this->getR(Apq_linear);
        R = join_rows(join_rows( Rl, zeros(3,3) ), zeros(3,3));
        A = Apq*cluster.Aqq;
        // TO-DO: preserve volume        
        //double da = det(A);
        //A = A/pow(da, 1.0/3.0 );
        T = this->beta*A + (1 - this->beta)*R;
    }catch(const std::exception& e){
        cout << "Error: unable to compute the transformation: "<< e.what();
    }

    return T;
}


// Shape
DeformableShape::DeformableShape( double alpha, 
								  std::vector<Point *>& points ):
								  alpha(alpha){
    double beta = 0.1;                                        
	this->transform = new T( points, beta );
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
    vec cm0 = computeCenterOfMass( originalShape );
	int i;

	for( i = 0; i < points.size(); i++ ){
		vec x0 = this->originalShape[i]->x;
		mat T = transform.getTransform( points );
		vec g = T*(x0 - cm0) + cm;
		goals.push_back( g );
	}

	return std::shared_ptr<vector<vec>>(&goals);
}

void DeformableShape::setAlpha( double alpha ){
	this->alpha = alpha;
}