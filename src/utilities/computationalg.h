#ifndef COMPALG_H
#define COMPALG_H

#include <iostream>
#include <vector>
#include <armadillo>
#include "util.h"

#include "../geometry/point.h"
#include "../geometry/edge.h"
#include "../geometry/face.h"
#include "../geometry/point.h"

using namespace arma;
using namespace std;

namespace morph{
namespace animats{
class Face;

template<typename F>
void zbrak( F fx, float x1, float x2, int n, float xb1[], float xb2[], int *nb );

template<typename F>
float rtflsp( F func, float x1, float x2, float xacc );
vec clamp( vec x1, vec x2, vec x3, vec x4, vec c, vec *p1, vec *p2 );
bool isProximal( Face *f, Point* p, double delta = -1 );
bool isProximal( Edge *ep, Edge* ef, double delta = -1 );
bool isColliding( Face *f, Point* p, vec *w, double* hc, double current_h );
bool isColliding( Edge* ep, Edge* ef, double* hc, double current_h );
bool isContact( Face* f, Point* p );
bool isContact( Edge* ep, Edge* ef );
vector<vec> getInelasticImpulses( Face *f, Point *p, vec *w );
vector<vec> getInelasticImpulses( Edge *ep, Edge *ef );
vector<vec> getCollisionImpulses( Face* f, Point *p, vec *wp );
vector<vec> getCollisionImpulses( Edge* ep, Edge* ef );

/**
 * centroid
 */
vec centroid( vector<Point *>& points );
vec computeBarycentricCoords( vec p1, vec p2, vec p3, vec pos );
vec normalCoefficients( vec x1, vec x2, vec x3, vec x4 );
double collision_poly(double t, vec x1, vec x2, vec x3, vec x4, vec v1, vec v2, vec v3, vec v4 );
vec computeEdgeNormal( vec x1, vec x2, vec x3, vec x4 );
double computeEdgeDistance( vec x1, vec x2, vec x3, vec x4 );
}}

#endif