#include <armadillo>

using namespace arma;

namespace morph{

namespace animats{

/*
 * Transforms
 */

class MatchTransform{
private:
    mat Aqq;

	virtual vec dataTransformation( vec x ) = 0;
    void precompute( vector<PointMass *>& points );
    mat MatchTransform::getApq( vector<Point *>& shapePoints, vector<Point *>& points );
    void getR( vector<PointMass *>& points );

public:
    explicit MatchTransform( double beta );
    virtual mat getTransform( vector<Point *>& shapePoints, vector<Point *>& points );
};

class LinearMatchTransform : public MatchTransform{
private:
    vec dataTransformation( vec x );
public: 
    mat getTransform( vector<Point *>& shapePoints, vector<Point *>& points );
};

class QuadraticMatchTransform : public MatchTransform{
private:
    vec dataTransformation( vec x );
public:
    mat getTransform( vector<Point *>& shapePoints, vector<Point *>& points );
};

/*
 * Shape specification
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