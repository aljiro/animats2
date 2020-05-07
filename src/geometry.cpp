#include "../include/geometry.h"

using namespace morph::animats;

// GeometricTransform fields
GeometricTransform::GeometricTransform(){
	this->M = { {1, 0, 0, 0}, 
				{0, 1, 0, 0}, 
				{0, 0, 1, 0},
				{0, 0, 0, 1} };
}

void GeometricTransform::map( vector<Point *>& points ){

	for( Point *p : points ){
		p->x = mapVec(p->x);
	}
}

vec GeometricTransform::mapVec( vec y ){
	vec x = {y(0), y(1), y(2), 1};
	vec xp = this->M*x;
	xp = {xp(0), xp(1), xp(2)};
	return xp;
}

void GeometricTransform::compose( GeometricTransform& GT ){
	this->M = GT.M*this->M;
}

// ScaleTransform
ScaleTransform::ScaleTransform( double sx, double sy, double sz){
	this->M = { {sx, 0, 0, 0}, 
				{0, sy, 0, 0}, 
				{0, 0, sz, 0},
				{0, 0, 0, 1} };
} 

// RotateTransform
RotateTransform::RotateTransform( double alpha, double beta, double gamma ){
	const double PI = 3.1416;
	auto f = []( auto x){return 3.1416*x/180.0;};
	alpha = f(alpha);
	beta = f(beta);
	gamma = f(gamma);
	mat Rx = { {1, 0, 0, 0}, 
				 {0, cos(alpha), -sin(alpha), 0}, 
				 {0, sin(alpha), cos(alpha), 0},
				 {0, 0, 0, 1} };
	mat Ry = { {cos(beta), 0, -sin(beta), 0}, 
				{0, 1, 0, 0}, 
				{sin(beta), 0, cos(beta), 0},
				{0, 0, 0, 1} };
	mat Rz = { {cos(gamma), -sin(gamma), 0, 0}, 
				{sin(gamma), cos(gamma), 0, 0}, 
				{0, 0, 1, 0},
				{0, 0, 0, 1} };
	this->M = Rx * Ry * Rz;

}


// TranslateTransform
TranslateTransform::TranslateTransform( double tx, double ty, double tz ){
	this->M = { {1, 0, 0, tx}, 
				{0, 1, 0, ty}, 
				{0, 0, 1, tz},
				{0, 0, 0, 1} };
}