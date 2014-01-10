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

include build-aux/make/init.mk

# Unfortunately Automake 1.14 does not generate this for us.
vcsn/config.hh: $(top_builddir)/lib/config.h
	cd $(top_builddir) && $(SHELL) ./config.status $@
BUILT_SOURCES += lib/config.h vcsn/config.hh
DISTCLEANFILES += vcsn/config.hh python/vcsn.pyc

AM_CXXFLAGS += $(WARNING_CXXFLAGS) $(VISIBILITY_CXXFLAGS)
AM_CPPFLAGS += -I$(top_srcdir) -I$(top_builddir)

include bin/local.mk
include lib/local.mk
include python/local.mk
include share/local.mk
include tests/local.mk
include vcsn/local.mk

EXTRA_DIST += COPYING.txt NEWS.txt TODO.txt README.txt
