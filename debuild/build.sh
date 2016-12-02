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
./configure CXXFLAGS='-O3' CPPFLAGS='-DNDEBUG'
# Make sure the package is complete: run distcheck first.  Prefer
# vcsn-distcheck which skips the build-check, but run the
# install-check.
pass=true
# Run VERBOSE so that we display test-suite.log.
make -j${NBPROC:-2} VERBOSE=1 vcsn-distcheck || pass=false
find . -name '*.log'
find . -name '*.log' -exec tar cvf - {} '+' | (cd "$package_dir" && tar xvf -)
$pass

make deb
mv vcsn_* "$package_dir"
