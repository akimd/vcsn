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
  bin/vcsn-accessible                           \
  bin/vcsn-aut-to-exp                           \
  bin/vcsn-cat                                  \
  bin/vcsn-complete                             \
  bin/vcsn-de-bruijn                            \
  bin/vcsn-determinize                          \
  bin/vcsn-eps-removal                          \
  bin/vcsn-evaluate                             \
  bin/vcsn-is-complete                          \
  bin/vcsn-is-deterministic                     \
  bin/vcsn-is-eps-acyclic                       \
  bin/vcsn-is-proper                            \
  bin/vcsn-ladybird                             \
  bin/vcsn-lift                                 \
  bin/vcsn-product                              \
  bin/vcsn-standard                             \
  bin/vcsn-transpose

noinst_LTLIBRARIES += bin/libtafkit.la
bin_libtafkit_la_SOURCES = bin/parse-args.hh bin/parse-args.cc
bin_libtafkit_la_LIBADD = $(all_libctx) $(libvcsn)

bin_vcsn_accessible_LDADD       = bin/libtafkit.la
bin_vcsn_aut_to_exp_LDADD       = bin/libtafkit.la
bin_vcsn_cat_LDADD              = bin/libtafkit.la
bin_vcsn_complete_LDADD         = bin/libtafkit.la
bin_vcsn_de_bruijn_LDADD        = bin/libtafkit.la
bin_vcsn_determinize_LDADD      = bin/libtafkit.la
bin_vcsn_eps_removal_LDADD      = bin/libtafkit.la
bin_vcsn_evaluate_LDADD         = bin/libtafkit.la
bin_vcsn_is_complete_LDADD      = bin/libtafkit.la
bin_vcsn_is_deterministic_LDADD = bin/libtafkit.la
bin_vcsn_is_eps_acyclic_LDADD   = bin/libtafkit.la
bin_vcsn_is_proper_LDADD        = bin/libtafkit.la
bin_vcsn_ladybird_LDADD         = bin/libtafkit.la
bin_vcsn_lift_LDADD             = bin/libtafkit.la
bin_vcsn_product_LDADD          = bin/libtafkit.la
bin_vcsn_standard_LDADD         = bin/libtafkit.la
bin_vcsn_transpose_LDADD        = bin/libtafkit.la
