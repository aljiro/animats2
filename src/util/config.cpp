#include "config.h"
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>

using namespace morph::softmats;

std::string Config::configPath = ".";
Config* Config::instance_ = nullptr;

namespace {

// Reads the entries of a flat JSON object whose values are strings or numbers:
// { "key" : "value", "other" : 1.5, ... }. That is all the configuration file
// contains, so no JSON library is needed. Numbers are kept as text.
bool readEntries( const std::string& path, std::map<std::string, std::string>& entries ){
    std::ifstream file( path );

    if( !file.is_open() )
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    const std::string text = buffer.str();

    const std::regex entry( "\"([^\"]+)\"\\s*:\\s*(?:\"([^\"]*)\"|([-+0-9.eE]+))" );

    for( std::sregex_iterator it( text.begin(), text.end(), entry ), end; it != end; ++it )
        entries[(*it)[1].str()] = (*it)[2].matched ? (*it)[2].str() : (*it)[3].str();

    return true;
}

// Paths in the configuration file may be relative to the file itself.
std::string resolvePath( const std::string& configFile, const std::string& path ){
    if( path.empty() || path[0] == '/' )
        return path;

    const size_t slash = configFile.find_last_of( '/' );

    if( slash == std::string::npos )
        return path;

    return configFile.substr( 0, slash + 1 ) + path;
}

}

Config* Config::getConfig(){
    if( instance_ == nullptr ){
        instance_ = new Config();
    }

    return instance_;
}

Config::Config():timeStep(0.01), numIterations(4), damping(0.0), contactDamping(0.0), staticFriction(0.6),
                 kineticFriction(0.5), stiffness(5e4), relaxationTime(0.1), contactDistance(0.01), recoverySpeed(1.0){
    std::map<std::string, std::string> entries;

    if( !readEntries( configPath, entries ) ){
        std::cerr << "Configuration file " << configPath << " not found\n";
        return;
    }

    this->meshLocation = resolvePath( configPath, entries.count("mesh") ? entries["mesh"] : "." );
    this->shaderLocation = resolvePath( configPath, entries.count("shaderPath") ? entries["shaderPath"] : "." );

    // The shader file names are appended directly to this location
    if( !shaderLocation.empty() && shaderLocation.back() != '/' )
        shaderLocation += '/';

    const auto number = [&entries]( const char* key, double fallback ){
        try{
            return entries.count(key) ? std::stod( entries[key] ) : fallback;
        }catch( const std::exception& ){
            std::cerr << "Ignoring the value of " << key << " in the configuration file\n";
            return fallback;
        }
    };

    timeStep = number( "timeStep", timeStep );
    numIterations = std::max( 1, static_cast<int>( number( "iterations", numIterations ) ) );
    damping = number( "damping", damping );
    contactDamping = number( "contactDamping", contactDamping );
    staticFriction = number( "staticFriction", staticFriction );
    kineticFriction = number( "kineticFriction", kineticFriction );
    stiffness = std::max( 0.0, number( "stiffness", stiffness ) );
    relaxationTime = std::max( 0.0, number( "relaxationTime", relaxationTime ) );
    contactDistance = number( "contactDistance", contactDistance );
    recoverySpeed = number( "recoverySpeed", recoverySpeed );

    std::cout << "config location: " << configPath << "\n";
    std::cout << "Mesh location: " << meshLocation << "\n";
    std::cout << "Shader location: " << shaderLocation << "\n";
}

Config::~Config(){

}

std::string Config::getMeshLocation(){
    return this->meshLocation;
}

std::string Config::getShaderLocation(){
    return this->shaderLocation;
}

double Config::getTimeStep(){
    return timeStep;
}

int Config::getNumIterations(){
    return numIterations;
}

double Config::getDamping(){
    return damping;
}

double Config::getContactDamping(){
    return contactDamping;
}

double Config::getStaticFriction(){
    return staticFriction;
}

double Config::getKineticFriction(){
    return kineticFriction;
}

double Config::getStiffness(){
    return stiffness;
}

double Config::getRelaxationTime(){
    return relaxationTime;
}

double Config::getContactDistance(){
    return contactDistance;
}

double Config::getRecoverySpeed(){
    return recoverySpeed;
}
