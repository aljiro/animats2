#ifndef UTIL_H
#define UTIL_H

namespace morph{
namespace Animats{
	
class Debug{

private:
	Debug(){}
public:
	static const int STD_OUT = 0;
	static const int FILE = 1;
	static bool debugging;
	static int type;
	static std::ofstream lout;

	static void log( std::string msg ){
		if( debugging ){
			if( type == STD_OUT ){
				std::cout << ">>" << msg << std::endl;
			}
			else if( type == FILE ){
				lout << ">>" << msg << std::endl;
			}
		}
	}

};


inline std::string printvec( arma::vec v ){
	std::stringstream s;
	s << "(" << v(0) << ", "<<v(1)<<", "<<v(2)<<")";
	return s.str();
}

inline std::string printvec2( arma::vec v ){
	std::stringstream s;
	s << "(" << v(0) << ", "<<v(1)<<")";
	return s.str();
}

inline vec computeCenterOfMass( const vector<Point *>& points ){
	vec cg = zeros<vec>(3);
    double M = 0;
    Point *p;

    for(int i=0; i < points.size(); i++){
        p = points[i];
        //Debug::log(string("Point position: ") + printvec(p.originalPosition));
        cg += p->m*p->x;
        M += p->m;
    }

    cg /= M;
    return cg;
}

}}

#endif