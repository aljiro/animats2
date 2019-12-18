
#include "../include/solver.h"

using namespace morph::animats;

Solver::Solver( double h ):h(h), t(0.0){

}

void Solver::step( Simulation& s ){
	vector<Point *> points;
	double c = 0.0;

	// Computing the goal positions
	for( SoftBody *go : s.getSoftBodies() ){
		points = go->getPoints();
		go->getShape()->setGoals( points );		
	}

	// External forces and reactions based upon the goals
	Debug::log(string("Computing external forces"), LOOP );
	s.computeExternalForces();
	int mp = 0;

	// Evolve speeds
	for( SoftBody *go : s.getSoftBodies() ){
		Debug::log(string("Updating soft body"), LOOP );
		points = go->getPoints();	
		double alpha = go->getShape()->getAlpha();

		Debug::log(string("Evolving points"), LOOP );
		vec vi;

		for( int i = 0; i < points.size(); i++ ){
			vec x = points[i]->x;
			vec g = points[i]->g;
			vec f = points[i]->f;
			double m = points[i]->m;
			
			points[i]->pre = new Point(*(points[i]));
			
			if( points[i]->move )
				points[i]->v += -alpha*( x - g )/h + h*f/m;//- alpha*points[i]->vc/(h);
			points[i]->vc = zeros<vec>(3);			
			

		}

		go->dx = zeros<vec>(3);
	}

	cout << "(Before)Total number of points unable to move: " << mp << endl;
	mp = 0;
	// Free points that will separate to move in this step
	Debug::log(string("Prunning contacts"), LOOP );
	s.collisionMgr.pruneContacts();

	Debug::log(string("Moving points"), LOOP );

	// Move soft bodies
	for( SoftBody *go : s.getSoftBodies() ){
		points = go->getPoints();

		for( int i = 0; i < points.size(); i++ ){			

			// Not move points that aren't allowed
			if( !points[i]->move ){
				mp++;
				continue;
			}
			
			points[i]->x += h*points[i]->v;			
		}

		Debug::log(string("Euler step done!"), LOOP );
	}

	cout << "(After)Total number of points unable to move: " << mp << endl;
	// cin.get();
	this->t += h;
	// Move Rigid bodies
	// TO-DO
}