
#include "../include/solver.h"

using namespace morph::animats;

Solver::Solver( double h ):h(h), t(0.0){

}

void Solver::step( Simulation& s ){
	vector<Point *> points;
	vector<vec> goals;
	double c = 0.9;

	Debug::log(string("Computing external forces"), LOOP );
	s.computeExternalForces();

	// Move soft bodies
	for( SoftBody *go : s.getSoftBodies() ){
		Debug::log(string("Updating soft body"), LOOP );
		points = go->getPoints();
		Debug::log(string("Obtaining goals"), LOOP );
		goals = go->getShape()->getGoals( points );		
		double alpha = go->getShape()->getAlpha();

		Debug::log(string("Evolving points"), LOOP );

		for( int i = 0; i < points.size(); i++ ){
			vec x = points[i]->x;
			vec g = goals[i];
			vec f = points[i]->f;
			double m = points[i]->m;


			if( points[i]->move )
				points[i]->pre = new Point(*(points[i]));
			else{
				points[i]->pre = new Point(*(points[i]));
				// cout << "Goal fixed point: " << printvec(g) 
				// 	 << ", actual position: " << printvec(x) << endl;
			}
			
			// else{				
			// 	points[i]->pre->v =  zeros<vec>(3);
			// 	points[i]->v = zeros<vec>(3) ;
			// }			

			// Modified Euler
			if( points[i]->move )
				points[i]->vi += -alpha*( x - g )/h - h*c*points[i]->vi/m;
			
			points[i]->ve += h*f/m;
			points[i]->v = points[i]->vi + points[i]->ve + points[i]->vc;
			//points[i]->vc = zeros<vec>(3);
			
			this->t += h;
			
		}
	}

	Debug::log(string("Prunning contacts"), LOOP );
	s.collisionMgr.pruneContacts();

	Debug::log(string("Moving points"), LOOP );

	// Move soft bodies
	for( SoftBody *go : s.getSoftBodies() ){
		points = go->getPoints();

		for( int i = 0; i < points.size(); i++ ){			

			// if( !points[i]->move ){
			// 	continue;
			// }
			
			points[i]->x += h*points[i]->v;	


			// if( norm(points[i]->x - points[i]->pre->x) > 0.12 ){
			// 	points[i]->x = points[i]->pre->x;
			// 	points[i]->v = zeros<vec>(3);
			// 	// cin.get();
			// 	return;
			// }		
		}

		Debug::log(string("Euler step done!"), LOOP );
	}

	// Move Rigid bodies
	// TO-DO
}