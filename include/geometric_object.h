class GeometricObject{
private:
	Shape *shape;
	vector<Point *> points;
public:

	GeometricObject( const GeometricObject& go ); // Copy constructor
	GeometricObject& operator=( const GeometricObject& go ); // Copy assignment operator
};