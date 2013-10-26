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
  %D%/aut-to-exp.chk                            \
  %D%/cat.chk                                   \
  %D%/chain.chk                                 \
  %D%/complement.chk                            \
  %D%/complete.chk                              \
  %D%/concatenate.chk                           \
  %D%/constant-term.chk                         \
  %D%/de-bruijn.chk                             \
  %D%/derivation.chk                            \
  %D%/determinize.chk                           \
  %D%/double-ring.chk                           \
  %D%/efsm.chk                                  \
  %D%/enumerate.chk                             \
  %D%/evaluate.chk                              \
  %D%/expand.chk                                \
  %D%/fado.chk                                  \
  %D%/grail.chk                                 \
  %D%/infiltration.chk                          \
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
  %D%/random.chk                                \
  %D%/shuffle.chk                               \
  %D%/standard.chk                              \
  %D%/star.chk                                  \
  %D%/star-normal-form.chk                      \
  %D%/sum.chk                                   \
  %D%/thompson.chk                              \
  %D%/transpose.chk                             \
  %D%/union.chk                                 \
  %D%/universal.chk

dist_TESTS += $(%C%_TESTS)

EXTRA_DIST +=                                   \
  %D%/aut-to-exp.dir/a.gv                       \
  %D%/aut-to-exp.dir/lao.gv                     \
  %D%/aut-to-exp.dir/lao-elim-0.gv              \
  %D%/aut-to-exp.dir/lao-elim-1.gv              \
  %D%/aut-to-exp.dir/lao-elim-2.gv              \
  %D%/derivation.dir/a?-dt.gv                   \
  %D%/derivation.dir/e1-dt.gv                   \
  %D%/derivation.dir/e2-dt.gv                   \
  %D%/derivation.dir/e2-dt-breaking.gv          \
  %D%/derivation.dir/h3-dt.gv                   \
  %D%/derivation.dir/h3-dt-breaking.gv          \
  %D%/determinize.dir/a-det-0.gv                \
  %D%/determinize.dir/a-det-1.gv                \
  %D%/determinize.dir/a.gv                      \
  %D%/determinize.dir/de-bruijn-3-det.gv        \
  %D%/determinize.dir/de-bruijn-8-det.gv        \
  %D%/determinize.dir/empty-det-0.gv            \
  %D%/determinize.dir/empty-det-1.gv            \
  %D%/determinize.dir/empty.gv                  \
  %D%/determinize.dir/epsilon-det-0.gv          \
  %D%/determinize.dir/epsilon-det-1.gv          \
  %D%/determinize.dir/epsilon.gv                \
  %D%/determinize.dir/ladybird-4-det.gv         \
  %D%/determinize.dir/ladybird-8-det.gv         \
  %D%/efsm.dir/a1.efsm                          \
  %D%/efsm.dir/binary.efsm                      \
  %D%/fado.dir/dfa.fado                         \
  %D%/fado.dir/dfa.gv                           \
  %D%/fado.dir/enfa.fado                        \
  %D%/fado.dir/enfa.gv                          \
  %D%/fado.dir/nfa.fado                         \
  %D%/fado.dir/nfa.gv                           \
  %D%/grail.dir/a.grail                         \
  %D%/grail.dir/a.gv                            \
  %D%/power.dir/binary^0.gv                     \
  %D%/power.dir/binary^1.gv                     \
  %D%/product.dir/a.gv                          \
  %D%/product.dir/ab.gv                         \
  %D%/product.dir/abc.gv                        \
  %D%/product.dir/ax.gv                         \
  %D%/product.dir/x.gv                          \
  %D%/product.dir/xa.gv                         \
  %D%/product.dir/xy.gv                         \
  %D%/product.dir/xyz.gv                        \
  %D%/random.dir/clique-a-4.gv

%D%/aut-to-exp.log:  $(wildcard $(srcdir)/%D%/aut-to-exp.dir/*)
%D%/derivation.log:  $(wildcard $(srcdir)/%D%/derivation.dir/*)
%D%/determinize.log: $(wildcard $(srcdir)/%D%/determinize.dir/*)
%D%/efsm.log:        $(wildcard $(srcdir)/%D%/efsm.dir/*)
%D%/fado.log:        $(wildcard $(srcdir)/%D%/fado.dir/*)
%D%/grail.log:       $(wildcard $(srcdir)/%D%/grail.dir/*)
%D%/infiltration.log: $(wildcard $(srcdir)/%D%/product.dir/*)
%D%/power.log:       $(wildcard $(srcdir)/%D%/power.dir/*)
%D%/random.log:      $(wildcard $(srcdir)/%D%/random.dir/*)

# Very coarse grain: we compile the whole TAF-Kit even if we just run
# one test which requires only a single TAF-Kit command.  Fine grain
# is dangerous if we forget some dependencies.  We need something
# automated.
$(%C%_TESTS:.chk=.log): $(all_vcsn) $(dist_bin_SCRIPTS)

.PHONY: check-tafkit
check-tafkit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(%C%_TESTS)'
