## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2012-2016 Vaucanson Group.
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

# A Bison wrapper for C++.
BISONXX = $(top_builddir)/build-aux/bin/bison++
BISONXX_IN = $(top_srcdir)/build-aux/bin/bison++.in

BISONXXFLAGS =                                  \
  $(if $(V:0=),--verbose)
AM_BISONFLAGS =                                 \
  -Wall --report=all # -Werror

# A Flex wrapper for C++.
FLEXXX = $(top_builddir)/build-aux/bin/flex++
FLEXXX_IN = $(top_srcdir)/build-aux/bin/flex++.in

libvcsn = lib/libvcsn.la
pkglib_LTLIBRARIES = $(libvcsn)
# Boost.FileSystem depends on Boost.System, but Boost.m4 does not
# simplify this for us.
lib_libvcsn_la_CPPFLAGS = $(AM_CPPFLAGS) -DBUILD_LIBVCSN	\
  $(BOOST_FLYWEIGHT_CPPFLAGS) $(BOOST_FILESYSTEM_CPPFLAGS)	\
  $(BOOST_SYSTEM_CPPFLAGS) $(BOOST_REGEX_CPPFLAGS)
nodist_lib_libvcsn_la_SOURCES =                 \
  %D%/algos/algos.cc                            \
  %D%/dyn/context-printer-header-algo.cc

# Used in vcsn/local.mk to generate the registries.
algo_implems =                                  \
  %D%/algos/daut.cc                             \
  %D%/algos/edit-automaton.cc                   \
  %D%/algos/efsm.cc                             \
  %D%/algos/fado.cc                             \
  %D%/algos/fwd.hh                              \
  %D%/algos/others.cc                           \
  %D%/algos/print.cc                            \
  %D%/algos/read.cc                             \
  %D%/algos/registry.hh

lib_libvcsn_la_SOURCES =                        \
  $(algo_implems)                               \
  %D%/dyn/context-parser.cc                     \
  %D%/dyn/context-parser.hh                     \
  %D%/dyn/context-printer.cc                    \
  %D%/dyn/context-printer.hh                    \
  %D%/dyn/context-visitor.hh                    \
  %D%/dyn/fwd.hh                                \
  %D%/dyn/signature-printer.cc                  \
  %D%/dyn/signature-printer.hh                  \
  %D%/dyn/translate.cc                          \
  %D%/dyn/translate.hh                          \
  %D%/dyn/type-ast.hh                           \
  %D%/misc/direction.cc                         \
  %D%/misc/escape.cc                            \
  %D%/misc/file-library.cc                      \
  %D%/misc/format.cc                            \
  %D%/misc/indent.cc                            \
  %D%/misc/random.cc                            \
  %D%/misc/signature.cc                         \
  %D%/misc/stream.cc                            \
  %D%/misc/xltdl.cc                             \
  %D%/misc/xltdl.hh

lib_libvcsn_la_LDFLAGS =					\
  $(BOOST_FLYWEIGHT_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) 	\
  $(BOOST_SYSTEM_LDFLAGS) $(BOOST_REGEX_LDFLAGS)
lib_libvcsn_la_LIBADD =					\
  $(BOOST_FLYWEIGHT_LIBS) $(BOOST_FILESYSTEM_LIBS)	\
  $(BOOST_SYSTEM_LIBS) $(BOOST_REGEX_LIBS)		\
  -lltdl $(RT_LIBS)

# expressionset.hxx depends on rat/driver.hh which requires
# rat/location.hh.  The dot parser, loads expressionset.hxx, so we _must_
# compile the rat parser first.
%D%/dot/parse.stamp: %D%/rat/parse.stamp

include %D%/dot/local.mk
include %D%/rat/local.mk
