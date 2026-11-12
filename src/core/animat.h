#pragma once

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "body.h"
#include "point.h"
#include "face.h"

namespace morph{ namespace softmats{
/**
 * Represets an Animat body
 * 
 * @author Alejandro Jimenez Rodriguez
 */
class Animat : public Body{
private:  
    void init(int);    
public:
    Animat( float x, float y, float z );    
    
    void setMass( double m );
    // Adds the shape matching constraint. The stiffness goes from 0 to 1 (rigid)
    void setConstraints( double stiffness = 0.2 );
    void addGroundImpulse( arma::vec f );
    // Pushes the animat in the given direction while it is on the ground
    void move( double x, double y, double z );
    
};

}}