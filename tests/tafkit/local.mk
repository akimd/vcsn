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

%C%_TESTS =                                     \
  %D%/accessible.chk                            \
  %D%/are-equivalent.chk                        \
  %D%/aut_to_exp.chk                            \
  %D%/cat.chk                                   \
  %D%/chain.chk                                 \
  %D%/complement.chk                            \
  %D%/complete.chk                              \
  %D%/concatenate.chk                           \
  %D%/constant-term.chk                         \
  %D%/de-bruijn.chk                             \
  %D%/determinize.chk                           \
  %D%/double-ring.chk                           \
  %D%/efsm.chk                                  \
  %D%/enumerate.chk                             \
  %D%/evaluate.chk                              \
  %D%/fado.chk                                  \
  %D%/grail.chk                                 \
  %D%/is-ambiguous.chk                          \
  %D%/is-complete.chk                           \
  %D%/is-deterministic.chk                      \
  %D%/is-normalized.chk                         \
  %D%/is-proper.chk                             \
  %D%/is-valid.chk                              \
  %D%/ladybird.chk                              \
  %D%/left-mult.chk                             \
  %D%/lift.chk                                  \
  %D%/power.chk                                 \
  %D%/product.chk                               \
  %D%/proper.chk                                \
  %D%/shortest.chk                              \
  %D%/standard.chk                              \
  %D%/star.chk                                  \
  %D%/sum.chk                                   \
  %D%/thompson.chk                              \
  %D%/transpose.chk                             \
  %D%/union.chk                                 \
  %D%/universal.chk

dist_TESTS += $(%C%_TESTS)

EXTRA_DIST +=                                   \
  %D%/determinize.dir/a-det.gv                  \
  %D%/determinize.dir/a.gv                      \
  %D%/determinize.dir/de-bruijn-3-det.gv        \
  %D%/determinize.dir/de-bruijn-8-det.gv        \
  %D%/determinize.dir/empty-det.gv              \
  %D%/determinize.dir/empty.gv                  \
  %D%/determinize.dir/epsilon-det.gv            \
  %D%/determinize.dir/epsilon.gv                \
  %D%/determinize.dir/ladybird-4-det.gv         \
  %D%/determinize.dir/ladybird-8-det.gv         \
  %D%/fado.dir/dfa.fado                         \
  %D%/fado.dir/dfa.gv                           \
  %D%/fado.dir/enfa.fado                        \
  %D%/fado.dir/enfa.gv                          \
  %D%/fado.dir/nfa.fado                         \
  %D%/fado.dir/nfa.gv

%D%/determinize.log: $(wildcard $(srcdir)/%D%/determinize.dir/*)
%D%/fado.log: $(wildcard $(srcdir)/%D%/fado.dir/*)

$(%C%_TESTS:.chk=.log): $(all_vcsn)

.PHONY: check-tafkit
check-tafkit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(%C%_TESTS)'
