## Vcsn, a generic library for finite state machines.
## Copyright (C) 2012-2017 Vcsn Group.
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

# Not check_PROGRAMS, see below why.
EXTRA_PROGRAMS +=                               \
  %D%/aut_lal_char_z                            \
  %D%/aut_lao_z                                 \
  %D%/aut_law_char_z                            \
  %D%/concat                                    \
  %D%/cross                                     \
  %D%/distance                                  \
  %D%/dyn                                       \
  %D%/label                                     \
  %D%/polynomialset                             \
  %D%/proper                                    \
  %D%/transpose                                 \
  %D%/weight                                    \
  %D%/zip                                       \
  %D%/zip-maps

unit_ldadd = $(libvcsn) $(RT_LIBS)

noinst_HEADERS = %D%/test.hh %D%/weight.hh

%C%_aut_lal_char_z_LDADD = $(unit_ldadd)
%C%_aut_lao_z_LDADD      = $(unit_ldadd)
%C%_aut_law_char_z_LDADD = $(unit_ldadd)
%C%_concat_LDADD         = $(unit_ldadd)
%C%_distance_LDADD       = $(unit_ldadd)
%C%_dyn_LDADD            = $(unit_ldadd)
%C%_label_LDADD          = $(unit_ldadd)
%C%_polynomialset_LDADD  = $(unit_ldadd)
%C%_proper_LDADD         = $(unit_ldadd)
%C%_transpose_LDADD      = $(unit_ldadd)
%C%_weight_LDADD         = $(unit_ldadd)

%C%_TESTS =                                     \
  %D%/aut_lal_char_z.chk                        \
  %D%/aut_lao_z.chk                             \
  %D%/aut_law_char_z.chk                        \
  %D%/concat.chk                                \
  %D%/cross.chk                                 \
  %D%/dyn.chk                                   \
  %D%/ipython.chk                               \
  %D%/label.chk                                 \
  %D%/polynomialset.chk                         \
  %D%/proper.chk                                \
  %D%/pylint.chk                                \
  %D%/score.chk                                 \
  %D%/score-compare.chk                         \
  %D%/transpose.chk                             \
  %D%/weight.chk                                \
  %D%/zip-maps.chk                              \
  %D%/zip.chk

dist_TESTS += $(%C%_TESTS)

EXTRA_DIST +=                                   \
  %D%/score-compare.dir/all.csv                 \
  %D%/score-compare.dir/all.tex                 \
  %D%/score-compare.dir/all.txt                 \
  %D%/score-compare.dir/default.txt             \
  %D%/score-compare.dir/shortest.txt            \
  %D%/score-compare.dir/v2.0-0001-g6bfe026      \
  %D%/score-compare.dir/v2.0-0423-g841c067      \
  %D%/score-compare.dir/v2.0-1255-gafaf339      \
  %D%/score-compare.dir/v2.1-0010-gd55ef26

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
%D%/aut_lal_char_z.log: %D%/aut_lal_char_z
%D%/aut_lao_z.log:      %D%/aut_lao_z
%D%/aut_law_char_z.log: %D%/aut_law_char_z
%D%/concat.log:         %D%/concat
%D%/cross.log:          %D%/cross
%D%/distance.log:       %D%/distance
%D%/dyn.log:            %D%/dyn
%D%/ipython.log:        $(vcsn_python)
%D%/label.log:          %D%/label
%D%/polynomialset.log:  %D%/polynomialset
%D%/proper.log:         %D%/proper
%D%/pylint.log:         $(vcsn_python) $(vcsn_python_pylint)
%D%/score-compare.log:  $(wildcard $(srcdir)/%D%/score-compare.dir/*) $(top_srcdir)/libexec/vcsn-score-compare
%D%/score.log:          $(VCSN_PYTHON_DEPS) $(top_srcdir)/libexec/vcsn-score
%D%/transpose.log:      %D%/transpose
%D%/weight.log:         %D%/weight
%D%/zip-maps.log:       %D%/zip-maps
%D%/zip.log:            %D%/zip

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(%C%_TESTS)'
