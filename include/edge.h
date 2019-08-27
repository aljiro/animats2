#ifndef EDGE_H
#define EDGE_H

namespace morph{ namespace animats{

class Edge{
public:
	Point* v0;
	Point* v1;

	Edge( Point* v0, Point* v1 );
	Point* getHead();
};

}

#endif
