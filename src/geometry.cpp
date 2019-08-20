#include "geometry.h"

// GeometricTransform fields
GeometricTransform::GeometricTransform(){
	this->M = { {1, 0, 0, 0}, 
				{0, 1, 0, 0}, 
				{0, 0, 1, 0},
				{0, 0, 0, 1} };
}

void GeometricTransform::map( vector<PointMass *>& points ){

	for( Point *p : points ){
		vec x = {p->x(0), p->x(1), p->x(2), 1};
		vec xp = this->M*x;
		p->x = {xp(0), xp(1), xp(2)};
	}
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