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

# We do not use Automake features here.
SOURCES_RAT_PARSE_YY =				\
  lib/vcsn/rat/stack.hh				\
  lib/vcsn/rat/position.hh			\
  lib/vcsn/rat/location.hh			\
  lib/vcsn/rat/parse.hh				\
  lib/vcsn/rat/parse.cc
BUILT_SOURCES += $(SOURCES_PARSE_RAT_EXP_YY)

# Compile the parser and save cycles.
# This code comes from "Handling Tools that Produce Many Outputs",
# from the Automake documentation.
EXTRA_DIST +=					\
  lib/vcsn/rat/README.txt			\
  lib/vcsn/rat/parse.stamp			\
  lib/vcsn/rat/parse.yy
# The dependency is on bison++.in and not bison++, since bison++ is
# regenerated at distribution time, and voids the time stamps (which
# we don't want!).
lib/vcsn/rat/parse.stamp: lib/vcsn/rat/parse.yy $(BISONXX_IN)
	$(AM_V_GEN)mkdir -p $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(MAKE) $(BISONXX)
	$(AM_V_at)$(BISONXX) $(BISONXXFLAGS) --	\
	  $< $(srcdir)/lib/vcsn/rat/parse.cc	\
	  $(AM_BISONFLAGS) $(BISONFLAGS)
	$(AM_V_at)mv -f $@.tmp $@

## If Make does not know it will generate in the srcdir, then when
## trying to compile from *.cc to *.lo, it will not apply VPATH
## lookup, since it expects the file to be in builddir.  So *here*,
## make srcdir explicit.
$(addprefix $(srcdir)/, $(SOURCES_RAT_PARSE_YY)): lib/vcsn/rat/parse.stamp
	@if test ! -f $@; then			\
	  rm -f $<;				\
	  $(MAKE) $(AM_MAKEFLAGS) $<;		\
	fi
