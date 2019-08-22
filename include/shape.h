#include "point.h"
#include "util.h"
#include <armadillo>

using namespace arma;
using namespace std;

namespace morph{ namespace animats{

/*
 * Transforms
 */

class MatchTransform{
private:
    mat Aqq;
    double beta;

	virtual vec dataTransformation( vec x ) = 0;
    void precompute( const vector<Point *>& points );
    mat getApq( vector<Point *>& shapePoints, vector<Point *>& points, const vec (*f)(vec) );
    mat getR( const mat& Apq );

public:
    explicit MatchTransform( const vector<Point *>& points, double beta );
    virtual mat getTransform( vector<Point *>& shapePoints, vector<Point *>& points );
};

// Main subclasses

class LinearMatchTransform : public MatchTransform{
private:
    vec dataTransformation( vec x );
public: 
    LinearMatchTransform(const vector<Point *>& points, double beta);
    mat getTransform( vector<Point *>& shapePoints, vector<Point *>& points );
};

class QuadraticMatchTransform : public MatchTransform{
private:
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
		T transform;
		std::vector<Point *> originalShape;

        void init( std::vector<Point *>& points );
	public:
		explicit DeformableShape( double alpha, std::vector<Point *>& points );
		vector<vec> getGoals( vector<Point*>& points );
        void setAlpha( double alpha );
};

} // namespace animats
} // namespace morph