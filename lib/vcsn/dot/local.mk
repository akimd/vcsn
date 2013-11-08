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

## ------------- ##
## The Scanner.  ##
## ------------- ##

BUILT_SOURCES += %D%/scan.cc
MAINTAINERCLEANFILES += %D%/scan.cc

EXTRA_DIST += %D%/scan.ll
# The dependency is on flex++.in and not flex++, since flex++ is
# regenerated at distribution time, and voids the time stamps (which
# we don't want!).
%D%/scan.cc: %D%/scan.ll $(FLEXXX_IN)
	$(AM_V_GEN)mkdir -p $(@D)
	$(AM_V_at)rm -f $@
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) $(FLEXXX)
# Guarantees atomic generation of the output.
	$(AM_V_at)$(FLEXXX) $< $@ $(FLEXXXFLAGS)

## ------------ ##
## The parser.  ##
## ------------ ##

SOURCES_%C%_PARSE_YY =				\
  %D%/stack.hh				        \
  %D%/parse.hh				        \
  %D%/parse.cc
BUILT_SOURCES += $(SOURCES_%C%_PARSE_YY)
MAINTAINERCLEANFILES +=                                 \
  $(addprefix $(srcdir)/,$(SOURCES_%C%_PARSE_YY))       \
  $(srcdir)/%D%/parse.html                              \
  $(srcdir)/%D%/parse.output                            \
  $(srcdir)/%D%/parse.stamp                             \
  $(srcdir)/%D%/parse.xml

# Compile the parser and save cycles.
# This code comes from "Handling Tools that Produce Many Outputs",
# from the Automake documentation.
EXTRA_DIST +=                                   \
  %D%/parse.stamp                               \
  %D%/parse.yy
# The dependency is on bison++.in and not bison++, since bison++ is
# regenerated at distribution time, and voids the time stamps (which
# we don't want!).
%D%/parse.stamp: %D%/parse.yy $(BISONXX_IN)
	$(AM_V_GEN)mkdir -p $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(MAKE) $(BISONXX)
	$(AM_V_at)$(BISONXX) $(BISONXXFLAGS) --	\
	  $< $(srcdir)/%D%/parse.cc	\
	  $(AM_BISONFLAGS) $(BISONFLAGS)
	$(AM_V_at)mv -f $@.tmp $@

## If Make does not know it will generate in the srcdir, then when
## trying to compile from *.cc to *.lo, it will not apply VPATH
## lookup, since it expects the file to be in builddir.  So *here*,
## make srcdir explicit.
$(addprefix $(srcdir)/, $(SOURCES_%C%_PARSE_YY)): %D%/parse.stamp
	@if test ! -f $@; then			\
	  rm -f $<;				\
	  $(MAKE) $(AM_MAKEFLAGS) $<;		\
	fi

## ------------- ##
## The library.  ##
## ------------- ##

lib_libvcsn_la_SOURCES +=                       \
  $(SOURCES_%C%_PARSE_YY)                       \
  %D%/driver.hh  %D%/driver.cc                  \
  %D%/fwd.hh                                    \
  %D%/scan.hh %D%/scan.cc
