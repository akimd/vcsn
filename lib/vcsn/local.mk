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

# A Bison wrapper for C++.
BISONXX = $(top_builddir)/build-aux/bin/bison++
BISONXX_IN = $(top_srcdir)/build-aux/bin/bison++.in

BISONXXFLAGS =                                  \
  $(if $(V:0=),--verbose)
AM_BISONFLAGS =                                 \
  -Wall --report=all # -Werror

pkglib_LTLIBRARIES = lib/libvcsn.la
lib_libvcsn_la_SOURCES =                        \
  lib/vcsn/algos/accessible.cc                  \
  lib/vcsn/algos/aut-to-exp.cc                  \
  lib/vcsn/algos/complete.cc                    \
  lib/vcsn/algos/copy.cc                        \
  lib/vcsn/algos/de_bruijn.cc                   \
  lib/vcsn/algos/determinize.cc                 \
  lib/vcsn/algos/dot.cc                         \
  lib/vcsn/algos/edit-automaton.cc              \
  lib/vcsn/algos/eval.cc                        \
  lib/vcsn/algos/is_complete.cc                 \
  lib/vcsn/algos/is-deterministic.cc            \
  lib/vcsn/algos/is-eps-acyclic.cc              \
  lib/vcsn/algos/fsm.cc                         \
  lib/vcsn/algos/ladybird.cc                    \
  lib/vcsn/algos/lift.cc                        \
  lib/vcsn/algos/make-context.cc                \
  lib/vcsn/algos/print.cc                       \
  lib/vcsn/algos/product.cc                     \
  lib/vcsn/algos/read.cc                        \
  lib/vcsn/algos/registry.hh                    \
  lib/vcsn/algos/standard-of.cc                 \
  lib/vcsn/algos/transpose.cc                   \
  lib/vcsn/algos/xml.cc                         \
  lib/vcsn/ctx/ctx.cc                           \
  lib/vcsn/misc/escape.cc

# ratexpset.hxx depends on rat/driver.hh which requires
# rat/location.hh.  The dot parser, loads ratexpset.hxx, so we _must_
# compile the rat parser first.
lib/vcsn/dot/parser.stamp: lib/vcsn/rat/parser.stamp

include lib/vcsn/dot/local.mk
lib_libvcsn_la_SOURCES +=                       \
  $(SOURCES_DOT_PARSE_YY)                       \
  lib/vcsn/dot/driver.hh lib/vcsn/dot/driver.cc \
  lib/vcsn/dot/scan.ll

include lib/vcsn/rat/local.mk
lib_libvcsn_la_SOURCES +=                       \
  $(SOURCES_RAT_PARSE_YY)                       \
  lib/vcsn/rat/driver.hh lib/vcsn/rat/driver.cc \
  lib/vcsn/rat/scan.ll
