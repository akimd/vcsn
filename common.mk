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


## ----------- ##
## Libraries.  ##
## ----------- ##

liblal_char_b    = $(top_builddir)/lib/liblal_char_b.la
liblan_char_b    = $(top_builddir)/lib/liblan_char_b.la
liblaw_char_b    = $(top_builddir)/lib/liblaw_char_b.la
liblau_br        = $(top_builddir)/lib/liblau_br.la
liblal_char_br   = $(top_builddir)/lib/liblal_char_br.la
liblaw_char_br   = $(top_builddir)/lib/liblaw_char_br.la
liblau_z         = $(top_builddir)/lib/liblau_z.la
liblal_char_z    = $(top_builddir)/lib/liblal_char_z.la
liblan_char_z    = $(top_builddir)/lib/liblan_char_z.la
liblaw_char_z    = $(top_builddir)/lib/liblaw_char_z.la
liblal_char_zr   = $(top_builddir)/lib/liblal_char_zr.la
liblaw_char_zr   = $(top_builddir)/lib/liblaw_char_zr.la
liblal_char_zrr  = $(top_builddir)/lib/liblal_char_zrr.la
liblaw_char_zrr  = $(top_builddir)/lib/liblaw_char_zrr.la
liblal_char_zmin = $(top_builddir)/lib/liblal_char_zmin.la
liblaw_char_zmin = $(top_builddir)/lib/liblaw_char_zmin.la
liblal_char_r    = $(top_builddir)/lib/liblal_char_r.la
liblaw_char_r    = $(top_builddir)/lib/liblaw_char_r.la

all_libctx =                                                            \
  $(liblal_char_b) $(liblan_char_b) $(liblaw_char_b)                    \
  $(liblau_br) $(liblal_char_br) $(liblaw_char_br)                      \
  $(liblal_char_r) $(liblaw_char_r)                                     \
  $(liblau_z) $(liblal_char_z) $(liblan_char_z) $(liblaw_char_z)        \
  $(liblal_char_zr) $(liblaw_char_zr)                                   \
  $(liblal_char_zrr) $(liblaw_char_zrr)                                 \
  $(liblal_char_zmin) $(liblaw_char_zmin)

libvcsn = $(top_builddir)/lib/libvcsn.la

## ---------- ##
## Programs.  ##
## ---------- ##

vcsn_accessible       = $(top_builddir)/bin/vcsn-accessible
vcsn_aut_to_exp       = $(top_builddir)/bin/vcsn-aut-to-exp
vcsn_cat              = $(top_builddir)/bin/vcsn-cat
vcsn_complete         = $(top_builddir)/bin/vcsn-complete
vcsn_de_bruijn        = $(top_builddir)/bin/vcsn-de-bruijn
vcsn_determinize      = $(top_builddir)/bin/vcsn-determinize
vcsn_evaluate         = $(top_builddir)/bin/vcsn-evaluate
vcsn_is_complete      = $(top_builddir)/bin/vcsn-is-complete
vcsn_is_deterministic = $(top_builddir)/bin/vcsn-is-deterministic
vcsn_is_proper        = $(top_builddir)/bin/vcsn-is-proper
vcsn_ladybird         = $(top_builddir)/bin/vcsn-ladybird
vcsn_lift             = $(top_builddir)/bin/vcsn-lift
vcsn_product          = $(top_builddir)/bin/vcsn-product
vcsn_proper           = $(top_builddir)/bin/vcsn-proper
vcsn_standard         = $(top_builddir)/bin/vcsn-standard
vcsn_thompson         = $(top_builddir)/bin/vcsn-thompson
vcsn_transpose        = $(top_builddir)/bin/vcsn-transpose

all_vcsn =                                      \
  $(vcsn_accessible)                            \
  $(vcsn_aut_to_exp)                            \
  $(vcsn_cat)                                   \
  $(vcsn_complete)                              \
  $(vcsn_de_bruijn)                             \
  $(vcsn_determinize)                           \
  $(vcsn_proper)                                \
  $(vcsn_evaluate)                              \
  $(vcsn_is_complete)                           \
  $(vcsn_is_deterministic)                      \
  $(vcsn_is_proper)                             \
  $(vcsn_ladybird)                              \
  $(vcsn_lift)                                  \
  $(vcsn_product)                               \
  $(vcsn_standard)                              \
  $(vcsn_thompson)                              \
  $(vcsn_transpose)
