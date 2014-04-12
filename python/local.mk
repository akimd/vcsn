nodist_python_PYTHON = %D%/vcsn_version.py

vcsn_pythondir = $(pythondir)/vcsn
vcsn_python_PYTHON =                            \
  %D%/vcsn/__init__.py                          \
  %D%/vcsn/automaton.py                         \
  %D%/vcsn/context.py                           \
  %D%/vcsn/label.py                             \
  %D%/vcsn/polynomial.py                        \
  %D%/vcsn/ratexp.py                            \
  %D%/vcsn/weight.py

pyexec_LTLIBRARIES = %D%/vcsn_cxx.la
%C%_vcsn_cxx_la_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_PYTHON_CPPFLAGS)
%C%_vcsn_cxx_la_LDFLAGS = -avoid-version -module $(BOOST_PYTHON_LDFLAGS)
%C%_vcsn_cxx_la_LIBADD = $(BOOST_PYTHON_LIBS) $(all_libctx) lib/libvcsn.la

CLEANDIRS += %D%/__pycache__

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
  $(nodist_python_PYTHON)                       \
  $(pyexec_LTLIBRARIES)                         \
  $(vcsn_python_PYTHON)

# A short-hand to update all the python dependencies.
.PHONY: python
python: $(VCSN_PYTHON_DEPS)
