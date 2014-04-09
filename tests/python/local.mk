## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2013, 2014 Vaucanson Group.
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

TEST_EXTENSIONS += .py
PY_LOG_DRIVER = $(TAP_DRIVER)

%C%_TESTS =                                     \
  %D%/accessible.py                             \
  %D%/aut-to-exp.py                             \
  %D%/automaton.py                              \
  %D%/chain.py                                  \
  %D%/complement.py                             \
  %D%/complete.py                               \
  %D%/concatenate.py                            \
  %D%/constant-term.py                          \
  %D%/context.py                                \
  %D%/de-bruijn.py                              \
  %D%/derivation.py                             \
  %D%/determinize.py                            \
  %D%/difference.py                             \
  %D%/double-ring.py                            \
  %D%/eliminate-state.py                        \
  %D%/enumerate.py                              \
  %D%/evaluate.py                               \
  %D%/expand.py                                 \
  %D%/factory.py                                \
  %D%/first-order.py                            \
  %D%/infiltration.py                           \
  %D%/info.py                                   \
  %D%/is-ambiguous.py                           \
  %D%/is-complete.py                            \
  %D%/is-deterministic.py                       \
  %D%/is-equivalent.py                          \
  %D%/is-isomorphic.py                          \
  %D%/is-normalized.py                          \
  %D%/is-proper.py                              \
  %D%/is-valid.py                               \
  %D%/kmul.py                                   \
  %D%/ladybird.py                               \
  %D%/minimize.py                               \
  %D%/polynomial.py                             \
  %D%/power.py                                  \
  %D%/product.py                                \
  %D%/proper.py	                                \
  %D%/ratexp.py                                 \
  %D%/shuffle.py                                \
  %D%/sort.py                                   \
  %D%/split.py                                  \
  %D%/standard.py                               \
  %D%/star-height.py                            \
  %D%/star-normal-form.py                       \
  %D%/star.py                                   \
  %D%/synchronizing-word.py                     \
  %D%/sum.py                                    \
  %D%/thompson.py                               \
  %D%/transpose.py                              \
  %D%/union.py                                  \
  %D%/universal.py                              \
  %D%/weight.py

if HAVE_CORRECT_LIST_INITIALIZER_ORDER
%C%_TESTS +=                                    \
  %D%/tuples.py
endif

EXTRA_DIST += %D%/test.py
$(%C%_TESTS:.py=.log): $(VCSN_PYTHON_DEPS) %D%/test.py

dist_TESTS += $(%C%_TESTS)

EXTRA_DIST +=                                   \
  %D%/aut-to-exp.dir/a.gv                       \
  %D%/automaton.dir/lal_char_b.in.gv            \
  %D%/automaton.dir/lal_char_b.out.gv           \
  %D%/automaton.dir/lal_char_b.tex              \
  %D%/automaton.dir/lan_char_b.in.gv            \
  %D%/automaton.dir/lan_char_b.out.gv           \
  %D%/automaton.dir/lan_char_b.tex              \
  %D%/automaton.dir/lao_z.in.gv                 \
  %D%/automaton.dir/lao_z.out.gv                \
  %D%/automaton.dir/lao_z.tex                   \
  %D%/automaton.dir/pyramid.in.gv               \
  %D%/automaton.dir/pyramid.out.gv              \
  %D%/automaton.dir/pyramid.tex                 \
  %D%/determinize.dir/de-bruijn-3-det.gv        \
  %D%/determinize.dir/de-bruijn-8-det.gv        \
  %D%/determinize.dir/ladybird-4-det.gv         \
  %D%/determinize.dir/ladybird-8-det.gv         \
  %D%/eliminate-state.dir/lao-elim-0.gv         \
  %D%/eliminate-state.dir/lao-elim-1.gv         \
  %D%/eliminate-state.dir/lao-elim-2.gv         \
  %D%/eliminate-state.dir/lao.gv                \
  %D%/factory.dir/clique-a-4.gv                 \
  %D%/minimize.dir/incomplete-non-trim.gv       \
  %D%/minimize.dir/incomplete-non-trim.moore.gv \
  %D%/minimize.dir/intricate.exp.gv             \
  %D%/minimize.dir/no-final-states.gv           \
  %D%/minimize.dir/no-final-states.moore.gv     \
  %D%/minimize.dir/no-initial-states.gv         \
  %D%/minimize.dir/no-initial-states.moore.gv   \
  %D%/minimize.dir/redundant.exp.gv             \
  %D%/minimize.dir/redundant.gv                 \
  %D%/minimize.dir/small-nfa.exp.gv             \
  %D%/minimize.dir/small-weighted.exp.gv        \
  %D%/minimize.dir/small-weighted.gv            \
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
  %D%/union.dir/abc.gv

%D%/aut-to-exp.log:  $(wildcard $(srcdir)/%D%/aut-to-exp.dir/*)
%D%/automaton.log: $(wildcard $(srcdir)/%D%/aut-to-exp.dir/*)
%D%/factory.log: $(wildcard $(srcdir)/%D%/factory.dir/*)
%D%/eliminate-state.log: $(wildcard $(srcdir)/%D%/eliminate-state.dir/*)
%D%/infiltration.log: $(wildcard $(srcdir)/%D%/product.dir/*)
%D%/minimize.log: $(wildcard $(srcdir)/%D%/minimize.dir/*)
%D%/power.log:       $(wildcard $(srcdir)/%D%/power.dir/*)
%D%/product.log: $(wildcard $(srcdir)/%D%/product.dir/*)
%D%/union.log:       $(wildcard $(srcdir)/%D%/union.dir/*)

.PHONY: check-python
check-python:
# Depend on check-TESTS, not 'check', because the latter depends on
# "all", which compiles TAF-Kit etc.  Leave "check" for simple users.
# Here, we know that our dependencies are right, so use check-TESTS.
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS TESTS='$(%C%_TESTS)'
