#include <armadillo>

using namespace arma;

namespace morph{

namespace animats{

/*
 * Transforms
 */

class MatchTransform{
private:
	virtual vec dataTransformation( vec x ) = 0;
    void precompute( vector<PointMass *>& points );
    void getR( vector<PointMass *>& points );
public:
    explicit MatchTransform( double beta );
    virtual void getTransform( vector<Point *>& points );
};

class LinearMatchTransform : public MatchTransform{
private:
    virtual vec dataTransformation( vec x ) = 0;
public: 
    void getTransform( vector<Point *>& points );
};

class QuadraticMatchTransform : public MatchTransform{
private:
    virtual vec dataTransformation( vec x ) = 0;
public:
    void getTransform( vector<Point *>& points );
};

/*
 * Shape specification
 */

template<class T> class DeformableShape{
	private:
        double alpha;
		T transform;
		std::vector<vec> originalShape;

        void init( std::vector<Point *>& points );
	public:
		explicit DeformableShape( double alpha, std::vector<Point *>& points );
		vector<vec> getGoals( vector<Point*>& points );
        void setAlpha( double alpha );
};

} // namespace animats
} // namespace morph