# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2013 Vaucanson Group.
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

TEST_EXTENSIONS += .py
PY_LOG_DRIVER = $(TAP_DRIVER)

%C%_TESTS =                                     \
  %D%/derivation.py
$(%C%_TESTS:.py=.log): $(VCSN_PYTHON_DEPS)

dist_TESTS += $(%C%_TESTS)
