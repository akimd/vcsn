# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012 Vaucanson Group.
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
  unit/aut_char_z_lau                           \
  unit/aut_char_z_lal                           \
  unit/aut_char_z_law                           \
  unit/concat					\
  unit/determinize                              \
  unit/ladybird-b                               \
  unit/ladybird-z                               \
  unit/ladybird-zmin                            \
  unit/poly                                     \
  unit/product                                  \
  unit/transpose

AM_LDADD = $(libvcsn)

noinst_HEADERS = unit/test.hh

unit_aut_char_z_lau_LDADD = $(libchar_z_lau) $(AM_LDADD)
unit_aut_char_z_lal_LDADD = $(libchar_z_lal) $(AM_LDADD)
unit_aut_char_z_law_LDADD = $(libchar_z_law) $(AM_LDADD)

unit_determinize_LDADD = $(libchar_b_lal) $(AM_LDADD)

unit_ladybird_b_SOURCES = unit/ladybird.cc
unit_ladybird_b_CPPFLAGS = $(AM_CPPFLAGS) -DW=b
unit_ladybird_b_LDADD = $(libchar_b_lal) $(AM_LDADD)

unit_ladybird_z_SOURCES = unit/ladybird.cc
unit_ladybird_z_CPPFLAGS = $(AM_CPPFLAGS) -DW=z
unit_ladybird_z_LDADD = $(libchar_z_lal) $(AM_LDADD)

unit_ladybird_zmin_SOURCES = unit/ladybird.cc
unit_ladybird_zmin_CPPFLAGS = $(AM_CPPFLAGS) -DW=zmin
unit_ladybird_zmin_LDADD = $(libchar_zmin_lal) $(AM_LDADD)

unit_poly_LDADD = $(libchar_z_lal) $(libchar_zmin_lal) $(AM_LDADD)

unit_product_LDADD = $(libchar_z_lal) $(AM_LDADD)

unit_transpose_LDADD = $(libchar_b_lal) $(libchar_z_lal) $(AM_LDADD)

unit_TESTS =                                    \
  unit/aut_char_z_lau.chk                       \
  unit/aut_char_z_lal.chk                       \
  unit/aut_char_z_law.chk                       \
  unit/concat.chk				\
  unit/determinize.chk                          \
  unit/eval.chk                                 \
  unit/ladybird.chk                             \
  unit/lift.chk                                 \
  unit/poly.chk                                 \
  unit/product.chk                              \
  unit/standard_of.chk                          \
  unit/transpose.chk				\
  unit/vcsn-cat.chk
dist_TESTS += $(unit_TESTS)

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
unit/aut_char_z_lau.log: unit/aut_char_z_lau
unit/aut_char_z_lal.log: unit/aut_char_z_lal
unit/aut_char_z_law.log: unit/aut_char_z_law
unit/aut_to_exp.log: rat/pprat
unit/determinize.log: unit/determinize
unit/concat.log: unit/concat
unit/ladybird.log: unit/ladybird-b unit/ladybird-z unit/ladybird-zmin
unit/poly.log: unit/poly
unit/product.log: unit/product
unit/standard_of.log: rat/pprat
unit/transpose.log: rat/pprat unit/transpose

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(unit_TESTS)'

