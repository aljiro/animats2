
#include "solver.h"

using namespace morph::animats;

Solver::Solver( Environment* environment, double h ):h0(h), t(0.0){
	this->environment = environment;
}

void Solver::stepMaterial(){
	this->h = this->h0;
	this->computeGoals();
	// External forces and reactions based upon the goals
	debugger.log(string("Computing external forces"), LOOP, "SOLVER" );
	environment->computeExternalForces();

	// Integrate equations of motion
	for( SoftBody *go : environment->getSoftBodies() ){
		this->integrateSoftBody( go );
	}	
	// Move Rigid bodies
	// TO-DO
}

void Solver::integrateSoftBody( SoftBody *go ){
	debugger.log(string("Integrating softbody"), LOOP, "SOLVER" );
	vector<Point *> points = go->getPoints();	
	double alpha = go->getShape()->alpha;
	double m;
	Point *p;
	double c = 0.02;

	for( int i = 0; i < points.size(); i++ ){
		p = points[i];

		vec x = p->x;
		vec g = p->g;
		vec f = p->f;
		vec vi = alpha*( x - g )/h;


		p->v_c = p->v - vi +
				 h*f/p->m - c*p->v;
		p->x_c = p->x + h*p->v_c;

		p->v_half = (p->x_c - p->x)/h;
	}
}

void Solver::partialStep( double hc ){
	for( SoftBody *go : environment->getSoftBodies() ){
		vector<Point *> points = go->getPoints();	

		for( int i = 0; i < points.size(); i++ ){
			// if( points[i]->state == Free )
			points[i]->x += hc*points[i]->v_half;
		}
	}

	this->h -= hc;
}


void Solver::stepCollisions( ){
	// Iterate over the collision list updating the velocity
	// Iterate over the rest of the points doing the update
	
	double alpha;
	double c = 0.0;

	for( SoftBody *go : environment->getSoftBodies() ){
		vector<Point *> points = go->getPoints();	
		alpha = go->getShape()->alpha;

		for( int i = 0; i < points.size(); i++ ){
			// if( points[i]->state == Free )
			points[i]->x += h*points[i]->v_half;
		}

		// go->getShape()->computeGoals( points );

		for( int i = 0; i < points.size(); i++ ){			
			
			vec x = points[i]->x;
			vec g = points[i]->g;
			vec f = points[i]->f;

			if( points[i]->state == Free ){
			// No collisions
				points[i]->v = points[i]->v_c;
			}else{
			// Collision or proximity
			// 
				points[i]->v = points[i]->v_half;// - alpha*( x - g )/h  +  h*f/points[i]->m  - c*points[i]->v_half;
			}

			points[i]->state = Free;
			points[i]->interactions = 0;
		}
	}	


	this->t += h;
}

void Solver::computeGoals(){
	vector<Point *> points;
	// Computing the goal positions
	for( SoftBody *go : environment->getSoftBodies() ){
		points = go->getPoints();
		go->getShape()->computeGoals( points );		
	}
}