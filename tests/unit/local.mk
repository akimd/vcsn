## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2012-2013 Vaucanson Group.
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

# Not check_PROGRAMS, see below why.
EXTRA_PROGRAMS +=                               \
  %D%/aut_lao_z                                 \
  %D%/aut_lal_char_z                            \
  %D%/aut_law_char_z                            \
  %D%/concat                                    \
  %D%/cross                                     \
  %D%/label                                     \
  %D%/polynomialset                             \
  %D%/proper                                    \
  %D%/transpose                                 \
  %D%/weight                                    \
  %D%/zip-maps

unit_ldadd = $(libvcsn)

noinst_HEADERS = %D%/test.hh %D%/weight.hh

%C%_aut_lao_z_LDADD      = $(liblao_z) $(unit_ldadd)
%C%_aut_lal_char_z_LDADD = $(liblal_char_z) $(unit_ldadd)
%C%_aut_law_char_z_LDADD = $(liblaw_char_z) $(unit_ldadd)
%C%_concat_LDADD         = $(liblaw_char_z) $(unit_ldadd)
%C%_label_LDADD          = $(liblal_char_b) $(unit_ldadd)
%C%_polynomialset_LDADD  = $(liblaw_char_z) $(unit_ldadd)
%C%_proper_LDADD         = $(all_libctx) $(unit_ldadd)
%C%_transpose_LDADD      = $(liblal_char_b) $(liblal_char_z) $(unit_ldadd)
%C%_weight_LDADD         = $(unit_ldadd)

%C%_TESTS =                                    \
  %D%/aut_lao_z.chk                            \
  %D%/aut_lal_char_z.chk                       \
  %D%/aut_law_char_z.chk                       \
  %D%/concat.chk                               \
  %D%/cross.chk                                \
  %D%/label.chk                                \
  %D%/polynomialset.chk                        \
  %D%/proper.chk                               \
  %D%/transpose.chk                            \
  %D%/weight.chk                               \
  %D%/zip-maps.chk

dist_TESTS += $(%C%_TESTS)

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
%D%/aut_lal_char_z.log: %D%/aut_lal_char_z
%D%/aut_lao_z.log: %D%/aut_lao_z
%D%/aut_law_char_z.log: %D%/aut_law_char_z
%D%/concat.log: %D%/concat
%D%/cross.log: %D%/cross
%D%/determinize.log: %D%/determinize
%D%/label.log: %D%/label
%D%/polynomialset.log: %D%/polynomialset
%D%/proper.log: %D%/proper
%D%/transpose.log: %D%/transpose
%D%/weight.log: %D%/weight
%D%/zip-maps.log: %D%/zip-maps

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(%C%_TESTS)'
