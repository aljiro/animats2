#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "face.h"
#include "point.h"

namespace morph{ namespace softmats{

class TriangleMesh{
 /**
 * Mesh data structure
 * 
 * Represents a mesh for a given body. This implementation can be made more
 * efficient through a half-edge implementation (TO-DO).
 * 
 * @author Alejandro Jimenez Rodriguez
 */
private:
    // Triangular faces
    std::vector<Face*> faces;
    // Vertices
    std::vector<Point*> vertices;
public:

    TriangleMesh();

    /**
     * Recomputes the normals of the faces.
     * 
     * @param candidate - If true, the normals are computed for the candidate position
     * @see pbdim.h
     */
    void computeNormals( bool candidate );
    // Updates the vertex normals based upon the face normals
    void updateVertexNormals();

    // getters
    std::vector<Point *>& getVertices();
    std::vector<Face *>& getFaces();
    int getNumVertices();

    // Mesh wide Transformations
    void translate( float x, float y, float z );
    void scale( float s );
    void center();
    // Makes the normals of a closed mesh point outwards, reordering the vertices of the faces if needed
    void orientOutwards();

    ~TriangleMesh();
};

}}