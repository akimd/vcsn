
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
  %D%/compose.py                                \
  %D%/concatenate.py                            \
  %D%/constant-term.py                          \
  %D%/context.py                                \
  %D%/derivation.py                             \
  %D%/determinize.py                            \
  %D%/difference.py                             \
  %D%/efsm.py                                   \
  %D%/eliminate-state.py                        \
  %D%/enumerate.py                              \
  %D%/evaluate.py                               \
  %D%/expand.py                                 \
  %D%/factory.py                                \
  %D%/first-order.py                            \
  %D%/has-twins-property.py                     \
  %D%/infiltration.py                           \
  %D%/info.py                                   \
  %D%/insplit.py                                \
  %D%/is-ambiguous.py                           \
  %D%/is-complete.py                            \
  %D%/is-deterministic.py                       \
  %D%/is-equivalent.py                          \
  %D%/is-isomorphic.py                          \
  %D%/is-normalized.py                          \
  %D%/is-proper.py                              \
  %D%/is-valid.py                               \
  %D%/kmul.py                                   \
  %D%/label.py                                  \
  %D%/lift.py                                   \
  %D%/minimize.py                               \
  %D%/prefix.py                                 \
  %D%/polynomial.py                             \
  %D%/power.py                                  \
  %D%/product.py                                \
  %D%/proper.py                                 \
  %D%/ratexp.py                                 \
  %D%/reduce.py                                 \
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
  %D%/tuples.py                                 \
  %D%/union.py                                  \
  %D%/universal.py                              \
  %D%/weight.py

$(%C%_TESTS:.py=.log): $(VCSN_PYTHON_DEPS) $(srcdir)/tests/bin/test.py

dist_TESTS += $(%C%_TESTS)

EXTRA_DIST +=                                   \
  %D%/aut-to-exp.dir/a.gv                       \
  %D%/automaton.dir/a.grail                     \
  %D%/automaton.dir/a.gv                        \
  %D%/automaton.dir/dfa.fado                    \
  %D%/automaton.dir/dfa.gv                      \
  %D%/automaton.dir/enfa.fado                   \
  %D%/automaton.dir/enfa.gv                     \
  %D%/automaton.dir/lal_char_b.daut             \
  %D%/automaton.dir/lal_char_b.in.gv            \
  %D%/automaton.dir/lal_char_b.out.gv           \
  %D%/automaton.dir/lal_char_b.tex              \
  %D%/automaton.dir/lal_char_q.daut             \
  %D%/automaton.dir/lal_char_q.in.gv            \
  %D%/automaton.dir/lal_char_q.out.gv           \
  %D%/automaton.dir/lal_char_q.tex              \
  %D%/automaton.dir/lan_char_b.daut             \
  %D%/automaton.dir/lan_char_b.in.gv            \
  %D%/automaton.dir/lan_char_b.out.gv           \
  %D%/automaton.dir/lan_char_b.tex              \
  %D%/automaton.dir/lao_z.daut                  \
  %D%/automaton.dir/lao_z.in.gv                 \
  %D%/automaton.dir/lao_z.out.gv                \
  %D%/automaton.dir/lao_z.tex                   \
  %D%/automaton.dir/nfa.fado                    \
  %D%/automaton.dir/nfa.gv                      \
  %D%/automaton.dir/pyramid.daut                \
  %D%/automaton.dir/pyramid.in.gv               \
  %D%/automaton.dir/pyramid.out.gv              \
  %D%/automaton.dir/pyramid.tex                 \
  %D%/compose.dir/left.gv                       \
  %D%/compose.dir/result.gv                     \
  %D%/compose.dir/right.gv                      \
  %D%/derivation.dir/e1-dt.gv                   \
  %D%/derivation.dir/e2-dt-breaking.gv          \
  %D%/derivation.dir/e2-dt.gv                   \
  %D%/derivation.dir/h3-dt-breaking.gv          \
  %D%/derivation.dir/h3-dt.gv                   \
  %D%/determinize.dir/b-det.gv                  \
  %D%/determinize.dir/b.gv                      \
  %D%/determinize.dir/de-bruijn-3-det.gv        \
  %D%/determinize.dir/de-bruijn-8-det.gv        \
  %D%/determinize.dir/deterministic-det.gv      \
  %D%/determinize.dir/deterministic.gv          \
  %D%/determinize.dir/empty-det.gv              \
  %D%/determinize.dir/empty.gv                  \
  %D%/determinize.dir/epsilon-det.gv            \
  %D%/determinize.dir/epsilon.gv                \
  %D%/determinize.dir/ladybird-4-det.gv         \
  %D%/determinize.dir/ladybird-8-det.gv         \
  %D%/determinize.dir/q-det.gv                  \
  %D%/determinize.dir/q.gv                      \
  %D%/determinize.dir/zmin-det.gv               \
  %D%/determinize.dir/zmin.gv                   \
  %D%/efsm.dir/a1.efsm                          \
  %D%/efsm.dir/a2x.efsm                         \
  %D%/efsm.dir/a2xyz.efsm                       \
  %D%/efsm.dir/abs.efsm                         \
  %D%/efsm.dir/binary.efsm                      \
  %D%/efsm.dir/one.efsm                         \
  %D%/efsm.dir/str.efsm                         \
  %D%/eliminate-state.dir/lao-elim-0.gv         \
  %D%/eliminate-state.dir/lao-elim-1.gv         \
  %D%/eliminate-state.dir/lao-elim-2.gv         \
  %D%/eliminate-state.dir/lao.gv                \
  %D%/factory.dir/cerny-6.gv                    \
  %D%/factory.dir/clique-a-4.gv                 \
  %D%/factory.dir/de-bruijn-2.gv                \
  %D%/factory.dir/de-bruijn-3.gv                \
  %D%/factory.dir/double-ring-1-0.gv            \
  %D%/factory.dir/double-ring-4-2-3.gv          \
  %D%/factory.dir/ladybird-2-zmin.gv            \
  %D%/factory.dir/ladybird-2.gv                 \
  %D%/factory.dir/u-5.gv                        \
  %D%/minimize.dir/incomplete-non-trim.gv       \
  %D%/minimize.dir/incomplete-non-trim.moore.gv \
  %D%/minimize.dir/intricate.exp.gv             \
  %D%/minimize.dir/no-final-states.gv           \
  %D%/minimize.dir/no-final-states.moore.gv     \
  %D%/minimize.dir/no-initial-states.gv         \
  %D%/minimize.dir/no-initial-states.moore.gv   \
  %D%/minimize.dir/nonlal.exp.gv                \
  %D%/minimize.dir/redundant.exp.gv             \
  %D%/minimize.dir/redundant.gv                 \
  %D%/minimize.dir/small-nfa.exp.gv             \
  %D%/minimize.dir/small-z.exp.gv               \
  %D%/minimize.dir/small-z.gv                   \
  %D%/power.dir/binary^0.gv                     \
  %D%/power.dir/binary^1.gv                     \
  %D%/prefix.dir/aut1-factor.gv                 \
  %D%/prefix.dir/aut1-prefix.gv                 \
  %D%/prefix.dir/aut1-subword.gv                \
  %D%/prefix.dir/aut1-suffix.gv                 \
  %D%/prefix.dir/aut1.gv                        \
  %D%/prefix.dir/aut2-factor.gv                 \
  %D%/prefix.dir/aut2-prefix.gv                 \
  %D%/prefix.dir/aut2-subword.gv                \
  %D%/prefix.dir/aut2-suffix.gv                 \
  %D%/prefix.dir/aut2.gv                        \
  %D%/prefix.dir/aut3-factor.gv                 \
  %D%/prefix.dir/aut3-prefix.gv                 \
  %D%/prefix.dir/aut3-subword.gv                \
  %D%/prefix.dir/aut3-suffix.gv                 \
  %D%/prefix.dir/aut3.gv                        \
  %D%/prefix.dir/aut4-factor.gv                 \
  %D%/prefix.dir/aut4-prefix.gv                 \
  %D%/prefix.dir/aut4-subword.gv                \
  %D%/prefix.dir/aut4-suffix.gv                 \
  %D%/prefix.dir/aut4.gv                        \
  %D%/product.dir/a.gv                          \
  %D%/product.dir/ab.gv                         \
  %D%/product.dir/abc.gv                        \
  %D%/product.dir/ax.gv                         \
  %D%/product.dir/x.gv                          \
  %D%/product.dir/xa.gv                         \
  %D%/product.dir/xy.gv                         \
  %D%/product.dir/xyz.gv                        \
  %D%/union.dir/abc.gv

%D%/aut-to-exp.log:      $(wildcard $(srcdir)/%D%/aut-to-exp.dir/*)
%D%/automaton.log:       $(wildcard $(srcdir)/%D%/automaton.dir/*)
%D%/compose.log:         $(wildcard $(srcdir)/%D%/compose/*)
%D%/derivation.log:      $(wildcard $(srcdir)/%D%/derivation.dir/*)
%D%/determinize.log:     $(wildcard $(srcdir)/%D%/determinize.dir/*)
%D%/eliminate-state.log: $(wildcard $(srcdir)/%D%/eliminate-state.dir/*)
%D%/efsm.log:            $(wildcard $(srcdir)/%D%/efsm.dir/*)
%D%/factory.log:         $(wildcard $(srcdir)/%D%/factory.dir/*)
%D%/infiltration.log:    $(wildcard $(srcdir)/%D%/product.dir/*)
%D%/minimize.log:        $(wildcard $(srcdir)/%D%/minimize.dir/*)
%D%/power.log:           $(wildcard $(srcdir)/%D%/power.dir/*)
%D%/prefix.log:          $(wildcard $(srcdir)/%D%/prefix.dir/*)
%D%/product.log:         $(wildcard $(srcdir)/%D%/product.dir/*)
%D%/union.log:           $(wildcard $(srcdir)/%D%/union.dir/*)

.PHONY: check-python
check-python:
# Depend on check-TESTS, not 'check', because the latter depends on
# "all", which compiles TAF-Kit etc.  Leave "check" for simple users.
# Here, we know that our dependencies are right, so use check-TESTS.
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS TESTS='$(%C%_TESTS)'
