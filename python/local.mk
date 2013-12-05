python_PYTHON = %D%/vcsn.py

pyexec_LTLIBRARIES = %D%/vcsn_python.la
%C%_vcsn_python_la_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_PYTHON_CPPFLAGS)
%C%_vcsn_python_la_LDFLAGS = -avoid-version -module $(BOOST_PYTHON_LDFLAGS)
%C%_vcsn_python_la_LIBADD = $(BOOST_PYTHON_LIBS) $(all_libctx) lib/libvcsn.la
