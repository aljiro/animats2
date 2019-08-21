#include "shape.h"

// MatchTransforms

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
		this->originalShape.push_back( p->x );
	}
}

void DeformableShape::getGoals( std::vector<Point *>& points ){
	vector<vec> goals;
	vec cm = computeCenterOfMass( points );
	int i;

	for( i = 0; i < points.size(); i++ ){
		vec cm0 = this->getCenterOfMass();
		vec x0 = this->originalShape[i]
		Mat T = transform.getTransform( points );
		vec g = T*(x0 - cm0) + cm;
		goals.push_back( g );
	}

	return std::shared_ptr<vector<vec>>(&goals);
}

void DeformableShape::setAlpha( double alpha ){
	this->alpha = alpha;
}