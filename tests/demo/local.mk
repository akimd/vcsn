# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2013 Vaucanson Group.
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

# Not check_PROGRAMS, see below why.
EXTRA_PROGRAMS +=                               \
  demo/prod-eval

demo_prod_eval_LDADD = $(all_libctx) $(AM_LDADD)

demo_TESTS =                                    \
  demo/prod-eval.chk

dist_TESTS += $(demo_TESTS)

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
demo/prod-eval.log: demo/prod-eval

.PHONY: check-demo
check-demo:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(demo_TESTS)'
