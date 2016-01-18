# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012-2016 Vaucanson Group.
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

# Should soon be obsolete with Autoconf 2.70.
ACLOCAL_AMFLAGS = -I build-aux/m4

include build-aux/make/init.mk
include build-aux/make/buildfarm.mk

AM_CXXFLAGS += $(WARNING_CXXFLAGS) $(VISIBILITY_CXXFLAGS)
AM_CPPFLAGS += -I$(top_srcdir) -I$(top_builddir) $(BOOST_CPPFLAGS)

include bin/local.mk
include doc/local.mk
include lib/local.mk
include libexec/local.mk
include python/local.mk
include share/local.mk
include tests/local.mk
include vcsn/local.mk

EXTRA_DIST += COPYING.txt NEWS.txt TODO.txt README.txt  \
  build-aux/bin/coverage build-aux/bin/fuse-switch      \
  build-aux/bin/move-if-change

# Code coverage.
.PHONY: coverage
coverage:
	CXX='$(CXX)' $(top_srcdir)/build-aux/bin/coverage

# Documentation.
PANDOC = pandoc
README.html: README.txt
	$(PANDOC)  -s --toc --from rst --to html5 -o $@ $<


## -------- ##
## Debian.  ##
## -------- ##

EXTRA_DIST += debian/changelog debian/compat	\
  debian/control debian/rules debian/copyright

debian/changelog: configure.ac
	$(top_builddir)/config.status --file debian/changelog

# Build Debian packages.
# Run it with:  make DEBUILDFLAGS=-j8 deb -j8
deb: dist
	rm -rf vcsn-$(VERSION) vcsn-$(VERSION)$(GITPATCH)
	tar Jxvf vcsn-$(VERSION).tar.xz
	mv vcsn-$(VERSION) vcsn-$(VERSION)$(GITPATCH)
	tar Jcvf vcsn_$(VERSION)$(GITPATCH).orig.tar.xz \
	         vcsn-$(VERSION)$(GITPATCH)
	cd vcsn-$(VERSION)$(GITPATCH) && debuild $(DEBUILDFLAGS) -us -uc
