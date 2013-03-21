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
RAT_LOG_COMPILER = $(srcdir)/rat/rat
AM_RAT_LOG_DEPS =				\
  $(RAT_LOG_COMPILER)				\
  $(all_vcsn)					\
  rat/common.rat rat/common-weights.rat
$(rat_TESTS:.rat=.log): $(AM_RAT_LOG_DEPS)

rat_TESTS =                                     \
  $(rat_XFAIL_TESTS)                            \
  rat/interface.rat                             \
  rat/b.rat                                     \
  rat/br.rat                                    \
  rat/z.rat                                     \
  rat/zr.rat                                    \
  rat/zrr.rat

rat/vcsn-transpose.log: $(top_builddir)/bin/vcsn-transpose

dist_TESTS += $(rat_TESTS)
EXTRA_DIST += $(AM_RAT_LOG_DEPS)

rat_XFAIL_TESTS =                               \
  rat/wrong-weight-set.rat

XFAIL_TESTS += $(rat_XFAIL_TESTS)

.PHONY: check-rat
check-rat:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(rat_TESTS)'
