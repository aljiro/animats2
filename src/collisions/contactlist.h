#pragma once
#include <vector>
#include "contact.h"

namespace morph{ namespace softmats{
/**
 * The contacts between all the pairs of bodies in the simulation
 * 
 * @author Alejandro Jimenez Rodriguez
 */
class ContactList{
private:
    // Owned by the list
    std::vector<Contact*> contacts;
public:
    
    ContactList();
    ~ContactList();

    Contact* findContact( Body* A, Body* B );
    // Adds a collision between the two bodies. The list takes ownership of it
    void push( Body* A, Body* B, Collision* c );
    int count();
    bool isEmpty();
    void clear();
    std::vector<Contact*>& getContacts();
    // Removes the collisions that are not active and the contacts left empty
    void prune();
    double getContactArea( bool includeFloor );
    void updateReceptors();
    void print();
};

}}
