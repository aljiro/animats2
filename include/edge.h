#ifndef EDGE_H
#define EDGE_H


namespace morph{ namespace animats{

class Edge{
public:
	PointMass* v0;
	PointMass* v1;

	Edge( PointMass* v0, PointMass* v1 );
	PointMass* getHead();
};

}

#endif
