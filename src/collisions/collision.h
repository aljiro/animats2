#ifndef COLLISION_H
#define COLLISION_H

#include <armadillo>
#include "../core/point.h"
#include "../core/face.h"
#include "../util/vec3.h"
#include "bodyframe.h"

namespace morph{ namespace softmats{

// Parameters of the contact response, shared by all the collisions in a step
struct ContactParams{
    // Coulomb friction coefficients
    double staticFriction;
    double kineticFriction;
    // Relaxation time of the surfaces, in time steps
    double relaxation;
};

// How the surfaces give way at a collision
struct ContactMaterial{
    // Displacement of the surfaces per unit of impulse: 1/(k h^2) for a contact of
    // stiffness k and a time step h. 0 when both bodies are rigid.
    double compliance;
    // Largest depth that the surfaces can give
    double limit;
    // Fraction of the depth taken by the surface of the vertex; the rest is taken by the other body
    double share;
    // Area of the surface that the collision stands for
    double area;
};

/**
 * A vertex of one body in contact with the surface of another body: either a
 * point on one of its faces, or the ground plane.
 *
 * The collision is a unilateral constraint on the candidate positions:
 *
 *     C = cnormal . (p - q) - rest >= 0
 *
 * where q is the contact point on the other body. The positions are those of
 * the bodies without dents, so -C is how far the surfaces are pressed into
 * each other. The surfaces resist it like a spring with a damper: the
 * constraint is solved with a compliance (see Macklin et al.), with an impulse
 * along the normal that grows with the depth. Beyond the depth that the
 * surfaces can give the constraint is rigid.
 *
 * The impulse is equal and opposite on the two bodies, so momentum is
 * conserved, and it acts on each body as a whole (see BodyFrame). Friction is
 * an impulse along the surface limited by the Coulomb cone.
 *
 * @author Alejandro Jimenez Rodriguez
 * @see Müller, M., Heidelberger, B., Hennix, M., & Ratcliff, J. (2007). Position based dynamics.
 * @see Macklin, M., Müller, M., & Chentanez, N. (2016). XPBD: position-based simulation of compliant constrained dynamics.
 */
class Collision{
public:
    Point* p; // Vertex in contact
    Face* f; // Face of the other body. nullptr when the other body is the ground
    double bary[3]; // Barycentric coordinates of the contact point in f
    arma::vec cnormal; // Unit normal, pointing from the other body towards p
    arma::vec cp; // Contact point on the surface of the other body
    double depth; // How far the surfaces are pressed into each other at the end of the step
    double gap; // Distance between the surfaces at the end of the step, negative when pressed
    double impulse; // Normal impulse applied during the step, in units of mass times displacement
    double area; // Area of the surface that the collision stands for
    bool active; // Whether the two bodies are touching at this collision

    // Collision of the point with a face of another body
    Collision( Point* p, BodyFrame* frame, Face* f, BodyFrame* faceFrame, const double bary[3], const Vec3& normal,
               double rest, double slack, const ContactMaterial& material );
    // Collision of the point with the ground
    Collision( Point* p, BodyFrame* frame, double groundHeight, double slack, const ContactMaterial& material );

    // Moves the candidate positions to satisfy the constraint and applies friction
    void solve( const ContactParams& params );
    // Updates the reported contact information once the step is finished
    void finish( double tolerance );
    // Displacement of the surface at the vertex, once the step is finished
    arma::vec dent() const;

private:
    BodyFrame* frame; // Body of the vertex
    BodyFrame* faceFrame; // Body of the face. nullptr for the ground
    Vec3 normal;
    Vec3 friction; // Friction impulse applied during the step
    double soft; // Part of the normal impulse applied through the compliance
    double rest; // Distance at which the surfaces are kept
    double slack; // Depth beyond the limit that is tolerated in this step, see CollisionConstraint
    double groundHeight;
    ContactMaterial material;

    // Current positions of the vertex and of the contact point on the other body
    Vec3 vertexPosition() const;
    Vec3 surfacePosition() const;
    // Displacement of the vertex relative to the contact point during this step
    Vec3 relativeDisplacement( const Vec3& x, const Vec3& q ) const;
    // Applies an impulse to the body of the vertex and the opposite one to the other body
    void applyImpulse( const Vec3& impulse, const Vec3& rp, const Vec3& rq );
};

}}
#endif // COLLISION
