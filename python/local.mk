python_PYTHON = %D%/vcsn.py

pyexec_LTLIBRARIES = %D%/vcsn_python.la
%C%_vcsn_python_la_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_PYTHON_CPPFLAGS)
%C%_vcsn_python_la_LDFLAGS = -avoid-version -module $(BOOST_PYTHON_LDFLAGS)
%C%_vcsn_python_la_LIBADD = $(BOOST_PYTHON_LIBS) $(all_libctx) lib/libvcsn.la

# We must wait for the libraries againt which this one is linked, to
# be installed.  Then we can install this one.  Otherwise libtool will
# complain that this Python library has dependencies that are not
# installed.
install-pyexecLTLIBRARIES: install-pkglibLTLIBRARIES install-libLTLIBRARIES

# Provide a simple means to express dependencies for Python tests.
VCSN_PYTHON_DEPS = $(python_PYTHON) $(pyexec_LTLIBRARIES)
