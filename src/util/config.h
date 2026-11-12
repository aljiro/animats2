#pragma once
#include <string>
namespace morph{ namespace softmats{
/**
 * Global properties of the simulation. <Singleton>
 *
 * They are read from a JSON file with the entries "mesh" and "shaderPath", and
 * optionally "timeStep", "iterations", "damping", "contactDamping", "staticFriction",
 * "kineticFriction", "stiffness", "relaxationTime", "contactDistance" and "recoverySpeed".
 * 
 * @author Alejandro Jimenez Rodriguez
 */
class Config{
private:
    
protected:
    // Step size of the integration
    double timeStep;
    // Number of iterations of the PBD solver in each step
    int numIterations;
    // Fraction of the velocity lost per unit of time
    double damping;
    // Fraction of their relative velocity that bodies in contact lose per unit of time
    double contactDamping;
    // Coulomb friction coefficients of the contacts
    double staticFriction;
    double kineticFriction;
    // Pressure needed to push the surface of a body in by a unit of length. 0 is rigid
    double stiffness;
    // Time that a dent in the surface takes to give way or to recover
    double relaxationTime;
    // Distance at which the surfaces of two bodies are kept apart
    double contactDistance;
    // Speed at which bodies that start overlapping by more than their surfaces can give are separated
    double recoverySpeed;
    // Location of the mesh
    std::string meshLocation;
    // Location of the shader
    std::string shaderLocation;
    Config();
    ~Config();
    static Config *instance_;
public:
    static std::string configPath;
    static Config *getConfig();
    double getTimeStep();
    int getNumIterations();
    double getDamping();
    double getContactDamping();
    double getStaticFriction();
    double getKineticFriction();
    double getStiffness();
    double getRelaxationTime();
    double getContactDistance();
    double getRecoverySpeed();
    std::string getMeshLocation();
    std::string getShaderLocation();

};

}}