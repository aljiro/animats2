#!/bin/sh
if $1 == 'true'; then
    echo "+++ Reconfiguring the project"
    rm -r build
    mkdir build
fi

cd build

if $1 == 'true'; then
    echo "+++ Reconfiguring the project"
    cmake ..
fi

make