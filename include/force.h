#ifndef FORCE_H
#define FORCE_H

#include "simulation.h"

namespace morph{
namespace Animats{

class ForceObject{
private:
	ForceObject *nextObject;
public:
	ForceObject( ForceObject *fo );
	ForceObject *next();
	
	void applyForce( GeometricObject* go );
	// Abstract functions
	virtual void apply( GeometricObject* go ) = 0;
	virtual ~ForceObject();
};

// Concrete force objects
class GravityForce: public ForceObject{
public:
	GravityForce( ForceObject *fo );
	void apply( GeometricObject* go );

};

}}

#endif