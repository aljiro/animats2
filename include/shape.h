namespace morph{

namespace shape{

	class Shape{
		private:
			Mat Aqq;

		public:

			

	};

	template<class T> class DeformableShape{
		private:
			T transform;

			void computeGoals( vector<Point*>& points );

		public:
			/*
			 *
			 */
			void applyInternalForcesSpeed( GeometricObject *go );
	};

}

}