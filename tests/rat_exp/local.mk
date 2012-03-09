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

dist_noinst_SCRIPTS += rat_exp/rat_exp.pl
TEST_EXTENSIONS = .rat_exp
RAT_EXP_LOG_COMPILER = $(srcdir)/rat_exp/rat_exp.pl
AM_RAT_EXP_LOG_FLAGS = $(top_builddir)/vcsn/io/rat-exp

dist_TESTS += rat_exp/test.rat_exp
