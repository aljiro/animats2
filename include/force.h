#ifndef FORCE_H
#define FORCE_H

#include "gobject.h"
#include "point.h"

namespace morph{
namespace animats{

/**
 * ForceObject - Abstraction of enviromental, external forces 
 * acting on the object.
 */
class ForceObject{
private:
	ForceObject *nextObject;
public:
	ForceObject( ForceObject *fo );
	ForceObject *next();
	void add( ForceObject *fo );
	

	void applyForce( GeometricObject* go );
	// Abstract functions
	// The nature of the force depends upon each concrete
	// Force object
	virtual void apply( GeometricObject* go ) = 0;
	//virtual ~ForceObject();
};

// Concrete force objects
/**
 * GravityForce - ForceObject corresponding to the force of gravity
 */
class GravityForce: public ForceObject{
public:
	GravityForce( ForceObject *fo );
	void apply( GeometricObject* go );

};

}}

#endif