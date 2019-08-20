#include "solver.h"

explicit Solver::Solver( double h = 0.01 ):h(h), t(0.01){

}

void Solver::step( Simulation *s ){
	vector<Point *> points;
	vector<vec> goals;

	s.computeExternalForces();

	// Move soft bodies
	for( SoftBody *go : s.softBodies ){
		points = go->getPoints();
		goals = go->getShape()->getGoals( points );
		double alpha = go->getShape()->alpha;

		for( int i = 0; i < p.size(); i++ ){

			if( !points[i].move )
				continue;

			vec x = points[i].x;
			vec g = goals[i];
			vec f = points[i].f;
			double m = points[i].m;
			// Modified Euler
			p->v += alpha*( g - x )/h + h*f/m;
			p->x += h*p->v;
			this->t += h
		}
	}

	// Move Rigid bodies
	// TO-DO
}