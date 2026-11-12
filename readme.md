# Softmats

A simulator for deformable agents ("softmats") that keeps explicit track of the
contacts between them, so behaviours that depend on body contact, such as
huddling, can be modelled.

Bodies are simulated with position-based dynamics. Each body holds its shape
through a shape-matching constraint. Contacts are found from the current state:
a vertex is in contact while it is touching or inside another body, or under
the ground. Each contact is solved as a constraint that pushes the two bodies
apart with equal and opposite impulses, acting on each body as a whole, and
with Coulomb friction. The contacts found at each step are passed to a user
callback.

Bodies are soft where they touch. The surface of a body gives way under
pressure like a bed of springs: pushing it in by a depth `d` takes a pressure
`stiffness * d`. Two bodies pressed together, or a body lying on the ground,
flatten against each other over an area that grows with the force between
them. Each vertex stands for the area of the surface around it, so the result
does not depend on how fine the mesh is, although a flat patch only looks flat
when several vertices fit in it. `Contact::getContactArea` and
`ContactList::getContactArea` report that area.

As a guide, a sphere of radius `R` and weight `W` lying on the ground is
lowered by `sqrt( W / (pi * R * stiffness) )` and touches it over an area
`2 * pi * R` times that. Only the surface gives way: a body does not bulge at
the sides when it is squeezed, and it cannot be pushed in by more than a quarter
of its thickness.

## Dependencies

- A C++17 compiler and CMake 3.10 or newer
- OpenGL and GLFW 3.3 or newer
- GLM
- Armadillo
- OpenCV (only used to record video)

On Debian or Ubuntu:

```
sudo apt install build-essential cmake libgl-dev libglfw3-dev libglm-dev libarmadillo-dev libopencv-dev
```

## Building

```
mkdir build
cd build
cmake ..
make
```

If linking fails on `glCreateVertexArrays` or similar, configure with
`cmake -DUSE_GLEW=ON ..` (this needs GLEW installed).

## Running the examples

Each example takes the path to its configuration file:

```
./build/examples/softcollision/softcollision examples/softcollision/softmats.json
./build/examples/bags/bags examples/bags/softmats.json
./build/examples/huddling/huddling examples/huddling/softmats.json
```

- `softcollision`: two softmats are pushed against each other and flatten where
  they touch. It uses a finer mesh (`res/models/sphere_fine.obj`, 642 vertices)
  so that the flat patches show.
- `bags`: softmats dropped on the ground, reporting the contact area.
- `huddling`: a litter of pups that move according to a thermal model. Feeding
  the contacts back into that model is not finished, so for now every pup
  walks in a straight line.

## Configuration file

A simulation reads its settings from a JSON file. The two paths are required;
relative paths are resolved from the location of the file.

```json
{
    "mesh" : "../../res/models/sphere.obj",
    "shaderPath" : "../../shaders/"
}
```

The mesh must be closed, and the collisions assume it is roughly convex.

The dynamics can be tuned with these optional numbers:

| Entry | Default | Meaning |
|---|---|---|
| `timeStep` | 0.01 | Time advanced on each step |
| `iterations` | 4 | Solver iterations per step |
| `staticFriction` | 0.6 | Friction coefficient of contacts that are not sliding |
| `kineticFriction` | 0.5 | Friction coefficient of sliding contacts |
| `stiffness` | 50000 | Pressure (force per unit of area) needed to push the surface of a body in by a unit of length. Lower is softer. For hard bodies use a large value, such as 1e7, rather than 0: with 0 the bodies are exactly rigid and keep rocking on their facets. `Body::setStiffness` changes it for one body |
| `relaxationTime` | 0.1 | Time that a dent takes to give way or to recover. It damps the contacts: with 0 the bodies bounce, with larger values they sink into each other slowly |
| `contactDamping` | 0.0 | Fraction of their relative velocity that bodies in contact lose per unit of time, on top of the above. Not needed for soft bodies |
| `contactDistance` | 0.01 | Distance at which the surfaces of two bodies are kept apart |
| `recoverySpeed` | 1.0 | Speed at which bodies that are deeper into each other, or into the ground, than their surfaces can give are separated |
| `damping` | 0.0 | Fraction of the velocity that every body loses per unit of time |

## Tests

The tests run small scenes without opening a window and check that the bodies
behave: a ball on the ground stays at rest and flattens as much as its weight
and stiffness say, a dropped ball lands and stops, a
ball that starts under the ground comes out, two balls pushed together stay
apart, momentum is conserved in a collision, and a pile comes to rest. Each
scene runs on the mesh of the examples (42 vertices) and on a finer sphere
(642 vertices).

```
cd build
ctest
```

## Writing a simulation

A program supplies `setup`, `update` and `draw` functions to `SoftmatSim` and
can register an `onContact` listener; see `examples/softcollision/colliding.cpp`
for the smallest complete case.

## Layout

- `src/core`: bodies, meshes, the position-based dynamics solver and the shape-matching constraint
- `src/collisions`: contact detection with a spatial hash, the collision constraints and the contact list
- `src/visual`: OpenGL view and video recorder
- `src/util`: configuration, mesh loading, geometry and OpenGL helpers
- `tests`: headless tests of the dynamics
- `shaders`, `res/models`: shaders and two sphere meshes, with 42 and 642 vertices
- `third_party`: the Khronos `GL3/gl3.h` header
