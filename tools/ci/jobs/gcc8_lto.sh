#!/usr/bin/env bash

export CC=gcc-8
export CXX=g++-8

if [ "$NEWCC" != "" ]; then
    export CC="$NEWCC"
fi
if [ "$NEWCXX" != "" ]; then
    export CXX="$NEWCXX"
fi

export LOGFILE=gcc8.log

source ./tools/ci/scripts/init.sh

export CXXFLAGS="$CXXFLAGS -ggdb3 -pipe -ffast-math -O9 -flto -fwhole-program \
-fno-omit-frame-pointer -funswitch-loops -D_FORTIFY_SOURCE=2 -std=gnu++2a \
-Wno-attributes -fno-omit-frame-pointer"

source ./tools/ci/flags/gcc8.sh

export CXXFLAGS="$CXXFLAGS $POST_CXXFLAGS"

do_init
run_configure --enable-werror $*
run_make

source ./tools/ci/scripts/exit.sh

exit 0
