## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2012-2013 Vaucanson Group.
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
lib_libvcsn_la_SOURCES =                        \
  %D%/algos/accessible.cc                       \
  %D%/algos/are-equivalent.cc                   \
  %D%/algos/are-isomorphic.cc                   \
  %D%/algos/aut-to-exp.cc                       \
  %D%/algos/complement.cc                       \
  %D%/algos/complete.cc                         \
  %D%/algos/compose.cc                          \
  %D%/algos/concatenate.cc                      \
  %D%/algos/constant-term.cc                    \
  %D%/algos/copy.cc                             \
  %D%/algos/de-bruijn.cc                        \
  %D%/algos/derivation.cc                       \
  %D%/algos/determinize.cc                      \
  %D%/algos/divkbaseb.cc                        \
  %D%/algos/dot.cc                              \
  %D%/algos/double-ring.cc                      \
  %D%/algos/edit-automaton.cc                   \
  %D%/algos/efsm.cc                             \
  %D%/algos/enumerate.cc                        \
  %D%/algos/eval.cc                             \
  %D%/algos/expand.cc                           \
  %D%/algos/fado.cc                             \
  %D%/algos/fwd.hh                              \
  %D%/algos/grail.cc                            \
  %D%/algos/info.cc                             \
  %D%/algos/insplit.cc                          \
  %D%/algos/is-ambiguous.cc                     \
  %D%/algos/is-complete.cc                      \
  %D%/algos/is-deterministic.cc                 \
  %D%/algos/is-eps-acyclic.cc                   \
  %D%/algos/is-normalized.cc                    \
  %D%/algos/is-proper.cc                        \
  %D%/algos/is-valid.cc                         \
  %D%/algos/ladybird.cc                         \
  %D%/algos/left-mult.cc                        \
  %D%/algos/lift.cc                             \
  %D%/algos/make-context.cc                     \
  %D%/algos/minimize.cc                         \
  %D%/algos/print.cc                            \
  %D%/algos/product.cc                          \
  %D%/algos/proper.cc                           \
  %D%/algos/random.cc                           \
  %D%/algos/read.cc                             \
  %D%/algos/registry.hh                         \
  %D%/algos/sort.cc                             \
  %D%/algos/standard.cc                         \
  %D%/algos/star.cc                             \
  %D%/algos/star-height.cc                      \
  %D%/algos/star-normal-form.cc                 \
  %D%/algos/strip.cc                            \
  %D%/algos/sum.cc                              \
  %D%/algos/synchronizing-word.cc               \
  %D%/algos/thompson.cc                         \
  %D%/algos/tikz.cc                             \
  %D%/algos/transpose.cc                        \
  %D%/algos/u.cc                                \
  %D%/algos/union.cc                            \
  %D%/algos/universal.cc                        \
  %D%/ctx/ctx.cc                                \
  %D%/dyn/context-parser.cc                     \
  %D%/dyn/context-printer.cc                    \
  %D%/dyn/signature-printer.cc                  \
  %D%/dyn/translate.cc                          \
  %D%/misc/escape.cc                            \
  %D%/misc/file-library.cc                      \
  %D%/misc/indent.cc                            \
  %D%/misc/path.cc                              \
  %D%/misc/signature.cc                         \
  %D%/misc/stream.cc

lib_libvcsn_la_LDFLAGS =					\
  $(BOOST_FLYWEIGHT_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) 	\
  $(BOOST_SYSTEM_LDFLAGS) $(BOOST_REGEX_LDFLAGS)
lib_libvcsn_la_LIBADD =					\
  $(BOOST_FLYWEIGHT_LIBS) $(BOOST_FILESYSTEM_LIBS)	\
  $(BOOST_SYSTEM_LIBS) $(BOOST_REGEX_LIBS)		\
  -lltdl

# ratexpset.hxx depends on rat/driver.hh which requires
# rat/location.hh.  The dot parser, loads ratexpset.hxx, so we _must_
# compile the rat parser first.
%D%/dot/parse.stamp: %D%/rat/parse.stamp

include %D%/dot/local.mk
include %D%/rat/local.mk
