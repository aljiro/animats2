#include "collision.h"
#include "contactlist.h"
#include "contact.h"
#include "../util/vec3.h"

using namespace morph::softmats;

// Contact
Contact::Contact(Body* A, Body* B ):A(A), B(B){
}

Contact::~Contact(){
    clearCollisions();
}

Body* Contact::getA(){
    return A;
}

Body* Contact::getB(){
    return B;
}

double Contact::getContactArea( bool includeFloor ){
    if( !includeFloor && (A->type==BodyType::GROUND || B->type == BodyType::GROUND ) )
        return 0.0;

    double area = 0.0;

    for( Collision *c : collisions )
        area += c->area;

    return area;
}

bool Contact::hasCollisions(){
    return !collisions.empty();
}

void Contact::addCollision( Collision* c  ){
    collisions.push_back( c );
}

void Contact::updateReceptors(){
    Body *animat = nullptr;

    if( A->type == BodyType::GROUND && B->type == BodyType::ANIMAT )
        animat = B;
    else if( B->type == BodyType::GROUND && A->type == BodyType::ANIMAT )
        animat = A;

    if( animat != nullptr ){
        for( Point *p : animat->getMesh()->getVertices() )
            p->ground_receptor = true;
    }
}

std::vector<Collision*>& Contact::getCollisions(){
    return collisions;
}

void Contact::print(){
    std::cout << "Contact: A = " << A->getId() << 
                 ", B = " << B->getId() << 
                 ". Collisions: " << getCollisions().size() << "\n";
}

void Contact::clearCollisions(){
    for( Collision *c : collisions )
        delete c;

    collisions.clear();
}

void Contact::clearInactiveCollisions(){
    std::vector<Collision*>::iterator it;

    for( it = collisions.begin(); it != collisions.end(); ){
        if( !(*it)->active ){
            delete *it;
            it = collisions.erase(it);
        }else
            ++it;
    }
}

// Contact list
ContactList::ContactList(){

}

ContactList::~ContactList(){
    clear();
}

void ContactList::print(){
    std::cout << "List of contacts ("<< contacts.size() << "): \n";

    for( Contact *c : contacts )
        c->print();
}

Contact *ContactList::findContact( Body *A, Body *B ){
    for( Contact *fc : contacts )
        if( (fc->getA() == A && fc->getB() == B) ||
            (fc->getB() == A && fc->getA() == B))
            return fc;

    return nullptr;
}

void ContactList::prune(){
    std::vector<Contact*>::iterator it;

    for( it = contacts.begin(); it != contacts.end(); ){
        Contact *c = (*it);
        c->clearInactiveCollisions();

        if( !c->hasCollisions() ){
            delete c;
            it = contacts.erase(it);
        }else
            ++it;
    }
}

double ContactList::getContactArea( bool includeFloor ){
    double area = 0.0;

    for( Contact *c : contacts )
        area += c->getContactArea( includeFloor );

    return area;
}

void ContactList::push( Body* A, Body* B, Collision* c ){
    Contact *contact = findContact(A, B);

    if( contact == nullptr ){
        contact = new Contact( A, B );
        contacts.push_back(contact);
    }

    contact->addCollision( c );
}

void ContactList::clear(){
    for( Contact *c : contacts )
        delete c;

    contacts.clear();
}

int ContactList::count(){
    return contacts.size();
}

bool ContactList::isEmpty(){
    return contacts.empty();
}

void ContactList::updateReceptors(){
    for( Contact *c : contacts ){
        c->updateReceptors();
    }
}

std::vector<Contact*>& ContactList::getContacts(){
    return contacts;
}

// Collision
Collision::Collision( Point* p, BodyFrame* frame, Face* f, BodyFrame* faceFrame, const double bary[3], const Vec3& normal,
                      double rest, double slack, const ContactMaterial& material ):
    p(p), f(f), cnormal(toArma(normal)), depth(0.0), gap(0.0), impulse(0.0), area(material.area), active(false),
    frame(frame), faceFrame(faceFrame), normal(normal), friction{0.0, 0.0, 0.0}, soft(0.0), rest(rest), slack(slack),
    groundHeight(0.0), material(material){
    this->bary[0] = bary[0];
    this->bary[1] = bary[1];
    this->bary[2] = bary[2];
    this->cp = p->x_c;
}

Collision::Collision( Point* p, BodyFrame* frame, double groundHeight, double slack, const ContactMaterial& material ):
    p(p), f(nullptr), cnormal({0.0, 1.0, 0.0}), depth(0.0), gap(0.0), impulse(0.0), area(material.area), active(false),
    frame(frame), faceFrame(nullptr), normal{0.0, 1.0, 0.0}, friction{0.0, 0.0, 0.0}, soft(0.0), rest(0.0), slack(slack),
    groundHeight(groundHeight), material(material){
    this->bary[0] = this->bary[1] = this->bary[2] = 0.0;
    this->cp = p->x_c;
}

Vec3 Collision::vertexPosition() const {
    return frame->positionOf( toVec3( p->x_c ) );
}

Vec3 Collision::surfacePosition() const {
    if( f == nullptr ){
        Vec3 q = vertexPosition();
        q.y = groundHeight;
        return q;
    }

    return bary[0]*faceFrame->positionOf( toVec3( f->points[0]->x_c ) ) +
           bary[1]*faceFrame->positionOf( toVec3( f->points[1]->x_c ) ) +
           bary[2]*faceFrame->positionOf( toVec3( f->points[2]->x_c ) );
}

Vec3 Collision::relativeDisplacement( const Vec3& x, const Vec3& q ) const {
    Vec3 d = x - toVec3( p->x );

    if( f != nullptr ){
        // Minus the displacement of the contact point on the face
        for( int i = 0; i < 3; ++i )
            d = d + bary[i]*toVec3( f->points[i]->x );
        d = d - q;
    }

    return d;
}

void Collision::applyImpulse( const Vec3& j, const Vec3& rp, const Vec3& rq ){
    frame->applyImpulse( rp, j );

    if( f != nullptr )
        faceFrame->applyImpulse( rq, -1.0*j );
}

void Collision::solve( const ContactParams& params ){
    Vec3 x = vertexPosition();
    Vec3 q = surfacePosition();
    double C = dot( normal, x - q ) - rest;

    // Nothing to do while the surfaces are apart and have not pushed each other in this step
    if( C >= 0.0 && impulse <= 0.0 )
        return;

    const Vec3 rp = x - frame->centre;
    const Vec3 rq = f != nullptr ? q - faceFrame->centre : Vec3{0.0, 0.0, 0.0};

    // How far the surfaces separate along a direction per unit of impulse
    const auto inverseMass = [&]( const Vec3& dir ){
        double w = frame->inverseMassAt( rp, dir );

        if( f != nullptr )
            w += faceFrame->inverseMassAt( rq, dir );

        return w;
    };

    const double w = inverseMass( normal );

    // The surfaces give way: a spring with a damper, integrated implicitly. The impulse
    // can only push the bodies apart.
    if( material.compliance > 0.0 ){
        const double gamma = params.relaxation;
        const double approach = dot( normal, relativeDisplacement( x, q ) );
        double lambda = ( -C - material.compliance*soft - gamma*approach )/( (1.0 + gamma)*w + material.compliance );
        lambda = std::max( lambda, -soft );

        applyImpulse( lambda*normal, rp, rq );
        soft += lambda;
        impulse += lambda;

        x = vertexPosition();
        q = surfacePosition();
        C = dot( normal, x - q ) - rest;
    }

    // Beyond what the surfaces can give, the bodies do not get into each other
    const double hard = C + material.limit + slack;

    if( hard < 0.0 && w > 0.0 ){
        const double lambda = -hard/w;
        applyImpulse( lambda*normal, rp, rq );
        impulse += lambda;

        x = vertexPosition();
        q = surfacePosition();
    }

    if( impulse <= 0.0 )
        return;

    // Friction: opposes the displacement of the vertex relative to the surface during
    // this step, with an impulse limited by the Coulomb cone
    Vec3 slip = relativeDisplacement( x, q );
    slip = slip - dot( slip, normal )*normal;
    const double slipLength = length( slip );

    if( slipLength < 1e-12 )
        return;

    const Vec3 tangent = (1.0/slipLength)*slip;
    const double wt = inverseMass( tangent );

    if( wt <= 0.0 )
        return;

    // Total friction impulse needed to stop the slip, limited to what the contact can hold
    Vec3 total = friction - (slipLength/wt)*tangent;
    const double size = length( total );

    if( size > params.staticFriction*impulse )
        total = ( params.kineticFriction*impulse/size )*total;

    applyImpulse( total - friction, rp, rq );
    friction = total;
}

void Collision::finish( double tolerance ){
    const Vec3 q = surfacePosition();
    gap = dot( normal, vertexPosition() - q ) - rest;
    depth = std::max( 0.0, -gap );
    active = gap <= tolerance;
    cp = toArma( q );
}

arma::vec Collision::dent() const {
    return toArma( (material.share*depth)*normal );
}
