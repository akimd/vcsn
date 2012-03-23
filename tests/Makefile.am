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
check_PROGRAMS =

AM_CXXFLAGS += $(WARNING_CXXFLAGS)
AM_CPPFLAGS += -I$(top_srcdir)

dist_noinst_SCRIPTS += checker
TEST_EXTENSIONS += .chk
CHK_LOG_COMPILER = $(srcdir)/checker

TESTS = $(dist_TESTS)

include rat/local.mk
include unit/local.mk
