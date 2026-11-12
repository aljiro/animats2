#pragma once
#include "constraint.h"
#include <vector>
#include "body.h"
#include <armadillo>

namespace morph{ namespace softmats{
/**
 * Shape matching algortithm
 * 
 * This class manages the shape matching constraint on the shape of each body. It keeps
 * a copy of the original shape and pulls the points towards the rigid transformation
 * of it that best fits their current positions.
 * TO-DO: Add clusters and other optimizations
 * 
 * @author Alejandro Jimenez Rodriguez
 * @see Müller, M., Heidelberger, B., Teschner, M., & Gross, M. (2005). Meshless deformations based on shape matching. ACM transactions on graphics (TOG), 24(3), 471-478.
 */
    class ShapeMatchingContraint : public Constraint{
    private:
        // Stiffness: fraction of the way to the goal positions that the points
        // are moved in one time step. 1 is rigid.
        double alpha;
        // The extent to which linear deformations (stretch, shear) of the original shape are allowed
        double beta;
        // Precomputed inverse
        arma::mat Aqqi;
        // Original center of mass
        arma::vec x0_cm;
        // Transformation
        arma::mat T;
        // Original shape: positions relative to the center of mass, and masses
        std::vector<arma::vec> shape;
        std::vector<double> mass;
        // A reference to the body whos shape is to be matched
        Body *body;

        // Center of mass of the candidate positions
        arma::vec computeCM();
        // Compute the required transformation for the goal points
        void computeMatrices( const arma::vec& x_cm );
        // Precomputation of the inverse matrix
        void precompute( );
    public:
        // --- Inherited constraint methods ----
        void init( Body *b );
        void generate(  int step = 0);
        void solve();
        void updateVelocity();
        void reset();

        ShapeMatchingContraint( double alpha );
        ~ShapeMatchingContraint();
    };

}}
