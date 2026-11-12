#pragma once
#include <glm/glm.hpp>
#include <armadillo>
#include <vector>
#include <algorithm>

namespace morph{ namespace softmats{
/**
 * Point data structure
 * 
 * Note that the point has a current position (x) and a candidate position (x_c) 
 * to which constraints are applied. The current position is where the surface of
 * the body is, dents included; the constraints work on the positions without dents.
 * 
 * @author Alejandro Jimenez Rodriguez
 */
typedef struct Point{
    arma::vec x; // current position
    arma::vec v; // current velocity
    arma::vec normal; // current normal
    arma::vec x_c; // candidate position
    arma::vec fext; // external force
    // Displacement of the surface at this point where another body presses on it. It
    // is included in x, and set by the collisions at the end of each step.
    arma::vec dent;
    glm::vec2 uv; // texture coordinates
    
    bool lock; // When locked, the point is not moved by the solver
    double w; // Inverse mass
    double temp_receptor;
    bool ground_receptor;
    std::vector<Point *> adj; // Adjacent points in the mesh

    Point(){ 
        this->x = {0.0, 0.0, 0.0};
        this->x_c = {0.0, 0.0, 0.0};
        this->v = {0.0, 0.0, 0.0};
        this->normal = {0.0, 0.0, 0.0};
        this->fext = {0.0, 0.0, 0.0};
        this->dent = {0.0, 0.0, 0.0};
        this->uv = glm::vec2(0.0f, 0.0f);
        this->lock = false;
        this->w = 1.0;
        this->temp_receptor = 0.0;
        this->ground_receptor = false;
    }

    // Inverse mass seen by the constraints: a locked point behaves as infinitely heavy
    double invMass() const {
        return lock ? 0.0 : w;
    }

    void addAdjacent( Point *p ){
        if( std::find( adj.begin(), adj.end(), p ) == adj.end())
            adj.push_back(p);
    }
} Point;

}}
