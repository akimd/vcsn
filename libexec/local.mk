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

dist_pkglibexec_SCRIPTS =                       \
  %D%/vcsn-compile                              \
  %D%/vcsn-gdb                                  \
  %D%/vcsn-notebook                             \
  %D%/vcsn-ps                                   \
  %D%/vcsn-score                                \
  %D%/vcsn-score-compare


# The generator.
tools_gen = build-aux/bin/tools-gen
dist_noinst_python += $(tools_gen)
# Files generated by tools_gen.
from_tools_gen =	\
	%D%/vcsn-tools-algos.cc
CLEANFILES += %D%/tools.stamp $(from_tools_gen)
%D%/tools.stamp: $(tools_gen) vcsn/dyn/algos.json
	$(AM_V_GEN)$(mkdir_p) $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(PYTHON) $(srcdir)/$(tools_gen)                     \
	  --output %D%/vcsn-tools-algos.cc.tmp                         \
		--header $(srcdir)/%D%/vcsn-tools.hh                   \
		vcsn/dyn/algos.json
	$(AM_V_at)for f in $(from_tools_gen);          \
	do                                              \
	  $(move_if_change) $$f.tmp $$f || exit 1;      \
	done
	$(AM_V_at)mv -f $@.tmp $@

$(from_tools_gen): %D%/tools.stamp
	@if test ! -f $@; then                  \
	  rm -f $<;                             \
	  $(MAKE) $(AM_MAKEFLAGS) $<;           \
	fi

pkglibexec_PROGRAMS = %D%/vcsn-tools
%C%_vcsn_tools_SOURCES = $(from_tools_gen) %D%/vcsn-tools.cc %D%/vcsn-tools.hh
%C%_vcsn_tools_LDADD = $(all_libctx) $(libvcsn)
