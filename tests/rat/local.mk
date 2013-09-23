# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012-2013 Vaucanson Group.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `COPYING' file in the root directory.
#
# The Vaucanson Group consists of people listed in the `AUTHORS' file.

TEST_EXTENSIONS += .rat
RAT_LOG_COMPILER = $(srcdir)/%D%/rat
AM_RAT_LOG_DEPS =				\
  $(RAT_LOG_COMPILER)				\
  $(all_vcsn)					\
  %D%/common.rat %D%/common-weights.rat
$(%C%_TESTS:.rat=.log): $(AM_RAT_LOG_DEPS)

%C%_TESTS =                                     \
  $(%C%_XFAIL_TESTS)                            \
  %D%/interface.rat                             \
  %D%/b.rat                                     \
  %D%/br.rat                                    \
  %D%/z.rat                                     \
  %D%/zr.rat                                    \
  %D%/zrr.rat

dist_TESTS += $(%C%_TESTS)
EXTRA_DIST += $(AM_RAT_LOG_DEPS)

%C%_XFAIL_TESTS =                               \
  %D%/wrong-weight-set.rat

XFAIL_TESTS += $(%C%_XFAIL_TESTS)

.PHONY: check-%D%
check-%D%:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(%C%_TESTS)'
