class Solver{
private:
	ForceObject forceChain;
	double h;
public:
	explicit Solver( double h = 0.01 );
	void step( Simulation& s );
};