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

# Not check_PROGRAMS, see below why.
EXTRA_PROGRAMS +=                               \
  unit/aut_lau_z                                \
  unit/aut_lal_char_z                           \
  unit/aut_law_char_z                           \
  unit/concat                                   \
  unit/eps-removal                              \
  unit/polynomialset                            \
  unit/transpose

AM_LDADD = $(libvcsn)

noinst_HEADERS = unit/test.hh

unit_aut_lau_z_LDADD = $(liblau_z) $(AM_LDADD)
unit_aut_lal_char_z_LDADD = $(liblal_char_z) $(AM_LDADD)
unit_aut_law_char_z_LDADD = $(liblaw_char_z) $(AM_LDADD)

unit_concat_LDADD = $(liblaw_char_z) $(AM_LDADD)

unit_eps_removal_LDADD = $(all_libctx) $(AM_LDADD)

unit_polynomialset_LDADD = $(liblal_char_z) $(liblal_char_zmin) $(AM_LDADD)

unit_transpose_LDADD = $(liblal_char_b) $(liblal_char_z) $(AM_LDADD)

unit_TESTS =                                    \
  unit/aut_lau_z.chk                            \
  unit/aut_lal_char_z.chk                       \
  unit/aut_law_char_z.chk                       \
  unit/concat.chk                               \
  unit/eps-removal.chk                          \
  unit/polynomialset.chk                        \
  unit/transpose.chk
dist_TESTS += $(unit_TESTS)

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
unit/aut_lal_char_z.log: unit/aut_lal_char_z
unit/aut_lau_z.log: unit/aut_lau_z
unit/aut_law_char_z.log: unit/aut_law_char_z
unit/concat.log: unit/concat
unit/eps-removal.log: unit/eps-removal
unit/polynomialset.log: unit/polynomialset
unit/transpose.log: unit/transpose

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(unit_TESTS)'
