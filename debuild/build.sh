#!/bin/bash

set -e
set -x

inside_vcsn="/build/vcsn-copy"
outside_vcsn="/build/vcsn"
build_dir="$inside_vcsn/_build"
package_dir="$outside_vcsn/_package"

cp -a "$outside_vcsn" "$inside_vcsn"

cd "$outside_vcsn"
./bootstrap

test -d "$build_dir" || mkdir "$build_dir"
cd "$inside_vcsn"
./configure CXXFLAGS='-O3' CPPFLAGS='-DNDEBUG' CC=gcc-4.9 CXX=g++-4.9
make deb

mv vcsn_* "$package_dir"
