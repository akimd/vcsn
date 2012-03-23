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

dist_noinst_SCRIPTS += rat/rat
TEST_EXTENSIONS += .rat
RAT_LOG_COMPILER = $(srcdir)/rat/rat
AM_RAT_LOG_FLAGS = rat/pprat

dist_TESTS += rat/test.rat rat/z.rat

check_PROGRAMS += rat/pprat
AM_CPPFLAGS += $(top_srcdir)/vcsn

rat_pprat_LDADD = $(top_builddir)/vcsn/librat.la
