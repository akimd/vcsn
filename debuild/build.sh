#!/bin/bash

set -e
set -x

vcsn_dir="/build/vcsn"
package_dir="$vcsn_dir/_package"

cd "$vcsn_dir"
./bootstrap
./configure CXXFLAGS='-O3' CPPFLAGS='-DNDEBUG'
# Make sure the package is complete: run distcheck first.  Prefer
# vcsn-distcheck which skips the build-check, but run the
# install-check.
pass=true
# Run VERBOSE so that we display test-suite.log.
make -j${NBPROC:-2} VERBOSE=1 vcsn-distcheck || pass=false
find . -name '*.log' -exec tar cf - {} '+' | tar -C "$package_dir" -xvf -
$pass

make deb
mv vcsn_* vcsn-dbgsym_* "$package_dir"
