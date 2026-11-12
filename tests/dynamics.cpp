/**
 * Headless tests of the dynamics: no window is opened.
 *
 * Usage: dynamics <scenario> <mesh.obj | icosphere:N> <working dir> [settings]
 *
 * Each scenario builds a small scene, runs it and checks that the bodies behave:
 * that they come to rest, do not sink in the ground and do not go through each other.
 * "icosphere:N" generates a unit sphere with N subdivisions instead of loading a mesh.
 * The settings are entries for the configuration file, such as '"stiffness" : 10000'.
 */
#include <core/animat.h>
#include <core/ground.h>
#include <core/bodyset.h>
#include <core/pbdim.h>
#include <collisions/collisionconstr.h>
#include <util/config.h>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <string>

using namespace morph::softmats;

namespace {

const double GROUND_HEIGHT = -2.0;
const double GRAVITY = 10.0;
// Depth under the ground that is tolerated, for bodies of radius 1
const double PENETRATION = 1e-3;
// Smallest distance between the centres of two balls of radius 1, however hard they are
// pressed: their surfaces give way, but only up to a part of their thickness
const double CLOSEST = 1.4;
// Relative error tolerated when comparing with a perfect sphere. Set by the mesh.
double TOLERANCE = 0.15;
int failures = 0;

void check( bool ok, const std::string& what, double value, double limit ){
    printf( "  [%s] %s: %.3g (limit %.3g)\n", ok ? "ok" : "FAIL", what.c_str(), value, limit );
    if( !ok ) ++failures;
}

// Writes a unit icosphere in the obj dialect read by the simulator
void writeIcosphere( const std::string& path, int subdivisions ){
    const double t = (1.0 + std::sqrt(5.0))/2.0;
    std::vector<arma::vec> v = {
        {-1, t, 0}, {1, t, 0}, {-1, -t, 0}, {1, -t, 0}, {0, -1, t}, {0, 1, t},
        {0, -1, -t}, {0, 1, -t}, {t, 0, -1}, {t, 0, 1}, {-t, 0, -1}, {-t, 0, 1} };
    std::vector<std::array<int, 3>> f = {
        {0,11,5}, {0,5,1}, {0,1,7}, {0,7,10}, {0,10,11}, {1,5,9}, {5,11,4}, {11,10,2}, {10,7,6}, {7,1,8},
        {3,9,4}, {3,4,2}, {3,2,6}, {3,6,8}, {3,8,9}, {4,9,5}, {2,4,11}, {6,2,10}, {8,6,7}, {9,8,1} };

    // Turned to an arbitrary orientation, so that it does not start balanced on a vertex
    arma::vec axis = {1.0, 2.0, 3.0};
    axis /= arma::norm(axis);
    const double angle = 0.7;

    for( arma::vec& p : v ){
        p /= arma::norm(p);
        p = p*std::cos(angle) + arma::cross(axis, p)*std::sin(angle) + axis*arma::dot(axis, p)*(1.0 - std::cos(angle));
    }

    for( int s = 0; s < subdivisions; ++s ){
        std::map<std::pair<int, int>, int> midpoints;
        const auto midpoint = [&]( int a, int b ){
            const std::pair<int, int> key = std::minmax( a, b );
            if( !midpoints.count(key) ){
                arma::vec m = v[a] + v[b];
                v.push_back( m/arma::norm(m) );
                midpoints[key] = v.size() - 1;
            }
            return midpoints[key];
        };

        std::vector<std::array<int, 3>> next;
        for( const auto& tri : f ){
            const int a = midpoint( tri[0], tri[1] ), b = midpoint( tri[1], tri[2] ), c = midpoint( tri[2], tri[0] );
            next.push_back( {tri[0], a, c} );
            next.push_back( {tri[1], b, a} );
            next.push_back( {tri[2], c, b} );
            next.push_back( {a, b, c} );
        }
        f = next;
    }

    std::ofstream out( path );
    for( const arma::vec& p : v ) out << "v " << p(0) << " " << p(1) << " " << p(2) << "\n";
    out << "vn 0 1 0\n";
    for( const auto& tri : f ) out << "f " << tri[0]+1 << "//1 " << tri[1]+1 << "//1 " << tri[2]+1 << "//1\n";
}

struct BodyState{
    arma::vec cm;
    double lowest; // Height of the lowest vertex over the ground
    double speed; // Largest vertex speed
    double radius; // Largest distance of a vertex to the centre
};

BodyState stateOf( Animat* a ){
    std::vector<Point*>& vs = a->getMesh()->getVertices();
    BodyState s = { arma::zeros<arma::vec>(3), 1e9, 0.0, 0.0 };

    for( Point* p : vs ){
        s.cm += p->x/vs.size();
        s.lowest = std::min( s.lowest, p->x(1) - GROUND_HEIGHT );
        s.speed = std::max( s.speed, arma::norm(p->v) );
    }

    for( Point* p : vs ) s.radius = std::max( s.radius, arma::norm(p->x - s.cm) );
    return s;
}

// A scene without a view
struct Scene{
    BodySet bodies;
    PBD solver;
    std::vector<Animat*> animats;
    int step = 0;
    double h = Config::getConfig()->getTimeStep();
    // Worst values seen while running
    double deepest = 0.0; // Largest depth of a vertex under the ground
    double fastest = 0.0;
    double closest = 1e9; // Smallest distance between the centres of two animats
    bool finite = true;
    std::function<void(Scene&)> beforeStep;

    Animat* add( double x, double y, double z, double stiffness = 0.2 ){
        Animat* a = new Animat( x, y, z );
        a->setMass( 100.0 );
        a->setConstraints( stiffness );
        bodies.add( a );
        animats.push_back( a );
        return a;
    }

    void finish( bool ground, double gravity ){
        if( ground ) bodies.add( new Ground( GROUND_HEIGHT ) );
        if( gravity != 0 ) bodies.addExternalForce( arma::vec({0.0, -gravity, 0.0}) );
        bodies.addCollisionConstraint( new CollisionConstraint() );
    }

    void run( double seconds ){
        const int n = static_cast<int>( seconds/h );

        for( int i = 0; i < n; ++i, ++step ){
            bodies.resetForces();
            if( beforeStep ) beforeStep( *this );
            bodies.resetReceptors();
            solver.loop( &bodies, step );

            std::vector<BodyState> states;
            for( Animat* a : animats ){
                const BodyState s = stateOf( a );
                deepest = std::max( deepest, -s.lowest );
                fastest = std::max( fastest, s.speed );
                finite = finite && s.cm.is_finite();
                states.push_back( s );
            }

            for( size_t a = 0; a < states.size(); ++a )
                for( size_t b = a + 1; b < states.size(); ++b )
                    closest = std::min( closest, arma::norm( states[a].cm - states[b].cm ) );
        }
    }

    arma::vec momentum(){
        arma::vec m = arma::zeros<arma::vec>(3);
        for( Animat* a : animats )
            for( Point* p : a->getMesh()->getVertices() ) m += p->v/p->w;
        return m;
    }

    bool touching( Body* a, Body* b ){
        ContactList* contacts = bodies.getContacts();
        Contact* c = contacts->findContact( a, b );
        return c != nullptr && c->hasCollisions();
    }
};

// Runs until nothing has moved for a while, or the time is up
void settle( Scene& scene, double limit ){
    for( double t = 0.0; t < limit; t += 1.0 ){
        scene.fastest = 0.0;
        scene.run( 1.0 );
        if( scene.fastest < 1e-6 ) break;
    }
}

// The bodies have to come to rest, and then stay where they are
void checkAtRest( Scene& scene, double seconds ){
    settle( scene, 120.0 );

    std::vector<BodyState> before;
    for( Animat* a : scene.animats ) before.push_back( stateOf( a ) );

    scene.fastest = 0.0;
    scene.run( seconds );

    double drift = 0.0, sink = 0.0;
    for( size_t i = 0; i < scene.animats.size(); ++i ){
        const BodyState after = stateOf( scene.animats[i] );
        drift = std::max( drift, arma::norm( after.cm - before[i].cm ) );
        sink = std::max( sink, -after.lowest );
    }

    check( drift < 1e-3, "drift of the centre while at rest", drift, 1e-3 );
    check( scene.fastest < 1e-3, "largest vertex speed while at rest", scene.fastest, 1e-3 );
    check( sink < PENETRATION, "depth under the ground", sink, PENETRATION );
}

// A ball placed on the ground stays where it is
void rest(){
    Scene scene;
    Animat* a = scene.add( 0, GROUND_HEIGHT + 1.0, 0 );
    scene.finish( true, GRAVITY );
    checkAtRest( scene, 20.0 );
    check( scene.deepest < PENETRATION, "largest depth under the ground", scene.deepest, PENETRATION );
    const double radius = stateOf( a ).radius;
    check( std::fabs(radius - 1.0) < 0.05, "change of radius", std::fabs(radius - 1.0), 0.05 );
    check( scene.touching( a, scene.bodies.getBodies().back() ), "contact with the ground reported", 1, 1 );
}

// A ball on the ground flattens against it as much as its weight and stiffness say,
// whatever the mesh, and a heavier ball flattens more
void soft(){
    const double pi = 3.14159265358979;
    const double stiffness = Config::getConfig()->getStiffness();
    double sinkOf[2], areaOf[2];

    for( int i = 0; i < 2; ++i ){
        const double mass = i == 0 ? 100.0 : 400.0;
        Scene scene;
        Animat* a = scene.add( 0, GROUND_HEIGHT + 1.0, 0 );
        a->setMass( mass );
        scene.finish( true, GRAVITY );
        settle( scene, 60.0 );

        // A sphere of radius R pressed in by d against a plane touches it over a disc of
        // area 2 pi R d, and is pushed back with a force stiffness*pi*R*d^2
        const double expected = std::sqrt( mass*GRAVITY/(pi*stiffness) );
        sinkOf[i] = 1.0 - (stateOf( a ).cm(1) - GROUND_HEIGHT);
        areaOf[i] = scene.bodies.getContacts()->getContactArea( true );
        printf( "  mass %g: centre lowered by %.4f (a sphere would be %.4f), contact area %.4f (%.4f)\n",
                mass, sinkOf[i], expected, areaOf[i], 2.0*pi*expected );

        const double error = std::fabs( sinkOf[i] - expected )/expected;
        check( error < TOLERANCE, "relative error of the flattening", error, TOLERANCE );
        // The area is counted in whole vertices, which is coarse when few of them touch
        const double expectedArea = 2.0*pi*expected;
        const double vertexArea = 4.0*pi/a->getMesh()->getNumVertices();
        const double areaLimit = 2.0*TOLERANCE + 1.5*vertexArea/expectedArea;
        const double areaError = std::fabs( areaOf[i] - expectedArea )/expectedArea;
        check( areaError < areaLimit, "relative error of the contact area", areaError, areaLimit );

        // The surface is flat where it touches the ground
        int flat = 0;
        for( Point* p : a->getMesh()->getVertices() )
            if( std::fabs( p->x(1) - GROUND_HEIGHT ) < 1e-6 ) ++flat;
        check( flat >= 1, "vertices lying on the ground", flat, 1 );
    }

    check( sinkOf[1] > 1.5*sinkOf[0], "the heavier ball flattens more", sinkOf[1]/sinkOf[0], 1.5 );
    check( areaOf[1] > areaOf[0], "and touches more ground", areaOf[1]/areaOf[0], 1.0 );
}

// A ball dropped from a height lands and comes to rest
void drop(){
    Scene scene;
    scene.add( 0, GROUND_HEIGHT + 3.0, 0 );
    scene.finish( true, GRAVITY );
    scene.run( 10.0 );
    check( scene.deepest < PENETRATION, "largest depth under the ground", scene.deepest, PENETRATION );
    checkAtRest( scene, 10.0 );
}

// A ball that starts half sunk in the ground comes out without being launched
void sunk(){
    Scene scene;
    scene.add( 0, GROUND_HEIGHT + 0.5, 0 );
    scene.finish( true, GRAVITY );
    scene.run( 10.0 );
    check( scene.fastest < 3.0, "largest speed while coming out", scene.fastest, 3.0 );
    checkAtRest( scene, 5.0 );
}

// Two balls pushed against each other stay apart, and at rest once they are pressed together
void push(){
    Scene scene;
    Animat* a = scene.add( -1.2, GROUND_HEIGHT + 1.0, 0 );
    Animat* b = scene.add( 1.2, GROUND_HEIGHT + 1.0, 0 );
    scene.finish( true, GRAVITY );
    // Each ball is pushed towards the other one
    scene.beforeStep = [=]( Scene& ){
        const arma::vec d = stateOf(b).cm - stateOf(a).cm;
        a->move( d(0), 0, d(2) );
        b->move( -d(0), 0, -d(2) );
    };
    scene.run( 20.0 );
    check( scene.finite, "positions are finite", scene.finite, 1 );
    check( scene.closest > CLOSEST, "smallest distance between centres", scene.closest, CLOSEST );
    check( scene.deepest < PENETRATION, "largest depth under the ground", scene.deepest, PENETRATION );
    check( scene.touching( a, b ), "contact between the balls reported", 1, 1 );
    const double separation = arma::norm( stateOf(a).cm - stateOf(b).cm );
    check( separation < 2.1, "balls together at the end", separation, 2.1 );
    scene.fastest = 0.0;
    scene.run( 5.0 );
    check( scene.fastest < 0.05, "largest vertex speed while pressed together", scene.fastest, 0.05 );
}

// Two balls colliding in empty space: momentum is conserved and they bounce apart
void momentum(){
    Scene scene;
    Animat* a = scene.add( -1.5, 0.1, 0 );
    Animat* b = scene.add( 1.5, -0.1, 0.2 );
    scene.finish( false, 0.0 );
    for( Point* p : a->getMesh()->getVertices() ) p->v = {2.0, 0.0, 0.0};
    for( Point* p : b->getMesh()->getVertices() ) p->v = {-1.0, 0.0, 0.0};
    const arma::vec before = scene.momentum();
    scene.run( 3.0 );
    const double change = arma::norm( scene.momentum() - before )/arma::norm( before );
    check( change < 1e-9, "relative change of momentum", change, 1e-9 );
    check( scene.closest > CLOSEST, "smallest distance between centres", scene.closest, CLOSEST );
    check( scene.fastest < 4.0, "largest vertex speed", scene.fastest, 4.0 );
    const double separation = arma::norm( stateOf(a).cm - stateOf(b).cm );
    check( separation > 2.0, "balls apart at the end", separation, 2.0 );
}

// Several balls dropped on top of each other end up in a pile at rest
void pile(){
    Scene scene;
    for( int i = 0; i < 6; ++i )
        scene.add( 0.3*(i%3) - 0.3, GROUND_HEIGHT + 1.5 + 2.3*i, 0.25*(i%2) );
    scene.finish( true, GRAVITY );
    scene.run( 30.0 );
    check( scene.finite, "positions are finite", scene.finite, 1 );
    check( scene.closest > CLOSEST, "smallest distance between centres", scene.closest, CLOSEST );
    check( scene.deepest < PENETRATION, "largest depth under the ground", scene.deepest, PENETRATION );
    checkAtRest( scene, 10.0 );
}

}

int main( int n, char** args ){
    if( n < 4 ){
        std::cerr << "Usage: " << args[0] << " <scenario> <mesh.obj | icosphere:N> <working dir> [settings]\n";
        return 2;
    }

    const std::string scenario = args[1];
    std::string mesh = args[2];
    const std::string dir = args[3];

    if( mesh.rfind( "icosphere:", 0 ) == 0 ){
        const int subdivisions = std::stoi( mesh.substr( 10 ) );
        mesh = dir + "/icosphere" + std::to_string( subdivisions ) + ".obj";
        writeIcosphere( mesh, subdivisions );
    }

    const std::string settings = n > 4 ? args[4] : "";
    const std::string config = dir + "/" + scenario + "_" + std::to_string( std::hash<std::string>()( mesh + settings ) ) + ".json";
    std::ofstream out( config );
    out << "{ \"mesh\" : \"" << mesh << "\", \"shaderPath\" : \".\"";
    if( !settings.empty() ) out << ", " << settings;
    out << " }\n";
    out.close();
    Config::configPath = config;

    const std::map<std::string, void(*)()> scenarios = {
        {"rest", rest}, {"soft", soft}, {"drop", drop}, {"sunk", sunk}, {"push", push}, {"momentum", momentum}, {"pile", pile} };

    if( !scenarios.count( scenario ) ){
        std::cerr << "Unknown scenario " << scenario << "\n";
        return 2;
    }

    const auto start = std::chrono::steady_clock::now();
    scenarios.at( scenario )();
    const double elapsed = std::chrono::duration<double>( std::chrono::steady_clock::now() - start ).count();
    printf( "  %s: %d failed checks, %.1f s\n", scenario.c_str(), failures, elapsed );
    return failures == 0 ? 0 : 1;
}
