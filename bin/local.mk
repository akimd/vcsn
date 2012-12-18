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

bin_PROGRAMS +=					\
  bin/vcsn-cat					\
  bin/vcsn-de-bruijn				\
  bin/vcsn-determinize 				\
  bin/vcsn-evaluate				\
  bin/vcsn-standard-of				\
  bin/vcsn-transpose

bin_vcsn_cat_LDADD         = $(all_libctx) $(libvcsn)
bin_vcsn_de_bruijn_LDADD   = $(all_libctx) $(libvcsn)
bin_vcsn_determinize_LDADD = $(all_libctx) $(libvcsn)
bin_vcsn_evaluate_LDADD    = $(all_libctx) $(libvcsn)
bin_vcsn_standard_of_LDADD = $(all_libctx) $(libvcsn)
bin_vcsn_transpose_LDADD   = $(all_libctx) $(libvcsn)
