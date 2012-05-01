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

include $(top_srcdir)/build-aux/make/init.mk

AM_CPPFLAGS += -I$(top_srcdir)

vcsndir = $(includedir)/vcsn
nobase_vcsn_HEADERS =                           \
  algos/determinize.hh				\
  algos/dotty.hh                                \
  algos/product.hh                              \
  algos/standard_of.hh                          \
  alphabets/char.hh                             \
  alphabets/setalpha.hh                         \
  core/crange.hh                                \
  core/entryiter.hh                             \
  core/kind.hh                                  \
  core/mutable_automaton.hh                     \
  core/rat/abstract_kratexps.hh                 \
  core/rat/kratexps.hh                          \
  core/rat/kratexps.hxx                         \
  core/rat/node.fwd.hh                          \
  core/rat/node.hh                              \
  core/rat/node.hxx                             \
  core/rat/printer.hh                           \
  core/rat/printer.hxx                          \
  core/rat/visitor.hh                           \
  core/rat/visitor.hxx                          \
  core/transition.hh                            \
  factory/ladybird.hh                           \
  misc/cast.hh                                  \
  misc/const_traits.hh                          \
  misc/echo.hh                                  \
  misc/escape.hh                                \
  weights/b.hh                                  \
  weights/poly.hh                               \
  weights/z.hh                                  \
  weights/z_min.hh

include io/local.mk
