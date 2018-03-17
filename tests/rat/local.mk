## Vcsn, a generic library for finite state machines.
## Copyright (C) 2012-2018 Vcsn Group.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## The complete GNU General Public Licence Notice can be found as the
## `COPYING' file in the root directory.
##
## The Vcsn Group consists of people listed in the `AUTHORS' file.

TEST_EXTENSIONS += .rat
RAT_LOG_DRIVER = $(TAP_DRIVER)
RAT_LOG_COMPILER = vcsn run $(PYTHON) $(srcdir)/tests/bin/rat.py
AM_RAT_LOG_DEPS =                               \
  $(srcdir)/tests/bin/rat.py                    \
  $(VCSN_PYTHON_DEPS)                           \
  $(srcdir)/tests/bin/test.py                   \
  %D%/common.rat %D%/common-weights.rat
$(%C%_TESTS:.rat=.log): $(AM_RAT_LOG_DEPS)

vcsn_python +=					\
  $(srcdir)/tests/bin/rat.py			\
  $(srcdir)/tests/bin/test.py

%C%_TESTS =                                     \
  $(%C%_XFAIL_TESTS)                            \
  %D%/b.rat                                     \
  %D%/q.rat                                     \
  %D%/qr.rat                                    \
  %D%/qrr.rat                                   \
  %D%/tuple.rat                                 \
  %D%/utf-8.rat

dist_TESTS += $(%C%_TESTS)
EXTRA_DIST +=                                   \
  $(srcdir)/tests/bin/rat.py                    \
  %D%/common.rat %D%/common-weights.rat

# These tests check that the test suite does catch errors.
%C%_XFAIL_TESTS =                               \
  %D%/xfail1.rat                                \
  %D%/xfail2.rat                                \
  %D%/xfail3.rat

XFAIL_TESTS += $(%C%_XFAIL_TESTS)

.PHONY: check-rat
check-rat:
# Depend on check-TESTS, not 'check', because the latter depends on
# "all", which compiles Tools etc.  Leave "check" for simple users.
# Here, we know that our dependencies are right, so use check-TESTS.
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS TESTS='$(%C%_TESTS)'
