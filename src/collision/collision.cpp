#include "collision.h"

using namespace morph::animats;

// Collision List
CollisionList::CollisionList(){

}

void CollisionList::push( Collision* c ){
    vector<Collision *>::iterator it;
    // cout << "Number of collisions before: " << collisions.size() << endl;
    if( collisions.empty() )collisions.push_back( c );
    else{
        for( it = collisions.begin(); it != collisions.end(); ++it  ){
            if( (*it)->hc > c->hc ){
                collisions.insert( it, c );
                break;
            }
        }
    }

    // cout << "collision list: ";
    // for( it = collisions.begin(); it != collisions.end(); ++it  )
    //     cout << (*it)->hc << ", ";
    // cout << endl;

    // cout << "Number of collisions after: " << collisions.size() << endl;
    // cin.get();
}

int CollisionList::count(){
    return collisions.size();
}

bool CollisionList::isEmpty(){
    return collisions.empty();
}

Collision* CollisionList::pop(){
    // cout << "Poping!" << collisions.size() << endl;
    if( !this->isEmpty() ){
        // cout << "Getting collision" << endl;
        Collision *c = this->collisions[0];
        this->collisions.erase( collisions.begin() );

        return c;
    }

    return NULL;
}

void CollisionList::discount( double hc ){
    for( Collision *c : collisions )
        c->hc -= hc;
}

// Collision
Collision::Collision( double hc ):hc(hc){

}

void Collision::updatePointSpeed( Point *p, vec imp ){
	if( p->state != Colliding ){
		p->v_half = 0*imp;
		p->state = Colliding;
	}else{
		p->v_half = 0*imp;
		
	}
	
    // p->interactions++;
}

// FPCollision
FPCollision::FPCollision( double hc, Face *f, Point *p, vec w ):Collision(hc),f(f),p(p),w(w){
    this->ctype = 0;
}

void FPCollision::resolve(){
    // set velocities - inelastic collision
		debugger.log(string("Face-point collision detected. Computing colliding impulses"), LOOP, "COLLISION");
		vector<vec> c_imp = getCollisionImpulses( f, p, &w );			

		//if( cf.go->type != PLANE ){
        for( int i = 0; i < 3; i++ ){
            this->updatePointSpeed( f->points[i], c_imp[i] );
        }
		//}
	
		//if( cp.go->type != PLANE ){
        this->updatePointSpeed( p, c_imp[3] );
		//}	
}

// EECollision
EECollision::EECollision( double hc, Edge *e1, Edge *e2 ):Collision(hc), e1(e1), e2(e2){
     this->ctype = 1;
}
    
    
void EECollision::resolve(){
    // change velocities - inelastic collision
    debugger.log(string("Edge-edge collision detected. Computing edge-edge colliding impulses"), LOOP, "COLLISION");
    vector<vec> c_imp = getCollisionImpulses( e1, e2 );
    // cout << "Modifying the v_halfs" << endl;
    // if( cp.go->type != PLANE ){
        this->updatePointSpeed( e1->v0, c_imp[0] );
        this->updatePointSpeed( e1->v1, c_imp[1] );
    // }
    // if( cf.go->type != PLANE ){
        this->updatePointSpeed( e2->v0, c_imp[2] );
        this->updatePointSpeed( e2->v1, c_imp[3] );
    // }
}