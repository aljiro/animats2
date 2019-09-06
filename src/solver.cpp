#include "../include/solver.h"

using namespace morph::animats;

Solver::Solver( double h ):h(h), t(0.0){

}

void Solver::step( Simulation& s ){
	vector<Point *> points;
	vector<vec> goals;
	double c = 2.5;

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

			if( norm(x - g) > 0.12 ){
				points[i]->x = points[i]->pre->x;
				points[i]->v = zeros<vec>(3);
				// cin.get();
				return;
			}

			if( points[i]->move )
				points[i]->pre = new Point(*(points[i]));// Copy
			

			// Modified Euler
			// points[i]->vi = 
			points[i]->v += -alpha*( x - g )/h - h*c*points[i]->v/m + 
							h*f/m;
			
			// Call prune constants here;
			this->t += h;

			if( !points[i]->move ){
				continue;
			}
			
			points[i]->x += h*points[i]->v;

			
		}

		Debug::log(string("Euler step done!"), LOOP );
	}

	// Move Rigid bodies
	// TO-DO
}