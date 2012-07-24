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

# A Bison wrapper for C++.
BISONXX = $(top_builddir)/build-aux/bin/bison++
BISONXX_IN = $(top_srcdir)/build-aux/bin/bison++.in

BISONXXFLAGS =					\
  $(if $(V:0=),--verbose)
AM_BISONFLAGS =					\
  -Wall --report=all # -Werror

pkglib_LTLIBRARIES = lib/libvcsn.la
lib_libvcsn_la_SOURCES =			\
  lib/vcsn/misc/escape.cc

include lib/vcsn/dot/local.mk
lib_libvcsn_la_SOURCES +=			\
  $(SOURCES_DOT_PARSE_YY)			\
  lib/vcsn/dot/driver.hh lib/vcsn/dot/driver.cc	\
  lib/vcsn/dot/scan.ll

include lib/vcsn/rat/local.mk
lib_libvcsn_la_SOURCES +=			\
  $(SOURCES_RAT_PARSE_YY)			\
  lib/vcsn/rat/driver.hh lib/vcsn/rat/driver.cc	\
  lib/vcsn/rat/scan.ll
