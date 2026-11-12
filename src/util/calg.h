#ifndef COMPALG_H
#define COMPALG_H

#include <vector>
#include <armadillo>

#include "../core/point.h"
#include "vec3.h"

namespace morph{ namespace softmats{
/**
 * Computational geometry used in the simulation
 * @author Alejandro Jimenez Rodriguez
 */

// Average position of a set of points
arma::vec centroid( std::vector<Point *>& points );

/**
 * Closest point to p on the triangle (a, b, c).
 *
 * @param bary - receives the barycentric coordinates of the closest point,
 *               so that closest = bary[0]*a + bary[1]*b + bary[2]*c
 * @see Ericson, C. (2004). Real-time collision detection, section 5.1.5.
 */
Vec3 closestPointOnTriangle( const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c, double bary[3] );

}}

#endif
