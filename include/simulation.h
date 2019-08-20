class Simulation{
private:
	vector<Contact *> contacts;
	vector<GeometricObject *> objects;
	Solver solver;
	ForceObject *forceChain;

	Simulation(){}
public:

	static Simulation* load( char** name );
	void computeExternalForces();
	void run();
};