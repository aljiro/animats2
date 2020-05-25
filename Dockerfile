FROM ubuntu:bionic
ENV LD_LIBRARY_PATH /usr/local/lib
ARG EXPERIMENT
ARG RECONFIGURE
ARG DEBUG
ARG DEBUG_LOOPS
ARG DEBUG_ORIGIN

# Install prerequisites
RUN apt-get update && apt-get install -y g++ make
RUN apt-get install -y build-essential libgl1-mesa-dev libglew-dev libglew2.0 libglm-dev libglfw3-dev libglfw3
RUN apt-get install -y liblapack-dev libblas-dev libboost-dev libarmadillo-dev cmake

WORKDIR /code

COPY . .
# Compile
RUN ./build_project.sh $RECONFIGURE
CMD ["./animats", "$EXPERIMENT --debug $DEBUG --debug-loops $DEBUG_LOOPS"]