#include <armadillo>

using namespace arma;

namespace morph{

namespace animats{

class MatchTransform{
private:
	virtual vec dataTransformation( vec x ) = 0;
public:
	vec transform( vec x  );
	virtual void precompute( vector<PointMass *>& points, Cluster& cluster ) = 0;
	virtual void compute( vector<PointMass *>& points, Cluster& cluster, double beta  ) = 0;
};

class LinearMatchTransform : public MatchTransform{
private:
    mat T;
    /*
        Computers the matrix Apq
    */
    mat getApq( vector<Point *>& points );
public: 

    /*
        Precomputes the static quatities and stores it in the cluster
    */
    void precompute( vector<Point *>& points );

    /*
        Computes the transformation matrix
    */
    void compute( vector<PointMass *>& points, double beta  );

    /* 
        Applies the computed transformations to a given point
    */
    vec transform( vec x0 );

};

class QuadraticMatchTransform : public MatchTransform{
private:

    mat T;
    vec quadraticVec( vec q );
    mat getLinearApq( vector<PointMass*>& points, Cluster& cluster );
    mat getApq( vector<PointMass*>& points, Cluster& cluster );

public:
    vec transform( vec x );
    void precompute( vector<PointMass*>& points, Cluster& cluster );
    void compute( vector<PointMass*>& points, Cluster& cluster, double beta  );

};

template<class T> class DeformableShape{
	private:
		T transform;
		std::vector<Point *> originalShape;

	public:
		explicit DeformableShape();
		void computeGoals( vector<Point*>& points );

};

class ShapeMatching{
public:
		explicit ShapeMatching( double h );
		/*
		 *
		 */
		void update( GeometricObject *go );
};

}

}