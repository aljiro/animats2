#include "calg.h"

using namespace morph::softmats;

arma::vec morph::softmats::centroid( std::vector<Point*>& points ){
    arma::vec cm = arma::zeros<arma::vec>(3);

    for( Point *p : points ){
        cm += p->x;
    }

    return cm/points.size();
}

Vec3 morph::softmats::closestPointOnTriangle( const Vec3& p, const Vec3& a, const Vec3& b, const Vec3& c, double bary[3] ){
    const Vec3 ab = b - a;
    const Vec3 ac = c - a;
    const Vec3 ap = p - a;

    // Vertex region a
    const double d1 = dot( ab, ap );
    const double d2 = dot( ac, ap );
    if( d1 <= 0.0 && d2 <= 0.0 ){
        bary[0] = 1.0; bary[1] = 0.0; bary[2] = 0.0;
        return a;
    }

    // Vertex region b
    const Vec3 bp = p - b;
    const double d3 = dot( ab, bp );
    const double d4 = dot( ac, bp );
    if( d3 >= 0.0 && d4 <= d3 ){
        bary[0] = 0.0; bary[1] = 1.0; bary[2] = 0.0;
        return b;
    }

    // Edge region ab
    const double vc = d1*d4 - d3*d2;
    if( vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0 ){
        const double v = d1/(d1 - d3);
        bary[0] = 1.0 - v; bary[1] = v; bary[2] = 0.0;
        return a + v*ab;
    }

    // Vertex region c
    const Vec3 cp = p - c;
    const double d5 = dot( ab, cp );
    const double d6 = dot( ac, cp );
    if( d6 >= 0.0 && d5 <= d6 ){
        bary[0] = 0.0; bary[1] = 0.0; bary[2] = 1.0;
        return c;
    }

    // Edge region ac
    const double vb = d5*d2 - d1*d6;
    if( vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0 ){
        const double w = d2/(d2 - d6);
        bary[0] = 1.0 - w; bary[1] = 0.0; bary[2] = w;
        return a + w*ac;
    }

    // Edge region bc
    const double va = d3*d6 - d5*d4;
    if( va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0 ){
        const double w = (d4 - d3)/((d4 - d3) + (d5 - d6));
        bary[0] = 0.0; bary[1] = 1.0 - w; bary[2] = w;
        return b + w*(c - b);
    }

    // Inside the face
    const double denom = 1.0/(va + vb + vc);
    const double v = vb*denom;
    const double w = vc*denom;
    bary[0] = 1.0 - v - w; bary[1] = v; bary[2] = w;
    return a + v*ab + w*ac;
}
