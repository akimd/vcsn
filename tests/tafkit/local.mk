## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2013-2016 Vaucanson Group.
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

%C%_TESTS =                \
  %D%/eval.chk             \
  %D%/help.chk

dist_TESTS += $(%C%_TESTS)

$(%C%_TESTS:.chk=.log): libexec/vcsn-tafkit

.PHONY: check-tafkit
check-tafkit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(%C%_TESTS)'
