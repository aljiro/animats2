#ifndef OBJSOURCE_H
#define OBJSOURCE_H

#include "../geometry/gobject.h"

using namespace std;

namespace morph{ namespace animats{
class ObjectSource{
private:
    double r;
    int n;
    int count;
    SoftBody *tBody;
public:
    ObjectSource( SoftBody *sb, int n );
    SoftBody* generate( int step );
};
}}

#endif