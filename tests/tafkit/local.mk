## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2012-2014 Vaucanson Group.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## The complete GNU General Public Licence Notice can be found as the
## `COPYING' file in the root directory.
##
## The Vaucanson Group consists of people listed in the `AUTHORS' file.

%C%_TESTS =                                     \
  %D%/efsm.chk                                  \
  %D%/lift.chk                                  \
  %D%/transpose.chk

dist_TESTS += $(%C%_TESTS)

EXTRA_DIST +=                                   \
  %D%/efsm.dir/a1.efsm                          \
  %D%/efsm.dir/binary.efsm

%D%/efsm.log:        $(wildcard $(srcdir)/%D%/efsm.dir/*)

# Very coarse grain: we compile the whole TAF-Kit even if we just run
# one test which requires only a single TAF-Kit command.  Fine grain
# is dangerous if we forget some dependencies.  We need something
# automated.
$(%C%_TESTS:.chk=.log): $(bin_PROGRAMS) $(dist_bin_SCRIPTS) $(nodist_bin_SCRIPTS)

.PHONY: check-tafkit
check-tafkit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(%C%_TESTS)'
