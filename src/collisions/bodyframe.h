#pragma once
#include "../util/vec3.h"

namespace morph{ namespace softmats{
/**
 * A body seen as a whole by the collisions: its mass, centre and inertia.
 *
 * A contact acts mostly on the body, not on the vertex where it happens: the
 * vertex is held in place by the shape of the body, so pushing it moves and
 * rotates the body with it. The frame collects that rigid motion while the
 * collisions are solved, and it is applied to all the vertices afterwards.
 *
 * @see Müller, M., Macklin, M., Chentanez, N., Jeschke, S., & Kim, T. Y. (2020). Detailed rigid body simulation with extended position based dynamics.
 */
struct BodyFrame{
    double invMass; // 0 when the body cannot be moved as a whole
    double invInertia[3][3]; // About the centre, in world coordinates
    Vec3 centre;
    // Rigid motion not yet applied to the vertices: translation and small rotation
    Vec3 translation;
    Vec3 rotation;
    bool moved;

    BodyFrame():invMass(0.0), invInertia{{0,0,0},{0,0,0},{0,0,0}}, centre{0,0,0}, translation{0,0,0}, rotation{0,0,0}, moved(false){}

    Vec3 applyInvInertia( const Vec3& a ) const {
        return { invInertia[0][0]*a.x + invInertia[0][1]*a.y + invInertia[0][2]*a.z,
                 invInertia[1][0]*a.x + invInertia[1][1]*a.y + invInertia[1][2]*a.z,
                 invInertia[2][0]*a.x + invInertia[2][1]*a.y + invInertia[2][2]*a.z };
    }

    // Where a vertex is once the pending rigid motion is taken into account
    Vec3 positionOf( const Vec3& x ) const {
        return x + translation + cross( rotation, x - centre );
    }

    // How far the point at r from the centre moves along n per unit of impulse along n
    double inverseMassAt( const Vec3& r, const Vec3& n ) const {
        const Vec3 c = cross( r, n );
        return invMass + dot( c, applyInvInertia( c ) );
    }

    // Applies an impulse (in units of mass times displacement) at r from the centre
    void applyImpulse( const Vec3& r, const Vec3& impulse ){
        translation = translation + invMass*impulse;
        rotation = rotation + applyInvInertia( cross( r, impulse ) );
        moved = true;
    }
};

}}
