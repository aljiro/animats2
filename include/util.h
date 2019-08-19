namespace morph{
namespace Animats{
	
	class Constants{
	public:
		static const DEBUGGING = true;

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

	inline void debug( std::string msg ){
		if( debugging )
			if( type == STD_OUT )
				std::cout << ">>" << msg << std::endl;
			else if( type == FILE )
				lout << ">>" << msg << std::endl;			
	}
}
}