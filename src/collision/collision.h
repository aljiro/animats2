#ifndef COLLISION_H
#define COLLISION_H

#include <armadillo>
#include <vector>
#include "../geometry/face.h"
#include "../geometry/point.h"
#include "../utilities/computationalg.h"

using namespace std;
using namespace arma;

namespace morph{ namespace animats{

class Collision{
public:
    double hc;
    int ctype;

    Collision( double hc );
    void updatePointSpeed( Point *p, vec imp );
    virtual void resolve() = 0;
};

class FPCollision : public Collision{
public:
    Face *f;
    Point *p;
    vec w;

    FPCollision( double hc, Face *f, Point *p, vec w );
    void resolve();
};

class EECollision : public Collision{
public:
    Edge *e1;
    Edge *e2;

    EECollision( double hc, Edge *e1, Edge *e2 );
    void resolve();
};

class CollisionList{
private:
    
public:
vector<Collision *> collisions;
    CollisionList();

    void push( Collision* c );
    int count();
    bool isEmpty();
    Collision* pop();
    void discount( double hc );
};
}}
#endif // COLLISION