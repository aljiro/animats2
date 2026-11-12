#pragma once
#include <vector>
#include "../core/constraint.h"
#include "../core/body.h"
#include "../core/bodyset.h"
#include "../core/point.h"
#include "../core/face.h"
#include "../util/vec3.h"

#include "spatialhash.h"
#include "bodyframe.h"
#include "collision.h"
#include "contactlist.h"

namespace morph{ namespace softmats {
class BodySet;
/**
 * Collision detection and response between all the bodies of a BodySet.
 *
 * At each step the candidate positions are searched for vertices that are
 * touching or inside another body. Each one becomes a Collision, a constraint
 * that the solver projects together with the shape of the bodies.
 *
 * Detection is based on the current state, not on crossing events: a vertex is
 * in contact for as long as it is within the contact distance of the surface
 * of the other body, on either side of it. The closest face is found with a
 * spatial hash, and the side with the outward normals of that body, so the
 * meshes must be closed and are assumed to be roughly convex.
 *
 * The ground is treated as the half space below its height.
 *
 * The response to a collision acts on the bodies as a whole, see BodyFrame.
 *
 * Bodies are soft: their surfaces give way under pressure, in proportion to
 * their stiffness. Each vertex stands for the area of the surface around it,
 * so the force between two bodies depends on how much of their surfaces is
 * pressed together and not on how fine their meshes are. The collisions are
 * found between the shapes without dents, and the dents are added to the
 * vertices at the end of the step.
 *
 * @author Alejandro Jimenez Rodriguez
 */
class CollisionConstraint : public Constraint{
private:
    // A face in terms of the indices of its vertices in the body
    struct FaceEntry{
        Face* face;
        int v[3];
    };

    // A registered body, with the data computed for it on each step
    struct BodyEntry{
        Body* body;
        std::vector<Point*>* points;
        std::vector<FaceEntry> faces;
        int offset; // Index of its first point among all the registered points
        std::vector<double> area; // Area of the surface around each point
        double give; // Largest depth of a dent, when the body is soft
        // Per step data
        std::vector<Vec3> xc; // Candidate positions
        std::vector<Vec3> normals; // Outward vertex normals at the candidate positions
        Vec3 boxMin, boxMax;
        bool near; // Is it close to any other body?
        bool colliding; // Does it take part in any collision?
        BodyFrame frame;
    };

    // The closest face of a body to a vertex of another one
    struct Candidate{
        int body;
        int face;
        double dist2;
        double bary[3];
        Vec3 closest;
    };

    std::vector<BodyEntry> entries;
    // Body and index within the body of each registered point
    std::vector<int> pointBody;
    std::vector<int> pointIndex;
    // Candidates found for each registered point in the current step
    std::vector<std::vector<Candidate>> candidates;
    std::vector<int> touched;

    SpatialHash hash;
    BodySet *bodySet;
    ContactList *contacts;
    Body *ground;
    double groundHeight;
    // Average length of the edges of the meshes, which sets the scale of the search
    double edgeLength;
    // Largest depth found in the last step, which the search has to reach
    double deepest;

    // How the surfaces give way where a vertex of a body presses on another body or on the ground
    ContactMaterial material( const BodyEntry& a, int vertex, const BodyEntry* b, double h ) const;

    void registerObject( Body *b );
    void updatePositions( BodyEntry& e );
    void updateNormals( BodyEntry& e );
    void updateFrame( BodyEntry& e );
    void applyFrame( BodyEntry& e );
    void findGroundCollisions( double margin, double h );
    void findCandidates( double radius );
    void storeCollisions( double rest, double margin, double h );
    
public:
    // Inherited constraints methods
    void init( BodySet *bs );
    void generate( int step = 0 );
    void solve();
    void updateVelocity();
    void reset();

    ContactList* getContacts();

    CollisionConstraint();
    ~CollisionConstraint();
};

}}
