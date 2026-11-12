#include "trianglemesh.h"
#include "../util/calg.h"

using namespace morph::softmats;

TriangleMesh::TriangleMesh(){

}

TriangleMesh::~TriangleMesh(){
    for( Point *p : vertices )
        delete p;

    for( Face *f : faces )
        delete f;
}

void TriangleMesh::computeNormals( bool candidate ){
    arma::vec x1, x2, x3;
    
    for( Face* f : faces ){
        if( candidate ){      
            x1 = f->points[0]->x_c;
            x2 = f->points[1]->x_c;
            x3 = f->points[2]->x_c;
            f->normal_c = arma::cross( x2 - x1, x3 - x1 );
            f->normal_c /= arma::norm(f->normal_c); 
        }else{
            x1 = f->points[0]->x;
            x2 = f->points[1]->x;
            x3 = f->points[2]->x;
            f->normal = arma::cross( x2 - x1, x3 - x1 );
            f->normal /= arma::norm(f->normal);
        }
        
    }
}

void TriangleMesh::updateVertexNormals(){
    for( Point *p : vertices ){
        p->normal = arma::zeros<arma::vec>(3);
    }

    for( Face *f : faces ){
        for( Point *p : f->points ){
            p->normal += f->normal;               
        }
    }

    for( Point *p : vertices ){
        p->normal /= arma::norm(p->normal);
    }
}

std::vector<Point *>& TriangleMesh::getVertices(){
    return vertices;
}

std::vector<Face *>& TriangleMesh::getFaces(){
    return faces;
}

int TriangleMesh::getNumVertices(){
    return vertices.size();
}

void TriangleMesh::translate( float x, float y, float z ){
    arma::vec t = {x, y, z};

    for( Point* p : vertices ){
        p->x += t;
        p->x_c += t;
    }
}

void TriangleMesh::scale( float s ){
    for( Point* p : vertices ){
        p->x *= s;
        p->x_c *= s;
    }
}

void TriangleMesh::center(){
    arma::vec cm = centroid( this->vertices );

    for( Point *p : vertices ){
        p->x -= cm;
        p->x_c -= cm;
    }
}

void TriangleMesh::orientOutwards(){
    // Signed volume enclosed by the mesh: negative when the faces are wound inwards
    double volume = 0.0;

    for( Face *f : faces )
        volume += arma::dot( f->points[0]->x, arma::cross( f->points[1]->x, f->points[2]->x ) );

    if( volume < 0 ){
        for( Face *f : faces )
            std::swap( f->points[1], f->points[2] );
    }
}
