#include "computationalg.h"

using namespace arma;
using namespace morph::animats;

template<typename F>
void morph::animats::zbrak( F fx, float x1, float x2, int n, float xb1[], float xb2[], int *nb ){
    int nbb, i;
    float x, fp, fc, dx;

    nbb = 0;
    dx = (x2 - x1)/n;
    fp = fx( x = x1 );

    for( i = 1; i <= n; i++ ){
        fc = fx( x += dx );

        if( fc*fp < 0.0 ){
            xb1[nbb] = x - dx;
            xb2[nbb++] = x;
            if( *nb == nbb ) return;
        }

        fp = fc;
    }

    *nb = nbb;
}

template<typename F>
float morph::animats::rtflsp( F func, float x1, float x2, float xacc ){
    const int MAXIT_FP = 30;
	int j;
    float fl, fh, xl, xh, swap, dx, del, f, rtf;

    fl = func(x1);
    fh = func(x2);

    if( fl*fh > 0.0 ){
        cout << "Error, no bracketing" << endl; 
        return 0.0;
    }

    if( fl < 0.0 ){
        xl = x1;
        xh = x2;
    }else{
        xl = x2;
        xh = x1;
        swap = fl;
        fl = fh;
        fh = swap;
    }

    dx = xh - xl;

    for( j = 1; j <= MAXIT_FP; j++ ){
        rtf = xl + dx*fl/(fl - fh);
        f = func(rtf);

        if( f < 0.0 ){
            del = xl - rtf;
            xl = rtf;
            fl = f;
        }else {
            del = xh - rtf;
            xh = rtf;
            fh = f;
        }

        dx = xh - xl;
        if( std::fabs(del) < xacc || f == 0.0 ) return rtf; 
    }

    cout << "Error: failed to converge" << endl;
    return -1000;
}

bool morph::animats::isProximal( Face *f, Point* p, double delta ){
	// cout << "Checking proximity face-point" << endl;
	double h_prox = 0.01;
	double c_length = 10.0;
	
	if( delta == -1 )
		delta = h_prox/c_length;

	vec x1 = f->points[0]->x;
	vec x2 = f->points[1]->x;
	vec x3 = f->points[2]->x;
	vec x4 = p->x;
	vec x43 = x4 - x3;

	f->computeNormal();

	if( abs(dot(x43, f->normal)) < h_prox ){
	// Potentially proximal
		// compute projection onto the plane
		vec x43_p = x4 - dot(x43, f->normal)*f->normal;
		vec w = computeBarycentricCoords( x1, x2, x3, x43_p );
		return allInInterval( w, -delta, 1 - delta);
	}

	return false;
}

vec morph::animats::normalCoefficients( vec x1, vec x2, vec x3, vec x4 ){
	// cout << "Computing the edge coefficients" << endl;
	vec x21 = x2 - x1;
	vec x43 = x4 - x3;
	vec x31 = x3 - x1;
	vec c;
	double p = norm(cross(x21, x43));
	if( p < 0.0001 ){
		c = {0.5, 0.5};
		return c;
	}

	// Solve linear system
	mat A(2,2);
	vec B = {dot(x21, x31), -dot(x43, x31)};
	A(0,0) = dot(x21, x21);
	A(0,1) = -dot(x21, x43);
	A(1,0) = -dot(x21, x43);
	A(1,1) = dot(x43, x43);
	// cout << "Edge 1: " << printvec(x1) << " -> " << printvec(x2) << endl;
	// cout << "Edge 2: " << printvec(x3) << " -> " << printvec(x4) << endl;
	// cout << "Solving the system" << endl;
	// cout << A << endl;
	// cout << B << endl;
	c = solve( A, B );
	// cout << "c: " << c << endl;
	
	return c;
}

vec morph::animats::clamp( vec x1, vec x2, vec x3, vec x4, vec c, vec *p1, vec *p2 ){
	// Clamping
	vec x21 = x2 - x1;
	vec x43 = x4 - x3;

	double a = c(0);
	double b = c(1);
	
	a = a < 0.0? 0.0 : (a > 1.0? 1.0 : a);
	b = b < 0.0? 0.0 : (b > 1.0? 1.0 : b);

	(*p1) = x1 + a*x21;
	(*p2) = x3 + b*x43;

	double da = fabs(a - c(0));
	double db = fabs(b - c(1));

	if( da > 0 || db > 0 ){
        vec u;
		if( da > db ){ // Project p
            u = (*p1) - x3;
			(*p2) = x3 + dot(u, x43/norm(x43))*x43/norm(x43);
			b = dot( x43, (*p2) - x3 )/dot(x43, x43);
		}else if( db > da ){
            u = (*p2) - x1;
			(*p2) = x1 + dot(u, x21)*x21/dot(x21, x21);
			a = dot(x21, (*p1) - x1)/dot(x21, x21);
		}
	}


	vec cp = {a, b};
	return cp;
}

vec morph::animats::computeEdgeNormal( vec x1, vec x2, vec x3, vec x4 ){
	vec x21 = x2 - x1;
	vec x43 = x4 - x3;
	vec x31 = x3 - x1;

	// Check if they are parallel
	double p = norm(cross(x21, x43));	

	if( p < 0.0001 ){
		// cout << "Parallel edges" << endl;
		if( norm( x1 - x3 ) < norm( x1 - x4 )) 
			return x1 - x3;
		else
			return x1 - x4;
	}else{
		vec c = normalCoefficients( x1, x2, x3, x4 );
		// To-Do: Do the clamping correctly
		vec x_p;
		vec x_f;

		clamp( x1, x2, x3, x4, c, &x_p, &x_f );
		return x_p - x_f;
	}
}

double morph::animats::computeEdgeDistance( vec x1, vec x2, vec x3, vec x4 ){
	return norm( computeEdgeNormal(x1, x2, x3, x4) );
} 

bool morph::animats::isProximal( Edge *ep, Edge* ef, double delta ){
	// cout << "Checking proximity edge-edge" << endl;
	if( delta == -1 )
		delta = 0.01;

	vec x1 = ep->v0->x;
	vec x2 = ep->v1->x;
	vec x3 = ef->v0->x;
	vec x4 = ef->v1->x;
	// cout << "Computing edge distance" << endl;
	double d = computeEdgeDistance(x1, x2, x3, x4 );
	return d < delta;
}

double morph::animats::collision_poly(double t, vec x1, vec x2, vec x3, vec x4, vec v1, vec v2, vec v3, vec v4 ){
	vec x21 = x2 - x1;
	vec x31 = x3 - x1;
	vec x41 = x4 - x1;
	vec v21 = v2 - v1;
	vec v31 = v3 - v1;
	vec v41 = v4 - v1;

	vec a = x21 + t*v21;
	vec b = x31 + t*v31;
	vec c = x41 + t*v41;

	return dot(cross( a, b ), c);
}

bool morph::animats::isContact( Face* f, Point* p ){
	// cout << "Checking contact" << endl;
	vec x1 = f->points[0]->x;
	vec x2 = f->points[1]->x;
	vec x3 = f->points[2]->x;
	vec x4 = p->x;

	f->computeNormal();
	vec w = computeBarycentricCoords( x1, x2, x3, x4 );

	if( abs(dot(f->normal, x1-x4)) < 1e-10 && allInInterval(w, 0, 1) )
		return true;
	else return false;	 
}

bool morph::animats::isContact( Edge* ep, Edge* ef ){

	vec x1 = ep->v0->x;
	vec x2 = ep->v1->x;
	vec x3 = ef->v0->x;
	vec x4 = ef->v1->x;
	// cout << "Computing edge distance" << endl;
	double d = computeEdgeDistance(x1, x2, x3, x4 );
	return d < 1e-10;
}

bool morph::animats::isColliding( Face *f, Point* p, vec *w, double* hc, double current_h ){
	// cout << "Checking f-p collision: h = " << current_h << endl;
	double h = current_h;
	double delta = 1e-10;
	vec x1 = f->points[0]->x;
	vec x2 = f->points[1]->x;
	vec x3 = f->points[2]->x;
	vec x4 = p->x;
	vec v1 = f->points[0]->v_half;
	vec v2 = f->points[1]->v_half;
	vec v3 = f->points[2]->v_half;
	vec v4 = p->v_half;

	f->computeNormal();
	if( isContact( f, p ) && dot((v4 - v1),f->normal)< 0 ){
		(*hc) = 0.0;
		(*w) = computeBarycentricCoords( x1, x2, x3, x4 );
		return true;
	}

	auto fx = [&]( float t ){
        return collision_poly( t, x1, x2, x3, x4, v1, v2, v3, v4 );
    };

	int nb = 3;
    float tb1[3], tb2[3];
    zbrak( fx, 0.0, h, 30, tb1, tb2, &nb );

    if( nb == 0 ){
		return false;
	}

	float t1, t2, rt = 10000, trt;

	for( int i = 0; i < nb; i++ ){
		t1 = tb1[i];
		t2 = tb2[i];
		rt = rtflsp( fx, t1, t2, 1e-10);
		
		vec u1 = x1 + rt*v1;
		vec u2 = x2 + rt*v2;
		vec u3 = x3 + rt*v3;
		vec y =  x4 + rt*v4;
		vec nt = cross(u1-u2, u1-u3);

		(*w) = computeBarycentricCoords( u1, u2, u3, y );
		(*hc) = rt;

		if( abs(dot(nt, u1-y)) < 0.00001 && allInInterval( *w, -delta, 1-delta) ){
			return true;
		}
	}
	
	return false;	
}

bool morph::animats::isColliding( Edge* ep, Edge* ef, double *hc, double current_h ){
	// cout << "Checking e-e collision" << endl;
	double h = current_h;

	// if( h < 1e-6 )
	// 	{cout << "step too small!"<< endl;cin.get();}
	double delta = 1e-6;
	vec x1 = ep->v0->x;
	vec x2 = ep->v1->x;
	vec x3 = ef->v0->x;
	vec x4 = ef->v1->x;
	vec v1 = ep->v0->v_half;
	vec v2 = ep->v1->v_half;
	vec v3 = ef->v0->v_half;
	vec v4 = ef->v1->v_half;

	vec n = computeEdgeNormal(x1, x2, x3, x4 );

	if( isContact( ep, ef ) && dot(v4-v1, n) < 0 ){
		(*hc) = 0.0;
		return true;
	}

	auto f = [&]( float t ){
        return collision_poly( t, x1, x2, x3, x4, v1, v2, v3, v4 );
    };

	int nb = 3;
    float tb1[3], tb2[3];
    zbrak( f, 0.0, h, 30, tb1, tb2, &nb );
    // cout << "Potential roots:" << nb << endl;

    if( nb == 0 ) return false;

	float t1, t2, rt = 10000, trt;
	
	for( int i = 0; i < nb; i++ ){
		t1 = tb1[i];
		t2 = tb2[i];
		rt = rtflsp( f, t1, t2, 1e-10);
		
		double d = computeEdgeDistance( x1 + rt*v1, x2 + rt*v2, x3 + rt*v3, x4 + rt*v4 );
		(*hc) = rt;
		if( d < delta ) return true;
	}
	
	return false;	
}

vector<vec> morph::animats::getInelasticImpulses( Face *f, Point *p, vec *wp ){
	// cout << "Getting f-p inelastic impulses" <<endl;
	vec x1 = f->points[0]->x;
	vec x2 = f->points[1]->x;
	vec x3 = f->points[2]->x;
	vec x4 = p->x;
	vec v1 = f->points[0]->v_half;
	vec v2 = f->points[1]->v_half;
	vec v3 = f->points[2]->v_half;
	vec v4 = p->v_half;
	// Compute normal direction
	f->computeNormal();
	vec n = f->normal;
	// Compute relative velocity
	vec w;
	if( wp == NULL ){
		vec x43 = x4 - x3;
		vec x4_p = x4 - dot(x43, n)*n;
		w = computeBarycentricCoords( x1, x2, x3, x4_p );
	}
	else w = *wp;

	vec vb = w(0)*v1 + w(1)*v2 + w(2)*v3;
	vec xt = w(0)*x1 + w(1)*x2 + w(1)*x3;
	vec xn = x4 - xt;
	vec v_rel = v4 - vb;

	// if( dot(n, xn) < 0 ){
	// 	cout << "Normal pointing in the wrong direction!!!" << endl;
	// 	cin.get();
	// 	n = -n;
	// }
	double vn;

	if(dot(v_rel, n) > 0)
		vn = 0.0;
	else
		vn = -dot(v_rel, n);
	// cout << "Relative velocity: " << vn << ", with vb: " << printvec(vb) << endl;

	// Adding impulse
	double mp = p->m;
	double mt = f->points[0]->m;
	double Ip = mp*vn/2.0;
	double It = mt*vn/2.0;


	double I = 2*Ip/( 1 + w(0)*w(0) + w(1)*w(1) + w(2)*w(2) );
	vector<vec> vels ={ -(w(0)*I/mt)*n, -(w(1)*I/mt)*n, -(w(2)*I/mt)*n, (I/mp)*n};
	// cout << "Face-point Impuses: " << vels[0] << ", " << vels[1] << ", " << vels[2] << ", " << vels[3] << endl;
	return vels;
}

vector<vec> morph::animats::getInelasticImpulses( Edge *ep, Edge *ef ){
	// cout << "Getting inelasting e-e impulses" << endl;
	vec x1 = ep->v0->x;
	vec x2 = ep->v1->x;
	vec x3 = ef->v0->x;
	vec x4 = ef->v1->x;
	vec v1 = ep->v0->v_half;
	vec v2 = ep->v1->v_half;
	vec v3 = ef->v0->v_half;
	vec v4 = ef->v1->v_half;
	// Compute normal direction
	vec n = computeEdgeNormal( x1, x2, x3, x4 );
	n /= norm(n);

	// Compute relative velocity
	vec c = normalCoefficients( x1, x2, x3, x4 );
	vec p1, p2;
	
	c = clamp( x1, x2, x3, x4, c, &p1, &p2 );
	double a = c(0);
	double b = c(1);
	vec va = (1-a)*v1 + a*v2;
	vec vb = (1-b)*v3 + b*v4;
	
	vec v_rel = va - vb;
	double vn;
	if( dot( v_rel, n ) > 0 ){
		vn = 0.0;
	}else
		vn = -dot( v_rel, n );
	
	// Compute impulse
	double m1 = ep->v0->m;
	double m2 = ef->v0->m;
	double Ip = m1*vn/2.0;
	double If = m2*vn/2.0;
	double I1 = 2*Ip/(a*a + (1-a)*(1-a) + b*b + (1-b)*(1-b));
	double I2 = 2*If/(a*a + (1-a)*(1-a) + b*b + (1-b)*(1-b));
	vector<vec> vels = {(1-a)*(I1/m1)*n, a*(I1/m1)*n, -(1-b)*(I2/m2)*n, -b*(I2/m2)*n};
	// cout << "Edge Impuses: " << vels[0] << ", " << vels[1] << ", " << vels[2] << ", " << vels[3] << endl;
	return vels;
}

vector<vec> morph::animats::getCollisionImpulses( Face* f, Point *p, vec *wp ){
	// cout << "Computing face-point collision impulses" << endl;
	vec x1 = f->points[0]->x;
	vec x2 = f->points[1]->x;
	vec x3 = f->points[2]->x;
	vec x4 = p->x;
	vec v1 = f->points[0]->v_half;
	vec v2 = f->points[1]->v_half;
	vec v3 = f->points[2]->v_half;
	vec v4 = p->v_half;
	// Compute normal direction
	f->computeNormal();
	vec n = f->normal;
	// Compute relative velocity
	vec w;
	if( wp == NULL )
		w = computeBarycentricCoords( x1, x2, x3, x4 );
	else w = *wp;

	vec vb = w(0)*v1 + w(1)*v2 + w(2)*v3;
	vec xt = w(0)*x1 + w(1)*x2 + w(2)*x3;
	vec xn = x4 - xt;
	// if( dot(n, xn) < 0 ){
	// 	cout << "Normal pointing in the wrong direction!!!" << endl;
	// 	// cin.get();
	// 	n = -n;
	// }
	vec vr = v4 - vb;
	double vn = -dot( vr, n ); // pointing outside the triangle assuming they are approaching

	// Adding impulse
	double mp = p->m;
	double mt = f->points[0]->m;
	double Ip = mp*vn;
	double It = mt*vn;

	if (  dot( vr, n ) > 0 ){
		Ip = 0.0;
		// If = 0.0;
	}else{
		Ip = mp*vn;
		// If = mt*vn;
	}

	double I = 2*Ip/( 1 + w(0)*w(0) + w(1)*w(1) + w(2)*w(2) );
	vector<vec> vels ={ -(w(0)*I/mt)*n, -(w(1)*I/mt)*n, -(w(2)*I/mt)*n, (I/mp)*n};
	
	return vels;

}

vector<vec> morph::animats::getCollisionImpulses( Edge* ep, Edge* ef ){
	// cout << " Computing edge - edge collision impulses" << endl;
	vec x1 = ep->v0->x;
	vec x2 = ep->v1->x;
	vec x3 = ef->v0->x;
	vec x4 = ef->v1->x;
	vec v1 = ep->v0->v_half;
	vec v2 = ep->v1->v_half;
	vec v3 = ef->v0->v_half;
	vec v4 = ef->v1->v_half;
	// Compute normal direction
	vec n = computeEdgeNormal( x1, x2, x3, x4 );
	n /= norm(n);

	// Compute relative velocity
	vec c = normalCoefficients( x1, x2, x3, x4 );
	vec p1, p2;
	c = clamp( x1, x2, x3, x4, c, &p1, &p2 );
	double a = c(0);
	double b = c(1);
	
	vec va = (1-a)*v1 + a*v2;
	vec vb = (1-b)*v3 + b*v4;
	
	vec v_rel = va - vb;
	double vn = -dot( v_rel, n );	

	
	// Compute impulse
	double m1 = ep->v0->m;
	double m2 = ef->v0->m;
	double Ip = m1*vn;
	double If = m2*vn;

	if (  dot( v_rel, n ) > 0 ){
		Ip = 0.0;
		If = 0.0;
	}else{
		Ip = m1*vn;
		If = m2*vn;
	}


	double I1 = 2*Ip/(a*a + (1-a)*(1-a) + b*b + (1-b)*(1-b));
	double I2 = 2*If/(a*a + (1-a)*(1-a) + b*b + (1-b)*(1-b));
	vector<vec> vels = {(1-a)*(I1/m1)*n, a*(I1/m1)*n, -(1-b)*(I2/m2)*n, -b*(I2/m2)*n};
	
	return vels;
}


/**
 * getCentroid - DEPRECATED 
 */
vec morph::animats::centroid( vector<Point *>& points ){
	vec p1 = points[0]->x;
	vec p2 = points[1]->x;
	vec p3 = points[2]->x;

	return (p1 + p2 + p3)/3;
}


vec morph::animats::computeBarycentricCoords( vec p1, vec p2, vec p3, vec pos ){
	// Computing the barycentric coordinates
	vec u0 = p2 - p1;
	vec u1 = p3 - p1;
	vec u2 = pos - p1;

	double d11 = dot( u1, u1 );
	double d00 = dot( u0, u0 );
	double d02 = dot( u0, u2 );
	double d01 = dot( u0, u1 );
	double d12 = dot( u1, u2 );


	double dT = d00*d11 - d01*d01;
	double lambda2 = (d11*d02 - d01*d12)/dT;
	double lambda3 = (d00*d12 - d01*d02)/dT;
	double lambda1 = 1 - lambda2 - lambda3;

	vec w = {lambda1, lambda2, lambda3};
	return w;
}