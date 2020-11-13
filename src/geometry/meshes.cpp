#include "meshes.h"

using arma::vec;
using arma::mat;
using namespace morph::animats;

void MeshProvider::addFace3( GeometricObject* go, int p1, int p2, int p3 ){
	//cout << "Adding face: "<< p1 << ", " << p2 << ", " << p3<< ": " << go->getPoints().size()<<endl;
	go->addFace( p1, p2, p3 );	
}

// void MeshProvider::addEdgeUnique( GeometricObject* go, Edge& e ){
// 	bool in_vector = false;

// 	for( int i = 0; i < go->edges.size(); i++ ){
// 		Edge edge = go->edges[i];
// 		if( e.v0 == edge.v0 && e.v1 == edge.v1 )
// 			in_vector = true;
// 	}

// 	if( !in_vector )
// 		go->edges.push_back( e );
// }

// void MeshProvider::collectEdges( GeometricObject* go ){
// 	for( int i = 0; i < go->faces.size(); i++ ){		

// 		for( int j = 0; j < go->faces[i]->edges.size(); j++ )
// 			this->addEdgeUnique(go, go->faces[i]->edges[j]);
// 	} 
// }


// CloneMeshProvider
CloneMeshProvider::CloneMeshProvider( GeometricObject* original ){
	this->original = original;	
}

void CloneMeshProvider::populate( GeometricObject* go ){
	for( Point *p : go->getPoints()  ){
		go->addPoint( p->x);
	}

	for( Face *f : go->getFaces() ){
		go->addFace( f->indexes[0], f->indexes[1], f->indexes[2]);
	}
}

// PlaneSkin
PlaneMeshProvider::PlaneMeshProvider(){

}

void PlaneMeshProvider::populate( GeometricObject* go ){

	vec v1 = {1, 1, 0};
	vec v2 = {1, -1, 0};
	vec v3 = {-1, -1, 0};
	vec v4 = {-1, 1, 0};
	
	go->addPoint( v1 );
	go->addPoint( v2 );
	go->addPoint( v3 );
	go->addPoint( v4 );

	this->addFace3( go, 0, 1, 2 );
	this->addFace3( go, 0, 2, 3 );
	// this->collectEdges(go);
	this->fixNormal(go);

}

void PlaneMeshProvider::fixNormal(GeometricObject* go){
	vec u = {0, 0, 1}; // THIS IS GOIN TO FAIL FOR NON-HORIZONTAL

	for(Face *f : go->getFaces()){
		f->normal = u;
		f->recompute = false;
	}
}

// ObjMeshProvider
ObjMeshProvider::ObjMeshProvider( const char* path ){
	this->path = path;
}


void ObjMeshProvider::populate( GeometricObject* go ){
	FILE *f = fopen( this->path, "r" );
	ObjMeshProccessChain *chain = new VertexChainLink( 
								  new TextureChainLink(
								  new FaceChainLink(NULL))); 

	if( f == NULL ){
		cerr << "No mesh file present." << endl;
		exit(0);
		return;
	}

	char lineheader[128];

	while( fscanf( f, "%s", lineheader ) != EOF ){
		chain->process( f, lineheader, go );
	}

	fclose( f );
}

// ObjMeshProccessChain
ObjMeshProccessChain::ObjMeshProccessChain( ObjMeshProccessChain *next ){
	this->next = next;
}

void ObjMeshProccessChain::process( FILE *f, char *s, GeometricObject* go ){
	this->doProcess( f, s, go );

	if( this->next != NULL )
		this->next->process( f, s, go );
}

// VertexChainLink
VertexChainLink::VertexChainLink(ObjMeshProccessChain *next):ObjMeshProccessChain( next ){}

bool VertexChainLink::doProcess( FILE *f, char *s, GeometricObject* go ){
	if( strcmp( s, "v" ) == 0 ){

		float x, y, z;
		fscanf(f, "%f %f %f\n", &x, &y, &z );
		vec pos = { x, y, z };
		//cout << "Adding point: "<<printvec(pos)<<endl;
		go->addPoint( pos );
		return true;
	}

	return false;
}

// TextureChainLink
TextureChainLink::TextureChainLink(ObjMeshProccessChain *next):ObjMeshProccessChain( next ){}

bool TextureChainLink::doProcess( FILE *f, char *s, GeometricObject* go ){
	// do nothing 
	if( strcmp( s, "vt" ) == 0 ){	
		cout << "Not processing textures so far" << endl;
	}

	return false;
}

// FaceChainLink
FaceChainLink::FaceChainLink(ObjMeshProccessChain *next):ObjMeshProccessChain( next ){}

bool FaceChainLink::doProcess( FILE *f, char *s, GeometricObject* go ){
	if( strcmp( s, "vn" ) == 0 ){	
		float x, y, z;
		fscanf(f, "%f %f %f\n", &x, &y, &z );
		vec pos = { x, y, z };
		cout << "Adding normal: "<<printvec(pos)<<endl;
		this->normals.push_back( pos );
		return true;
	}

	if( strcmp( s, "f" ) == 0 ){
		int vi, vj, vk, ti, tj, tk, ni, nj, nk;	
		// int matches = fscanf(f, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vi, &ti, &ni, &vj, &tj, &nj, &vk, &tk, &nk );
		int matches = fscanf(f, "%d//%d %d//%d %d//%d\n", &vi, &ni, &vj, &nj, &vk, &nk );
	    // if (matches != 9){
	    //     cerr<<"Error in the format of the mesh file"<<endl;
	    //     return false;
	    // }
	    //cout << "Adding face: "<< vi << ", "<< vj << ", "<< vk <<endl;
	    go->addFace( --vi, --vj, --vk, normals[--nk] );

	    return true;
	}

	return false;
}

