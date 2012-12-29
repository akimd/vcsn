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


## ----------- ##
## Libraries.  ##
## ----------- ##

libchar_b_lal = $(top_builddir)/lib/libchar_b_lal.la
libchar_b_law = $(top_builddir)/lib/libchar_b_law.la
libchar_br_lau = $(top_builddir)/lib/libchar_br_lau.la
libchar_br_lal = $(top_builddir)/lib/libchar_br_lal.la
libchar_br_law = $(top_builddir)/lib/libchar_br_law.la
libchar_z_lau = $(top_builddir)/lib/libchar_z_lau.la
libchar_z_lal = $(top_builddir)/lib/libchar_z_lal.la
libchar_z_law = $(top_builddir)/lib/libchar_z_law.la
libchar_zr_lal = $(top_builddir)/lib/libchar_zr_lal.la
libchar_zr_law = $(top_builddir)/lib/libchar_zr_law.la
libchar_zrr_lal = $(top_builddir)/lib/libchar_zrr_lal.la
libchar_zrr_law = $(top_builddir)/lib/libchar_zrr_law.la
libchar_zmin_lal = $(top_builddir)/lib/libchar_zmin_lal.la
libchar_zmin_law = $(top_builddir)/lib/libchar_zmin_law.la

all_libctx =                                            \
  $(libchar_b_lal) $(libchar_b_law)                     \
  $(libchar_br_lau) $(libchar_br_lal) $(libchar_br_law) \
  $(libchar_z_lau) $(libchar_z_lal) $(libchar_z_law)    \
  $(libchar_zr_lal) $(libchar_zr_law)                   \
  $(libchar_zrr_lal) $(libchar_zrr_law)                 \
  $(libchar_zmin_lal) $(libchar_zmin_law)

libvcsn = $(top_builddir)/lib/libvcsn.la

## ---------- ##
## Programs.  ##
## ---------- ##

vcsn_cat = $(top_builddir)/bin/vcsn-cat
vcsn_de_bruijn = $(top_builddir)/bin/vcsn-de-bruijn
vcsn_determinize = $(top_builddir)/bin/vcsn-determinize
vcsn_evaluate = $(top_builddir)/bin/vcsn-evaluate
vcsn_lift = $(top_builddir)/bin/vcsn-lift
vcsn_standard_of = $(top_builddir)/bin/vcsn-standard-of
vcsn_transpose = $(top_builddir)/bin/vcsn-transpose

all_vcsn =                                      \
  $(vcsn_cat)                                   \
  $(vcsn_de_bruijn)                             \
  $(vcsn_determinize)                           \
  $(vcsn_evaluate)                              \
  $(vcsn_lift)                                  \
  $(vcsn_standard_of)                           \
  $(vcsn_transpose)
