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

tafkit_TESTS =                                  \
  tafkit/accessible.chk                         \
  tafkit/are-equivalent.chk                     \
  tafkit/aut_to_exp.chk                         \
  tafkit/cat.chk                                \
  tafkit/complement.chk                         \
  tafkit/complete.chk                           \
  tafkit/constant-term.chk                      \
  tafkit/de-bruijn.chk                          \
  tafkit/determinize.chk                        \
  tafkit/efsm.chk                               \
  tafkit/enumerate.chk                          \
  tafkit/eval.chk                               \
  tafkit/grail.chk                              \
  tafkit/is-deterministic.chk                   \
  tafkit/is-normalized.chk                      \
  tafkit/is-proper.chk                          \
  tafkit/is_complete.chk                        \
  tafkit/ladybird.chk                           \
  tafkit/lift.chk                               \
  tafkit/power.chk                              \
  tafkit/product.chk                            \
  tafkit/proper.chk                             \
  tafkit/shortest.chk                           \
  tafkit/standard.chk                           \
  tafkit/thompson.chk                           \
  tafkit/transpose.chk                          \
  tafkit/universal.chk

dist_TESTS += $(tafkit_TESTS)

$(tafkit_TESTS:.chk=.log): $(all_vcsn)

.PHONY: check-tafkit
check-tafkit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(tafkit_TESTS)'
