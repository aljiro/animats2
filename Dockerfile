FROM ubuntu:bionic

# ARG EXPERIMENT
ARG RECONFIGURE
ENV DISPLAY=:0
ENV RECONFIGURE=${RECONFIGURE}
# ARG DEBUG
# ARG DEBUG_LOOPS
# ARG DEBUG_ORIGIN

# Install prerequisites
RUN apt-get update && apt-get install -y g++ make build-essential libglu1-mesa-dev libgl1-mesa-dev libglm-dev liblapack-dev \
    libblas-dev libopenblas-dev libboost-dev libarmadillo-dev cmake libglew-dev libglew2.0 \
    libxmu-dev libxi-dev libgl-dev dos2unix git wget xorg-dev xvfb libgl1-mesa-glx mesa-utils && \
    git clone https://github.com/glfw/glfw.git /glfw && cd /glfw && cmake . && make install

#RUN apt-get install -y wget unzip
# RUN wget https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.tgz/download && tar -zxvf download && cd $(find . -maxdepth 1 -name "glew-*" -type d | sed 1q) && make install

WORKDIR /code

COPY . .
# Compile
# RUN ./build_project.sh $RECONFIGURE
#CMD ["./build/animats", "$EXPERIMENT --debug $DEBUG --debug-loops $DEBUG_LOOPS"]
# CMD ./build_project.sh ${RECONFIGURE} && cd build && \
#     ./animats ${EXPERIMENT} --debug ${DEBUG} --debug-loops ${DEBUG_LOOPS}

CMD ./build_project.sh ${RECONFIGURE}