#ifndef MESHES_H
#define MESHES_H

#include <vector>
#include <armadillo>
#include <cstdlib>

#include "util.h"
#include "gobject.h"
#include "edge.h"
#include "face.h"

using namespace arma;
using namespace std;

namespace morph{ namespace animats{

class GeometricObject;

class MeshProvider{
private:	

public:		
	//void addFace4( int p1, int p2, int p3, int p4 );
	void addFace3( GeometricObject*, int p1, int p2, int p3 );
	void addEdgeUnique( GeometricObject*, Edge& e );
	void collectEdges( GeometricObject* );
	/*
		Generates the triangulation of the polygon given by the polyhedron described by the given points
	*/
	virtual void populate( GeometricObject* go ) = 0;

};

class CloneMeshProvider: public MeshProvider{
private:
	GeometricObject *original;
public:
	CloneMeshProvider( GeometricObject *original);
	void populate( GeometricObject* go );
};


class PlaneMeshProvider : public MeshProvider{
private:	

public:

	PlaneMeshProvider( );
	void populate( GeometricObject* go );
	void fixNormal(GeometricObject*);

};


class ObjMeshProvider : public MeshProvider{
private:
	const char* path;
public:
	ObjMeshProvider( const char* path );
	void populate( GeometricObject* go );
};

class ObjMeshProccessChain{
private:
	ObjMeshProccessChain * next;
	virtual bool doProcess( FILE* f, char *s, GeometricObject* go ) = 0;
public:
	ObjMeshProccessChain( ObjMeshProccessChain *next );
	void process( FILE *f, char *s, GeometricObject* go );
};

class VertexChainLink : public ObjMeshProccessChain{
private:
	bool doProcess( FILE *f, char *s, GeometricObject* go );
public:
	VertexChainLink(ObjMeshProccessChain *next);
};

class TextureChainLink : public ObjMeshProccessChain{
private:
	bool doProcess( FILE *f, char *s, GeometricObject* go );
public:
	TextureChainLink(ObjMeshProccessChain *next);
};

class FaceChainLink : public ObjMeshProccessChain{
private:
	vector<vec> normals;
	bool doProcess( FILE *f, char *s, GeometricObject* go );
public:
	FaceChainLink(ObjMeshProccessChain *next);
};

}}
#endif