class Point{
private:
	vec x;
	vec v;
	float m;

	bool move;

public:
	explicit Point( vec x, float m = 1.0 ):x(x), v(0.0), m(m);
	Point( const Point& p );
	Point& operator=( const Point& p ); // Copy assignment operator

	~Point();
};