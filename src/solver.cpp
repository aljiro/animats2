#include "solver.h"

explicit Solver::Solver( double h = 0.01 ):h(h){

}

void Solver::step( Simulation& s ){
	vector<Point *> points;
	vector<vec> goals;

	s.computeExternalForces();

	for( GeometricObject *go : s.objects ){
		points = go->getPoints();
		goals = go->shape->getGoals( points );
		double alpha = go->shape->alpha;

		for( int i = 0; i < p.size(); i++ ){
			vec x = points[i].x;
			vec g = goals[i];
			vec f = points[i].f;
			double m = points[i].m;
			// Modified Euler
			p->v += alpha*( g - x )/h + h*f/m;
			p->x += h*p->v;
		}
	}
}