# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012-2014 Vaucanson Group.
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

dist_bin_SCRIPTS +=                             \
  %D%/efstcompile                               \
  %D%/efstdecompile                             \
  %D%/vcsn-notebook                             \
  %D%/vcsn-score

nodist_bin_SCRIPTS +=                           \
  %D%/vcsn

## ---------- ##
## Programs.  ##
## ---------- ##

all_vcsn =                                      \
  %D%/vcsn-de-bruijn                            \
  %D%/vcsn-tafkit                               \
  %D%/vcsn-double-ring                          \
  %D%/vcsn-ladybird                             \
  %D%/vcsn-random                               \
  %D%/vcsn-u

bin_PROGRAMS += $(all_vcsn)
noinst_LTLIBRARIES += %D%/libtafkit.la
%C%_libtafkit_la_SOURCES = %D%/parse-args.hh %D%/parse-args.cc
%C%_libtafkit_la_LIBADD = $(all_libctx) $(libvcsn)

%C%_vcsn_de_bruijn_LDADD        = %D%/libtafkit.la
%C%_vcsn_double_ring_LDADD      = %D%/libtafkit.la
%C%_vcsn_tafkit_LDADD           = %D%/libtafkit.la
%C%_vcsn_ladybird_LDADD         = %D%/libtafkit.la
%C%_vcsn_random_LDADD           = %D%/libtafkit.la
%C%_vcsn_u_LDADD                = %D%/libtafkit.la
