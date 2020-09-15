#ifndef FORCE_H
#define FORCE_H

#include "gobject.h"
#include "point.h"

namespace morph{
namespace animats{

class ForceObject{
private:
	ForceObject *nextObject;
public:
	ForceObject( ForceObject *fo );
	ForceObject *next();
	void add( ForceObject *fo );
	
	void applyForce( GeometricObject* go );
	// Abstract functions
	virtual void apply( GeometricObject* go ) = 0;
	//virtual ~ForceObject();
};

// Concrete force objects
class GravityForce: public ForceObject{
public:
	GravityForce( ForceObject *fo );
	void apply( GeometricObject* go );

};

class HForce: public ForceObject{
public:
	HForce( ForceObject *fo );
	void apply( GeometricObject* go );

};

}}

#endif