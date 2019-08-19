class Simulation{
private:
	vector<Contact> contacts;
	vector<GeometricObject> objects;
	Solver solver;

	Simulation(){}
public:

	static Simulation* load( char** name );
	void run();
};