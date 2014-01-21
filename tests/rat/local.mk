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
RAT_LOG_DRIVER = $(TAP_DRIVER) $(srcdir)/tests/bin/rat.py
AM_RAT_LOG_DEPS =				\
  $(RAT_LOG_DRIVER)				\
  $(VCSN_PYTHON_DEPS)				\
  %D%/common.rat %D%/common-weights.rat
$(%C%_TESTS:.rat=.log): $(AM_RAT_LOG_DEPS)

%C%_TESTS =                                     \
  $(%C%_XFAIL_TESTS)                            \
  %D%/b.rat                                     \
  %D%/br.rat                                    \
  %D%/z.rat                                     \
  %D%/zrr.rat

dist_TESTS += $(%C%_TESTS)
EXTRA_DIST += $(AM_RAT_LOG_DEPS)

# These tests check that the test suite does catch errors.
%C%_XFAIL_TESTS =                               \
  %D%/xfail1.rat                                \
  %D%/xfail2.rat                                \
  %D%/xfail3.rat

XFAIL_TESTS += $(%C%_XFAIL_TESTS)

.PHONY: check-rat
check-rat:
# Depend on check-TESTS, not 'check', because the latter depends on
# "all", which compiles TAF-Kit etc.  Leave "check" for simple users.
# Here, we know that our dependencies are right, so use check-TESTS.
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS TESTS='$(%C%_TESTS)'
