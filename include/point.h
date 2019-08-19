class Point{
private:
	vec x;
	vec v;
	float m;

public:
	explicit Point( const vec x, float m):x(x), v(0.0), m(m);
	Point( const Point& p );
	Point& operator=( const Point& p ); // Copy assignment operator

	~Point();
};