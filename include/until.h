namespace morph{
namespace Animats{
	
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
}
}