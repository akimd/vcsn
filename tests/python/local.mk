# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2013, 2014 Vaucanson Group.
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

TEST_EXTENSIONS += .py
PY_LOG_DRIVER = $(TAP_DRIVER)

%C%_TESTS =                                     \
  %D%/accessible.py                             \
  %D%/aut-to-exp.py                             \
  %D%/constant-term.py                          \
  %D%/derivation.py                             \
  %D%/difference.py                             \
  %D%/eliminate-state.py                        \
  %D%/expand.py                                 \
  %D%/factory.py                                \
  %D%/infiltration.py                           \
  %D%/info.py                                   \
  %D%/is-equivalent.py                          \
  %D%/is-isomorphic.py                          \
  %D%/minimize.py                               \
  %D%/product.py                                \
  %D%/ratexp.py                                 \
  %D%/shuffle.py                                \
  %D%/split.py                                  \
  %D%/star-height.py                            \
  %D%/star.py

if HAVE_CORRECT_LIST_INITIALIZER_ORDER
%C%_TESTS +=                                    \
  %D%/context.py                                \
  %D%/tuples.py
endif

EXTRA_DIST += %D%/test.py
$(%C%_TESTS:.py=.log): $(VCSN_PYTHON_DEPS) %D%/test.py

dist_TESTS += $(%C%_TESTS)

EXTRA_DIST +=                                   \
  %D%/aut-to-exp.dir/a.gv                       \
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
  %D%/product.dir/a.gv                          \
  %D%/product.dir/ab.gv                         \
  %D%/product.dir/abc.gv                        \
  %D%/product.dir/ax.gv                         \
  %D%/product.dir/x.gv                          \
  %D%/product.dir/xa.gv                         \
  %D%/product.dir/xy.gv                         \
  %D%/product.dir/xyz.gv

%D%/aut-to-exp.log: $(wildcard $(srcdir)/%D%/aut-to-exp.dir/*)
%D%/factory.log: $(wildcard $(srcdir)/%D%/factory.dir/*)
%D%/eliminate-state.log: $(wildcard $(srcdir)/%D%/eliminate-state.dir/*)
%D%/infiltration.log: $(wildcard $(srcdir)/%D%/product.dir/*)
%D%/minimize.log: $(wildcard $(srcdir)/%D%/minimize.dir/*)
%D%/product.log: $(wildcard $(srcdir)/%D%/product.dir/*)

.PHONY: check-python
check-python:
# Depend on check-TESTS, not 'check', because the latter depends on
# "all", which compiles TAF-Kit etc.  Leave "check" for simple users.
# Here, we know that our dependencies are right, so use check-TESTS.
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS TESTS='$(%C%_TESTS)'
