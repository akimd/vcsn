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

bin_PROGRAMS +=                                 \
  bin/vcsn-aut-to-exp                           \
  bin/vcsn-cat                                  \
  bin/vcsn-de-bruijn                            \
  bin/vcsn-determinize                          \
  bin/vcsn-evaluate                             \
  bin/vcsn-lift                                 \
  bin/vcsn-standard-of                          \
  bin/vcsn-transpose

bin_vcsn_aut_to_exp_LDADD  = $(all_libctx) $(libvcsn)
bin_vcsn_cat_LDADD         = $(all_libctx) $(libvcsn)
bin_vcsn_de_bruijn_LDADD   = $(all_libctx) $(libvcsn)
bin_vcsn_determinize_LDADD = $(all_libctx) $(libvcsn)
bin_vcsn_evaluate_LDADD    = $(all_libctx) $(libvcsn)
bin_vcsn_lift_LDADD        = $(all_libctx) $(libvcsn)
bin_vcsn_standard_of_LDADD = $(all_libctx) $(libvcsn)
bin_vcsn_transpose_LDADD   = $(all_libctx) $(libvcsn)

bin_vcsn_aut_to_exp_SOURCES  = bin/parse-args.cc bin/vcsn-aut-to-exp.cc
bin_vcsn_cat_SOURCES         = bin/parse-args.cc bin/vcsn-cat.cc
bin_vcsn_de_bruijn_SOURCES   = bin/parse-args.cc bin/vcsn-de-bruijn.cc
bin_vcsn_lift_SOURCES        = bin/parse-args.cc bin/vcsn-lift.cc
bin_vcsn_standard_of_SOURCES = bin/parse-args.cc bin/vcsn-standard-of.cc
bin_vcsn_transpose_SOURCES   = bin/parse-args.cc bin/vcsn-transpose.cc
