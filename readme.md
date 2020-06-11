## Introduction
Soft body simulation engine.

## Compiling from scratch
### Dependencies

In order to compile the source code you will need: opengl, glew, lapack, glm, armadillo and openblass; apart from the 
conventional building tools and cmake.

```
apt-get update && apt-get install -y g++ make build-essential libglu1-mesa-dev \
libgl1-mesa-dev libglm-dev liblapack-dev \
libblas-dev libopenblas-dev libboost-dev libarmadillo-dev cmake libglew-dev libglew2.0
```
In order to get the las version of glfw3, we need to compile it from the sources:

```
git clone https://github.com/glfw/glfw.git  && cd glfw && cmake . && make install

```
### Building

Once you have all the requeriments, you can configure and build the project

```
mkdir build 
cd build
cmake ..
make
```

### Running
```
./animats <ExperimentDIR> [options]
```

## Running with docker