#pragma once
#include <armadillo>
#include <cmath>

namespace morph{ namespace softmats{
/**
 * Plain 3D vector used in the inner loops of the collision code, where the
 * general purpose armadillo vectors are too slow.
 */
struct Vec3{
    double x, y, z;
};

inline Vec3 operator+( const Vec3& a, const Vec3& b ){ return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 operator-( const Vec3& a, const Vec3& b ){ return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 operator*( double s, const Vec3& a ){ return {s*a.x, s*a.y, s*a.z}; }
inline double dot( const Vec3& a, const Vec3& b ){ return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Vec3 cross( const Vec3& a, const Vec3& b ){
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
inline double length( const Vec3& a ){ return std::sqrt( dot(a, a) ); }

inline Vec3 toVec3( const arma::vec& a ){ return {a(0), a(1), a(2)}; }
inline arma::vec toArma( const Vec3& a ){ return arma::vec({a.x, a.y, a.z}); }
inline void addTo( arma::vec& a, const Vec3& b ){ a(0) += b.x; a(1) += b.y; a(2) += b.z; }

}}
