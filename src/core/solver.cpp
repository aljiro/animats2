
#include "solver.h"

using namespace morph::animats;

Solver::Solver( double h ):h(h), t(0.0){

}

void Solver::step( Simulation& s ){
	this->computeGoals( s );
	// External forces and reactions based upon the goals
	debugger.log(string("Computing external forces"), LOOP, "SOLVER" );
	s.computeExternalForces();

	// Integrate equations of motion
	for( SoftBody *go : s.getSoftBodies() ){
		this->integrateSoftBody( go );
	}

	this->t += h;
	// Move Rigid bodies
	// TO-DO
}

void Solver::integrateSoftBody( SoftBody *go ){
	debugger.log(string("Integrating softbody"), LOOP, "SOLVER" );
	vector<Point *> points = go->getPoints();	
	double alpha = go->getShape()->getAlpha();
	double alpha_c = 1.0;
	double m;	
	vec vc;
	double c = 0.05; // Friction of the elastic springs
	Point *p;

	for( int i = 0; i < points.size(); i++ ){
		p = points[i];
		vec x = p->x;
		vec g = p->g;
		vec f = p->f;
		// vec fc = p->fc;
		m = p->m;
		vc = zeros<vec>(3);

		if( p->state == Free )
			p->pre = new Point(*(p));
		
		if( p->state == Invalid )
			vc = alpha_c*(x - p->xc)/h;		
		
		p->v +=  -alpha*( x - g )/h +
				 h*f/m -c*p->v - vc ;
		p->x += h*p->v;		
	}

}

void Solver::computeGoals( Simulation& s ){
	vector<Point *> points;
	// Computing the goal positions
	for( SoftBody *go : s.getSoftBodies() ){
		points = go->getPoints();
		go->getShape()->setGoals( points );		
	}
}