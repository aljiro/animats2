#pragma once
#include <iostream>
#include <vector>
#include "collision.h"
#include "../core/body.h"

namespace morph{ namespace softmats{

/**
 * All the collisions between a pair of bodies
 * 
 * @author Alejandro Jimenez Rodriguez
 */
class Contact{
private:
    Body *A;
    Body *B;
    // Owned by the contact
    std::vector<Collision *> collisions;
public:

    Contact( Body *A, Body *B );
    ~Contact();
    /**
     * Adds a new collision to the contact, which takes ownership of it
     * 
     * @param c - new collision, not null
     */
    void addCollision( Collision* c );
    Body* getA();
    Body* getB();
    void clearCollisions();
    bool hasCollisions();
    // Area of the surface over which the two bodies are pressed together
    double getContactArea( bool includeFloor );
    void clearInactiveCollisions();
    std::vector<Collision*>& getCollisions();
    void updateReceptors();
    void print();
};

}}
