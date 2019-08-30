#include "../include/shape.h"

using namespace morph::animats;

// MatchTransforms
MatchTransform::MatchTransform( const vector<Point *>& points, double beta ):beta(beta){
}

void MatchTransform::precompute( const vector<Point *>& points ){
	mat M = zeros<mat>(3,3);
    vec q;
    Debug::log(string("Precomputing Aqq."));

    for( Point *p : points ){
        q = p->x - computeCenterOfMass( points );
    	q = this->dataTransformation(q);
        M += p->m*(q*q.t());
    }

    this->Aqq = pinv(M);
}

mat MatchTransform::getApq( vector<Point *>& shapePoints, 
                            vector<Point *>& points, 
                            vec (*tx)(vec) ){
    
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
    mat U, V;
    vec s;
    svd( U, s, V, Apq );
    mat R = U*V.t(); 
    return R;
}

// Linear Match transform
LinearMatchTransform::LinearMatchTransform( 
                    const vector<Point *>& points, 
                    double beta ):MatchTransform( points, beta ){
    this->precompute( points );
}

vec LinearMatchTransform::dataTransformation( vec x ){
	return idTx(x);
}

mat LinearMatchTransform::getTransform( vector<Point *>& shapePoints, 
                                        vector<Point *>& points ){
	assert( shapePoints.size() == points.size() );

	mat U, V, R, Apq, Aqq, A;
	mat T(3,3, fill::eye);
    vec s;
    Apq = this->getApq( shapePoints, points, idTx );    

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
                        const vector<Point *>& points, 
                        double beta):MatchTransform( points, beta ){
    this->precompute( points );
}


vec QuadraticMatchTransform::dataTransformation( vec q ){
	return quadTx( q );
}

mat QuadraticMatchTransform::getTransform( vector<Point *>& shapePoints, 
                                           vector<Point *>& points ){
    mat U, V, Rl, R, Apq, Aqq, A, Apq_linear;
    mat T(3,3, fill::eye);
    vec s;
    Apq = this->getApq( shapePoints, points, quadTx ); 
    Apq_linear = this->getApq( shapePoints, points, []( vec x ){ return x; } );

    try{
        Rl = this->getR(Apq_linear);
        R = join_rows(join_rows( Rl, zeros(3,3) ), zeros(3,3));
        A = Apq*this->Aqq;
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
template<class T>
DeformableShape<T>::DeformableShape( double alpha, 
								  std::vector<Point *>& points ):
								  alpha(alpha){
    
}

template<class T>
void DeformableShape<T>::init( std::vector<Point *>& points ){
// Set the basic precomputations and asign the orignal shape
    Debug::log(string("Initializing deformable shape"));
    double beta = 0.1; 
    this->transform = new T( points, beta );
	this->originalShape.clear();

	for( Point *p : points ){
		Point* q = new Point( *p ); // Invoking the copying constructor
		this->originalShape.push_back( q );
        cout << printvec(p->x) << endl;
	}
}

template<class T>
vector<vec> DeformableShape<T>::getGoals( std::vector<Point *>& points ){
	vector<vec> goals;
	vec cm = computeCenterOfMass( points );
    vec cm0 = computeCenterOfMass( originalShape );
	int i;

	for( i = 0; i < points.size(); i++ ){
		vec x0 = this->originalShape[i]->x;
		mat Tx = transform->getTransform( originalShape, points );
		vec g = Tx*(x0 - cm0) + cm;
		goals.push_back( g );
	}

	return goals;
}

template<class T>
void DeformableShape<T>::setAlpha( double alpha ){
	this->alpha = alpha;
}

template<class T>
double DeformableShape<T>::getAlpha(){
    return this->alpha;
}

template class DeformableShape<LinearMatchTransform>; 