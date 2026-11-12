#include "collisionconstr.h"
#include <unordered_map>
#include <limits>
#include "../core/ground.h"
#include "../util/calg.h"
#include "../util/config.h"

using namespace morph::softmats;

namespace {

bool boxesOverlap( const Vec3& minA, const Vec3& maxA, const Vec3& minB, const Vec3& maxB, double margin ){
    return minA.x - margin <= maxB.x && maxA.x + margin >= minB.x &&
           minA.y - margin <= maxB.y && maxA.y + margin >= minB.y &&
           minA.z - margin <= maxB.z && maxA.z + margin >= minB.z;
}

}

void CollisionConstraint::init( BodySet *bs ){
    bodySet = bs;
    double edgeSum = 0.0;
    int edgeCount = 0;

    for( Body* b : bs->getBodies() ){
        if( b->type == BodyType::GROUND ){
            ground = b;
            groundHeight = static_cast<Ground*>(b)->getHeight();
            continue;
        }

        registerObject( b );

        for( FaceEntry& fe : entries.back().faces ){
            for( int i = 0; i < 3; ++i ){
                edgeSum += arma::norm( fe.face->points[i]->x - fe.face->points[(i+1)%3]->x );
                ++edgeCount;
            }
        }
    }

    edgeLength = edgeCount > 0 ? edgeSum/edgeCount : 1.0;

    // Nothing is known yet about how deep the bodies are into each other
    deepest = 0.0;

    for( BodyEntry& e : entries )
        deepest = std::max( deepest, 2.0*e.give );
    candidates.assign( pointBody.size(), std::vector<Candidate>() );
}

void CollisionConstraint::reset(){
    entries.clear();
    pointBody.clear();
    pointIndex.clear();
    candidates.clear();
    touched.clear();
    contacts->clear();
    ground = nullptr;
}

void CollisionConstraint::registerObject( Body *b ){
    BodyEntry e;
    e.body = b;
    e.points = &b->getMesh()->getVertices();
    e.offset = pointBody.size();
    e.near = false;
    e.colliding = false;

    std::unordered_map<Point*, int> indexOf;

    for( unsigned int i = 0; i < e.points->size(); ++i ){
        indexOf[(*e.points)[i]] = i;
        pointBody.push_back( entries.size() );
        pointIndex.push_back( i );
    }

    for( Face* f : b->getMesh()->getFaces() ){
        FaceEntry fe;
        fe.face = f;

        for( int i = 0; i < 3; ++i )
            fe.v[i] = indexOf[f->points[i]];

        e.faces.push_back( fe );
    }

    // The shape of the body without the dents it may have now
    std::vector<Vec3> shape;
    Vec3 centre = {0.0, 0.0, 0.0};

    for( Point *p : *e.points ){
        shape.push_back( toVec3( p->x ) - toVec3( p->dent ) );
        centre = centre + (1.0/e.points->size())*shape.back();
    }

    // Each vertex stands for a third of the area of the faces around it
    e.area.assign( e.points->size(), 0.0 );

    for( FaceEntry& fe : e.faces ){
        const double area = 0.5*length( cross( shape[fe.v[1]] - shape[fe.v[0]], shape[fe.v[2]] - shape[fe.v[0]] ) );

        for( int i = 0; i < 3; ++i )
            e.area[fe.v[i]] += area/3.0;
    }

    // A dent can take up to a part of the thickness of the body
    double thickness = std::numeric_limits<double>::max();

    for( const Vec3& x : shape )
        thickness = std::min( thickness, length( x - centre ) );

    e.give = shape.empty() ? 0.0 : 0.25*thickness;

    e.xc.resize( e.points->size() );
    e.normals.resize( e.points->size() );
    entries.push_back( e );
}

ContactMaterial CollisionConstraint::material( const BodyEntry& a, int vertex, const BodyEntry* b, double h ) const {
    // How much each surface gives per unit of pressure. The ground does not give.
    const double ka = a.body->getStiffness();
    const double kb = b != nullptr ? b->body->getStiffness() : 0.0;
    const double ca = ka > 0.0 ? 1.0/ka : 0.0;
    const double cb = kb > 0.0 ? 1.0/kb : 0.0;

    ContactMaterial m = { 0.0, 0.0, 0.0, a.area[vertex] };

    // Where two bodies press on each other, the vertices of both of them find the
    // same patch of surface, so each one stands for half of its area
    if( b != nullptr )
        m.area *= 0.5;

    if( ca + cb <= 0.0 || m.area <= 0.0 )
        return m;

    m.compliance = (ca + cb)/( m.area*h*h );
    m.share = ca/(ca + cb);
    m.limit = ( ca > 0.0 ? a.give : 0.0 ) + ( cb > 0.0 ? b->give : 0.0 );
    return m;
}

void CollisionConstraint::updatePositions( BodyEntry& e ){
    const double inf = std::numeric_limits<double>::max();
    e.boxMin = {inf, inf, inf};
    e.boxMax = {-inf, -inf, -inf};

    for( unsigned int i = 0; i < e.points->size(); ++i ){
        const Vec3 x = toVec3( (*e.points)[i]->x_c );
        e.xc[i] = x;
        e.boxMin = {std::min(e.boxMin.x, x.x), std::min(e.boxMin.y, x.y), std::min(e.boxMin.z, x.z)};
        e.boxMax = {std::max(e.boxMax.x, x.x), std::max(e.boxMax.y, x.y), std::max(e.boxMax.z, x.z)};
    }
}

// Vertex normals as the area weighted average of the normals of the faces around
void CollisionConstraint::updateNormals( BodyEntry& e ){
    for( Vec3& n : e.normals )
        n = {0.0, 0.0, 0.0};

    for( FaceEntry& fe : e.faces ){
        const Vec3 fn = cross( e.xc[fe.v[1]] - e.xc[fe.v[0]], e.xc[fe.v[2]] - e.xc[fe.v[0]] );

        for( int i = 0; i < 3; ++i )
            e.normals[fe.v[i]] = e.normals[fe.v[i]] + fn;
    }
}

// Mass, centre and inertia of the body at the candidate positions
void CollisionConstraint::updateFrame( BodyEntry& e ){
    BodyFrame& frame = e.frame;
    frame = BodyFrame();

    double mass = 0.0;
    Vec3 centre = {0.0, 0.0, 0.0};
    bool movable = true;

    for( Point *p : *e.points ){
        if( p->invMass() <= 0.0 ){
            movable = false;
            continue;
        }

        const double m = 1.0/p->w;
        centre = centre + m*toVec3( p->x_c );
        mass += m;
    }

    if( mass <= 0.0 )
        return;

    frame.centre = (1.0/mass)*centre;

    // A body with locked points is held in place: only its surface gives way
    if( !movable )
        return;

    arma::mat inertia( 3, 3, arma::fill::zeros );

    for( Point *p : *e.points ){
        const Vec3 r = toVec3( p->x_c ) - frame.centre;
        const double m = 1.0/p->w;
        const double r2 = dot( r, r );
        const double c[3] = {r.x, r.y, r.z};

        for( int i = 0; i < 3; ++i ){
            inertia(i, i) += m*r2;
            for( int j = 0; j < 3; ++j )
                inertia(i, j) -= m*c[i]*c[j];
        }
    }

    arma::mat inverse;

    if( !arma::inv( inverse, inertia ) )
        inverse.zeros( 3, 3 );

    frame.invMass = 1.0/mass;

    for( int i = 0; i < 3; ++i )
        for( int j = 0; j < 3; ++j )
            frame.invInertia[i][j] = inverse(i, j);
}

// Moves the vertices with the rigid motion collected by the frame
void CollisionConstraint::applyFrame( BodyEntry& e ){
    BodyFrame& frame = e.frame;

    if( !frame.moved )
        return;

    // The surface has moved since the frame was set up. Rotating about the centre of
    // mass as it is now keeps the linear momentum exact.
    Vec3 centre = {0.0, 0.0, 0.0};

    for( Point *p : *e.points )
        centre = centre + (1.0/p->w)*toVec3( p->x_c );

    centre = frame.invMass*centre;

    for( Point *p : *e.points ){
        const Vec3 x = toVec3( p->x_c );
        addTo( p->x_c, frame.translation + cross( frame.rotation, x - centre ) );
    }

    frame.translation = {0.0, 0.0, 0.0};
    frame.rotation = {0.0, 0.0, 0.0};
    frame.moved = false;
}

void CollisionConstraint::findGroundCollisions( double margin, double h ){
    if( ground == nullptr )
        return;

    const double recovery = h*Config::getConfig()->getRecoverySpeed();

    for( BodyEntry& e : entries ){
        if( e.boxMin.y > groundHeight + margin )
            continue;

        for( unsigned int i = 0; i < e.points->size(); ++i ){
            if( e.xc[i].y > groundHeight + margin )
                continue;

            Point *p = (*e.points)[i];
            const ContactMaterial m = material( e, i, nullptr, h );
            // A vertex that starts the step deeper than the surface can give is brought back gradually
            const double startDepth = std::max( 0.0, groundHeight - m.limit - p->x(1) );
            const double slack = std::max( 0.0, startDepth - recovery );
            contacts->push( e.body, ground, new Collision( p, &e.frame, groundHeight, slack, m ) );
            e.colliding = true;
        }
    }
}

// Finds, for each vertex, the closest face of each of the bodies around it
void CollisionConstraint::findCandidates( double radius ){
    for( int g : touched )
        candidates[g].clear();

    touched.clear();

    // Only bodies whose bounding boxes are close to each other need to be searched
    bool any = false;

    for( BodyEntry& e : entries )
        e.near = false;

    for( unsigned int a = 0; a < entries.size(); ++a ){
        for( unsigned int b = a + 1; b < entries.size(); ++b ){
            if( boxesOverlap( entries[a].boxMin, entries[a].boxMax, entries[b].boxMin, entries[b].boxMax, radius ) ){
                entries[a].near = entries[b].near = true;
                any = true;
            }
        }
    }

    if( !any )
        return;

    size_t numPoints = 0;

    for( BodyEntry& e : entries )
        if( e.near ) numPoints += e.points->size();

    hash.reset( std::max( edgeLength, radius ), 2*numPoints + 1 );

    for( BodyEntry& e : entries ){
        if( !e.near ) continue;

        for( unsigned int i = 0; i < e.xc.size(); ++i )
            hash.insert( e.xc[i].x, e.xc[i].y, e.xc[i].z, e.offset + i );
    }

    const double radius2 = radius*radius;

    for( unsigned int b = 0; b < entries.size(); ++b ){
        BodyEntry& eb = entries[b];
        if( !eb.near ) continue;

        for( unsigned int fi = 0; fi < eb.faces.size(); ++fi ){
            const FaceEntry& fe = eb.faces[fi];
            const Vec3& q0 = eb.xc[fe.v[0]];
            const Vec3& q1 = eb.xc[fe.v[1]];
            const Vec3& q2 = eb.xc[fe.v[2]];
            // Bounding box of the face, extended by the search radius
            const Vec3 lo = { std::min({q0.x, q1.x, q2.x}) - radius, std::min({q0.y, q1.y, q2.y}) - radius, std::min({q0.z, q1.z, q2.z}) - radius };
            const Vec3 hi = { std::max({q0.x, q1.x, q2.x}) + radius, std::max({q0.y, q1.y, q2.y}) + radius, std::max({q0.z, q1.z, q2.z}) + radius };

            for( int kx = hash.cell(lo.x); kx <= hash.cell(hi.x); ++kx )
            for( int ky = hash.cell(lo.y); ky <= hash.cell(hi.y); ++ky )
            for( int kz = hash.cell(lo.z); kz <= hash.cell(hi.z); ++kz ){
                const std::vector<int>* bucket = hash.query( kx, ky, kz );
                if( bucket == nullptr ) continue;

                for( int g : *bucket ){
                    if( pointBody[g] == static_cast<int>(b) ) continue;

                    const Vec3& x = entries[pointBody[g]].xc[pointIndex[g]];

                    if( x.x < lo.x || x.x > hi.x || x.y < lo.y || x.y > hi.y || x.z < lo.z || x.z > hi.z )
                        continue;

                    Candidate c;
                    c.closest = closestPointOnTriangle( x, q0, q1, q2, c.bary );
                    const Vec3 d = x - c.closest;
                    c.dist2 = dot( d, d );

                    if( c.dist2 > radius2 ) continue;

                    c.body = b;
                    c.face = fi;

                    // Keep only the closest face of this body
                    std::vector<Candidate>& found = candidates[g];
                    bool known = false;

                    for( Candidate& other : found ){
                        if( other.body == c.body ){
                            known = true;
                            if( c.dist2 < other.dist2 ) other = c;
                            break;
                        }
                    }

                    if( !known ){
                        if( found.empty() ) touched.push_back( g );
                        found.push_back( c );
                    }
                }
            }
        }
    }
}

// Turns the candidates that are close enough to the surface into collisions
void CollisionConstraint::storeCollisions( double rest, double margin, double h ){
    const double recovery = h*Config::getConfig()->getRecoverySpeed();

    for( int g : touched ){
        BodyEntry& ea = entries[pointBody[g]];
        Point *p = (*ea.points)[pointIndex[g]];
        const Vec3& x = ea.xc[pointIndex[g]];

        for( Candidate& c : candidates[g] ){
            BodyEntry& eb = entries[c.body];
            const FaceEntry& fe = eb.faces[c.face];

            // The vertex is inside the other body when it is behind its surface. Both the
            // normal of the face and the smooth normal at the closest point have to agree,
            // which avoids misjudging vertices that are next to an edge.
            const Vec3 faceNormal = cross( eb.xc[fe.v[1]] - eb.xc[fe.v[0]], eb.xc[fe.v[2]] - eb.xc[fe.v[0]] );
            Vec3 smoothNormal = c.bary[0]*eb.normals[fe.v[0]] + c.bary[1]*eb.normals[fe.v[1]] + c.bary[2]*eb.normals[fe.v[2]];
            const double smoothLength = length( smoothNormal );

            if( smoothLength < 1e-12 ) continue;

            smoothNormal = (1.0/smoothLength)*smoothNormal;

            const Vec3 d = x - c.closest;
            const double dist = std::sqrt( c.dist2 );
            const bool inside = dot( d, smoothNormal ) < 0.0 && dot( d, faceNormal ) < 0.0;
            const double signedDist = inside ? -dist : dist;

            if( signedDist - rest > margin ) continue;

            Vec3 n = smoothNormal;

            if( dist > 1e-9 )
                n = ( (inside ? -1.0 : 1.0)/dist )*d;

            const ContactMaterial m = material( ea, pointIndex[g], &eb, h );
            // Depth beyond what the surfaces can give at the start of the step, which is brought back gradually
            const Vec3 q = c.bary[0]*toVec3( fe.face->points[0]->x ) +
                           c.bary[1]*toVec3( fe.face->points[1]->x ) +
                           c.bary[2]*toVec3( fe.face->points[2]->x );
            const double startDepth = std::max( 0.0, rest - m.limit - dot( n, toVec3( p->x ) - q ) );
            const double slack = std::max( 0.0, startDepth - recovery );

            contacts->push( ea.body, eb.body, new Collision( p, &ea.frame, fe.face, &eb.frame, c.bary, n, rest, slack, m ) );
            ea.colliding = eb.colliding = true;
        }
    }
}

void CollisionConstraint::generate( int step ){
    Config *config = Config::getConfig();
    const double h = config->getTimeStep();
    const double rest = config->getContactDistance();
    // Collisions are created a bit before the surfaces touch, so that they are already
    // in place when the other constraints move the vertices during the solver iterations
    const double margin = 0.25*edgeLength;

    contacts->clear();

    double maxSpeed = 0.0;

    for( BodyEntry& e : entries ){
        updatePositions( e );
        e.colliding = false;
        e.frame = BodyFrame();

        for( Point *p : *e.points )
            maxSpeed = std::max( maxSpeed, arma::norm( p->x_c - p->x )/h );
    }

    findGroundCollisions( margin, h );

    // Vertices can be found up to this distance of a surface, on either side. It grows
    // with the speed so that a vertex cannot get past the surface within one step, and
    // with the depth that the bodies had in the last step.
    const double radius = rest + margin + 2.0*h*maxSpeed + 1.2*deepest;
    findCandidates( radius );

    for( BodyEntry& e : entries )
        if( e.near ) updateNormals( e );

    storeCollisions( rest, margin, h );
}

void CollisionConstraint::solve(){
    Config *config = Config::getConfig();
    const ContactParams params = { config->getStaticFriction(), config->getKineticFriction(),
                                   config->getRelaxationTime()/config->getTimeStep() };

    for( BodyEntry& e : entries )
        if( e.colliding ) updateFrame( e );

    // The collisions of a body depend on each other through its frame, so several
    // passes are made over them. The ground cannot give way, so it is solved last.
    const int passes = 3;

    for( int pass = 0; pass < passes; ++pass ){
        for( Contact* contact: contacts->getContacts() ){
            if( contact->getB() == ground ) continue;

            for( Collision* c : contact->getCollisions() )
                c->solve( params );
        }

        for( Contact* contact: contacts->getContacts() ){
            if( contact->getB() != ground ) continue;

            for( Collision* c : contact->getCollisions() )
                c->solve( params );
        }
    }

    for( BodyEntry& e : entries )
        if( e.colliding ) applyFrame( e );
}

void CollisionConstraint::updateVelocity(){
    const double tolerance = Config::getConfig()->getContactDistance();
    deepest = 0.0;

    for( Contact* contact: contacts->getContacts() ){
        for( Collision* c : contact->getCollisions() ){
            c->finish( tolerance );
            c->p->dent += c->dent();

            if( contact->getB() != ground )
                deepest = std::max( deepest, c->depth );
        }
    }

    // A dent made by another body does not push the surface under the ground
    if( ground != nullptr ){
        for( BodyEntry& e : entries ){
            if( !e.colliding ) continue;

            for( Point *p : *e.points )
                p->dent(1) = std::max( p->dent(1), std::min( 0.0, groundHeight - p->x_c(1) ) );
        }
    }

    // Only the collisions where the bodies are touching are reported
    contacts->prune();
    contacts->updateReceptors();

    // Bodies in contact lose part of their velocity relative to each other. This stands
    // for the energy that real soft bodies dissipate when they roll or slide on something.
    const double loss = std::min( 1.0, Config::getConfig()->getTimeStep()*Config::getConfig()->getContactDamping() );

    if( loss <= 0.0 )
        return;

    for( Contact* contact: contacts->getContacts() ){
        std::vector<Body*> pair;

        for( Body* b : {contact->getA(), contact->getB()} )
            if( b != ground ) pair.push_back( b );

        // The velocity is damped towards that of the pair as a whole, which keeps
        // its momentum, or towards rest when the other body is the ground
        arma::vec reference = arma::zeros<arma::vec>(3);

        if( pair.size() == 2 ){
            double mass = 0.0;

            for( Body* b : pair ){
                for( Point* p : b->getMesh()->getVertices() ){
                    if( p->invMass() <= 0.0 ) continue;
                    reference += p->v/p->w;
                    mass += 1.0/p->w;
                }
            }

            if( mass <= 0.0 ) continue;
            reference /= mass;
        }

        for( Body* b : pair ){
            for( Point* p : b->getMesh()->getVertices() ){
                if( p->invMass() > 0.0 )
                    p->v -= loss*(p->v - reference);
            }
        }
    }
}

ContactList* CollisionConstraint::getContacts(){
    return contacts;
}

CollisionConstraint::CollisionConstraint():bodySet(nullptr), ground(nullptr), groundHeight(0.0), edgeLength(1.0), deepest(0.0){
    contacts = new ContactList();
}

CollisionConstraint::~CollisionConstraint(){
    delete contacts;
}
