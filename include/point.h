class Point{
private:
	vec x;
	vec v;
	float m;
	float f;

	bool move;

public:
	explicit Point( vec x, float m = 1.0 ):x(x), v(zeros<vec>(3)), m(m), f(zeros<vec>(3));
	Point( const Point& p );
	Point& operator=( const Point& p ); // Copy assignment operator

	~Point();
};