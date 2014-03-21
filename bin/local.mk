## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2012-2014 Vaucanson Group.
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

dist_bin_SCRIPTS +=                             \
  %D%/efstcompile                               \
  %D%/efstdecompile                             \
  %D%/vcsn-notebook                             \
  %D%/vcsn-score

nodist_bin_SCRIPTS +=                           \
  %D%/vcsn

bin_PROGRAMS += %D%/vcsn-tafkit
%C%_vcsn_tafkit_SOURCES = %D%/vcsn-tafkit.cc %D%/parse-args.hh %D%/parse-args.cc
%C%_vcsn_tafkit_LDADD = $(all_libctx) $(libvcsn)
