#pragma once
#include <vector>
#include "bodyset.h"

namespace morph{ namespace softmats{
/**
 * Position based dynamics loop 
 * 
 * Implements the general PBD loop. Bodies have local constraints (their shape), 
 * BodySets have global constraints (the collisions between bodies).
 * 
 * @author Alejandro Jimenez Rodriguez
 * @see Bender, J., Müller, M., & Macklin, M. (2017). A survey on position based dynamics, 2017. In Proceedings of the European Association for Computer Graphics: Tutorials (pp. 1-31).
 */
class PBD{
private:
    double h;
    // Sets up the constraints of bodies and body sets from the candidate positions
    void generateConstraints( BodySet *bs, int step );
    // One pass over all the constraints, moving the candidate positions
    void projectConstraints( BodySet *bs );
    // Lets the constraints know that the step has been integrated
    void velocityUpdate( BodySet *bs );
public:
    PBD();
    // Advances the simulation one time step
    void loop( BodySet *bs, int step );

};

}}
