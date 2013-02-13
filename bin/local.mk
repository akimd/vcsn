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

# Keep common.mk in sync.
bin_PROGRAMS +=                                 \
  bin/vcsn-aut-to-exp                           \
  bin/vcsn-cat                                  \
  bin/vcsn-de-bruijn                            \
  bin/vcsn-determinize                          \
  bin/vcsn-evaluate                             \
  bin/vcsn-ladybird                             \
  bin/vcsn-lift                                 \
  bin/vcsn-product                              \
  bin/vcsn-standard-of                          \
  bin/vcsn-transpose                            \
  bin/vcsn-accessible

bin_vcsn_aut_to_exp_LDADD  = $(all_libctx) $(libvcsn)
bin_vcsn_cat_LDADD         = $(all_libctx) $(libvcsn)
bin_vcsn_de_bruijn_LDADD   = $(all_libctx) $(libvcsn)
bin_vcsn_determinize_LDADD = $(all_libctx) $(libvcsn)
bin_vcsn_evaluate_LDADD    = $(all_libctx) $(libvcsn)
bin_vcsn_ladybird_LDADD    = $(all_libctx) $(libvcsn)
bin_vcsn_lift_LDADD        = $(all_libctx) $(libvcsn)
bin_vcsn_product_LDADD     = $(all_libctx) $(libvcsn)
bin_vcsn_standard_of_LDADD = $(all_libctx) $(libvcsn)
bin_vcsn_transpose_LDADD   = $(all_libctx) $(libvcsn)
bin_vcsn_accessible_LDADD  = $(all_libctx) $(libvcsn)

parse_args = bin/parse-args.hh bin/parse-args.cc
bin_vcsn_aut_to_exp_SOURCES  = $(parse_args) bin/vcsn-aut-to-exp.cc
bin_vcsn_cat_SOURCES         = $(parse_args) bin/vcsn-cat.cc
bin_vcsn_de_bruijn_SOURCES   = $(parse_args) bin/vcsn-de-bruijn.cc
bin_vcsn_determinize_SOURCES = $(parse_args) bin/vcsn-determinize.cc
bin_vcsn_evaluate_SOURCES    = $(parse_args) bin/vcsn-evaluate.cc
bin_vcsn_ladybird_SOURCES    = $(parse_args) bin/vcsn-ladybird.cc
bin_vcsn_lift_SOURCES        = $(parse_args) bin/vcsn-lift.cc
bin_vcsn_product_SOURCES     = $(parse_args) bin/vcsn-product.cc
bin_vcsn_standard_of_SOURCES = $(parse_args) bin/vcsn-standard-of.cc
bin_vcsn_transpose_SOURCES   = $(parse_args) bin/vcsn-transpose.cc
bin_vcsn_accessible_SOURCES  = $(parse_args) bin/vcsn-accessible.cc

