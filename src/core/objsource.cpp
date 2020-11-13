#include "objsource.h"

using namespace morph::animats;

ObjectSource::ObjectSource( SoftBody *sb, int n ){
    this->tBody = sb;
    this->n = n;
}
    
    
SoftBody* ObjectSource::generate( int step ){
    return NULL;
}