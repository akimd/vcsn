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

SOURCES_DOT_PARSE_YY =				\
  lib/vcsn/dot/stack.hh				\
  lib/vcsn/dot/position.hh			\
  lib/vcsn/dot/location.hh			\
  lib/vcsn/dot/parse.hh				\
  lib/vcsn/dot/parse.cc
BUILT_SOURCES += $(SOURCES_PARSE_DOT_EXP_YY)

# Compile the parser and save cycles.
# This code comes from "Handling Tools that Produce Many Outputs",
# from the Automake documentation.
EXTRA_DIST +=					\
  lib/vcsn/dot/README.txt			\
  lib/vcsn/dot/parse.stamp			\
  lib/vcsn/dot/parse.yy
# The dependency is on bison++.in and not bison++, since bison++ is
# regenedoted at distribution time, and voids the time stamps (which
# we don't want!).
lib/vcsn/dot/parse.stamp: lib/vcsn/dot/parse.yy $(BISONXX_IN)
	$(AM_V_GEN)mkdir -p $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(MAKE) $(BISONXX)
	$(AM_V_at)$(BISONXX) $(BISONXXFLAGS) --	\
	  $< $(srcdir)/lib/vcsn/dot/parse.cc	\
	  $(AM_BISONFLAGS) $(BISONFLAGS)
	$(AM_V_at)mv -f $@.tmp $@

## If Make does not know it will genedote in the srcdir, then when
## trying to compile from *.cc to *.lo, it will not apply VPATH
## lookup, since it expects the file to be in builddir.  So *here*,
## make srcdir explicit.
$(addprefix $(srcdir)/, $(SOURCES_DOT_PARSE_YY)): lib/vcsn/dot/parse.stamp
	@if test -f $@; then :; else		\
	  rm -f $<;				\
	  $(MAKE) $(AM_MAKEFLAGS) $<;		\
	fi
