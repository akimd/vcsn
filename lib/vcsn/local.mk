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
  -Wall -Werror --report=all

# We do not use Automake features here.
SOURCES_PARSE_RAT_EXP_YY =			\
  lib/vcsn/stack.hh				\
  lib/vcsn/position.hh				\
  lib/vcsn/location.hh				\
  lib/vcsn/parse-rat-exp.hh			\
  lib/vcsn/parse-rat-exp.cc
BUILT_SOURCES += $(SOURCES_PARSE_RAT_EXP_YY)

# Compile the parser and save cycles.
# This code comes from "Handling Tools that Produce Many Outputs",
# from the Automake documentation.
EXTRA_DIST +=					\
  lib/vcsn/parse-rat-exp.txt			\
  lib/vcsn/parse-rat-exp.stamp			\
  lib/vcsn/parse-rat-exp.yy
# The dependency is on bison++.in and not bison++, since bison++ is
# regenerated at distribution time, and voids the time stamps (which
# we don't want!).
lib/vcsn/parse-rat-exp.stamp: lib/vcsn/parse-rat-exp.yy $(BISONXX_IN)
	$(AM_V_GEN)mkdir -p $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(MAKE) $(BISONXX)
	$(AM_V_at)$(BISONXX) $(BISONXXFLAGS) --	\
	  $< $(srcdir)/lib/vcsn/parse-rat-exp.cc	\
	  $(AM_BISONFLAGS) $(BISONFLAGS)
	$(AM_V_at)mv -f $@.tmp $@

## If Make does not know it will generate in the srcdir, then when
## trying to compile from *.cc to *.lo, it will not apply VPATH
## lookup, since it expects the file to be in builddir.  So *here*,
## make srcdir explicit.
$(addprefix $(srcdir)/, $(SOURCES_PARSE_RAT_EXP_YY)): lib/vcsn/parse-rat-exp.stamp
	@if test -f $@; then :; else		\
	  rm -f $<;				\
	  $(MAKE) $(AM_MAKEFLAGS) $<;		\
	fi

pkglib_LTLIBRARIES = lib/libvcsn.la
lib_libvcsn_la_SOURCES =			\
  $(SOURCES_PARSE_RAT_EXP_YY)			\
  lib/vcsn/driver.hh lib/vcsn/driver.cc		\
  lib/vcsn/lexer.ll
