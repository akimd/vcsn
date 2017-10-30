## Vcsn, a generic library for finite state machines.
## Copyright (C) 2013-2017 Vcsn Group.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## The complete GNU General Public Licence Notice can be found as the
## `COPYING' file in the root directory.
##
## The Vcsn Group consists of people listed in the `AUTHORS' file.

EXTRA_DIST += %D%/pylintrc

vcsn_pythondir = $(pythondir)/vcsn
vcsn_python_PYTHON =                            \
  %D%/vcsn/__init__.py                          \
  %D%/vcsn/automaton.py                         \
  %D%/vcsn/context.py                           \
  %D%/vcsn/d3Widget.py                          \
  %D%/vcsn/dot.py                               \
  %D%/vcsn/expansion.py                         \
  %D%/vcsn/expression.py                        \
  %D%/vcsn/ipython.py                           \
  %D%/vcsn/label.py                             \
  %D%/vcsn/polynomial.py                        \
  %D%/vcsn/proxy.py                             \
  %D%/vcsn/python3.py                           \
  %D%/vcsn/score.py                             \
  %D%/vcsn/tools.py                             \
  %D%/vcsn/weight.py

vcsn_python_demodir = $(vcsn_pythondir)/demo
vcsn_python_demo_PYTHON =                       \
  %D%/vcsn/demo/__init__.py                     \
  %D%/vcsn/demo/automaton.py                    \
  %D%/vcsn/demo/eliminate_state.py

vcsn_tools_pythondir = $(pythondir)/vcsn_tools
vcsn_tools_python_PYTHON =                      \
  %D%/vcsn_tools/__init__.py                    \
  %D%/vcsn_tools/demangle.py                    \
  %D%/vcsn_tools/gdb-demangle.py

# All our Python sources, installed or not.
vcsn_python +=                                   \
  $(python_scripts)                             \
  $(vcsn_python_PYTHON)                         \
  $(vcsn_python_demo_PYTHON)                    \
  $(vcsn_tools_python_PYTHON)

vcsn_python_pylint = %D%/pylintrc

pyexec_LTLIBRARIES = %D%/vcsn_cxx.la
%C%_vcsn_cxx_la_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_PYTHON_CPPFLAGS)
%C%_vcsn_cxx_la_LDFLAGS = -avoid-version -module $(BOOST_PYTHON_LDFLAGS)
%C%_vcsn_cxx_la_LIBADD = $(BOOST_PYTHON_LIBS) $(all_libctx) lib/libvcsn.la
%C%_vcsn_cxx_la_SOURCES = %D%/vcsn_cxx.cc

# Arguably should be taken care of by Automake.  But at least 1.15
# fails to clean the latter properly.
CLEANDIRS += %D%/__pycache__ %D%/vcsn_tools/__pycache__

# Before installing this library, wait for the libraries against which
# it is linked to be installed.  Otherwise libtool will complain that
# this Python library has dependencies that are not installed.
#
# However do not write "install-pyexecLTLIBRARIES: ...", because then Automake
# does not generate the rule!  We must obfuscate.
DASH = -
install$(DASH)pyexecLTLIBRARIES: install-pkglibLTLIBRARIES install-libLTLIBRARIES

# A simple means to express dependencies for Python tests.
VCSN_PYTHON_DEPS =                              \
  $(HEADERS)                                    \
  $(pyexec_LTLIBRARIES)                         \
  $(vcsn_python)

# A short-hand to update all the python dependencies.
.PHONY: python
python: $(VCSN_PYTHON_DEPS)
