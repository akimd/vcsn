#!/bin/bash

set -e
set -x

cp -a /build/vcsn /build/vcsn-copy/
cd /build/vcsn-copy
./bootstrap
test -d /build/vcsn/_package || mkdir /build/vcsn/_package
cd /build/vcsn/_package
/build/vcsn-copy/configure CXXFLAGS='-O3' CPPFLAGS='-DNDEBUG' CC=gcc-4.9 CXX=g++-4.9
make deb

