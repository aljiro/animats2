#include "../include/solver.h"

using namespace morph::animats;

Solver::Solver( double h ):h(h), t(0.0){

}

void Solver::step( Simulation& s ){
	vector<Point *> points;
	vector<vec> goals;

	Debug::log(string("Computing external forces"), LOOP );
	s.computeExternalForces();

	// Move soft bodies
	for( SoftBody *go : s.getSoftBodies() ){
		Debug::log(string("Updating soft body"), LOOP );
		points = go->getPoints();
		Debug::log(string("Obtaining goals"), LOOP );
		goals = go->getShape()->getGoals( points );
		double alpha = go->getShape()->getAlpha();

		for( int i = 0; i < points.size(); i++ ){

			if( !points[i]->move )
				continue;

			points[i]->pre = new Point(*(points[i]));// Copy
			vec x = points[i]->x;
			vec g = goals[i];
			vec f = points[i]->f;
			double m = points[i]->m;
			// Modified Euler
			points[i]->v += alpha*( g - x )/h + h*f/m;
			points[i]->x += h*points[i]->v;

			this->t += h;
		}
	}

	// Move Rigid bodies
	// TO-DO
}