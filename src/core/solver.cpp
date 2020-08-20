
#include "solver.h"

using namespace morph::animats;

Solver::Solver( double h ):h(h), t(0.0){

}

void Solver::stepMaterial( Simulation& s ){
	this->computeGoals( s );
	// External forces and reactions based upon the goals
	debugger.log(string("Computing external forces"), LOOP, "SOLVER" );
	s.computeExternalForces();

	// Integrate equations of motion
	for( SoftBody *go : s.getSoftBodies() ){
		this->integrateSoftBody( go );
	}	
	// Move Rigid bodies
	// TO-DO
}

void Solver::integrateSoftBody( SoftBody *go ){
	debugger.log(string("Integrating softbody"), LOOP, "SOLVER" );
	vector<Point *> points = go->getPoints();	
	double alpha = go->getShape()->getAlpha();
	double alpha_c = 1.0;
	double m;
	Point *p;

	for( int i = 0; i < points.size(); i++ ){
		p = points[i];

		if( p->state == Free )
			p->pre = new Point(*(p));

		vec x = p->x;
		vec g = p->g;
		vec f = p->f;
		double m = p->m;

		p->vc += -alpha*( x - g )/h +
				 h*f/m ;
		p->xc += h*p->v;
	}
}

void Solver::stepCollisions( Simulation& s ){
	this->t += h;
}

void Solver::computeGoals( Simulation& s ){
	vector<Point *> points;
	// Computing the goal positions
	for( SoftBody *go : s.getSoftBodies() ){
		points = go->getPoints();
		go->getShape()->setGoals( points );		
	}
}