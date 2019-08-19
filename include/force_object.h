class ForceObject{
private:
	ForceObject *nextObject;
public:
	ForceObject( ForceObject *fo );
	ForceObject *next();
	
	void applyForce( Simulation& s );
	// Abstract functions
	virtual void apply( ParticleSystem *ps ) = 0;
	virtual ~ForceObject();
};

// Concrete force objects
class GravityForce: public ForceObject{
public:
	GravityForce( ForceObject *fo );
	void apply( Simulation& s );

};