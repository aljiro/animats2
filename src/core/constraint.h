#pragma once

namespace morph{ namespace softmats{


class Constraint{
/**
 * General constraint interface for Bodies and BodySets
 * 
 * @author Alejandro Jimenez Rodriguez
 */
public:
    // Sets up the constraint on each step, from the candidate positions
    virtual void generate( int step = 0 ) = 0;
    // Moves the candidate positions towards satisfying the constraint.
    // Called once per solver iteration.
    virtual void solve() = 0;
    // Called once the step has been integrated
    virtual void updateVelocity() = 0;
    // Resets the constraint
    virtual void reset() = 0;
    virtual ~Constraint(){}
};

}}
