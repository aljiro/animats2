#!/bin/sh
echo "Reconfigure: $1"

if $1 == 'true'; then
    echo "+++ Reconfiguring the project"
    rm -r build
    mkdir build
fi

cd build

if $1 == 'true'; then
    echo "+++ Reconfiguring the project"
    cmake -D CMAKE_FIND_DEBUG_MODE=ON ..
fi

make