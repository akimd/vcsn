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
include $(top_srcdir)/common.mk

## ------- ##
## Tests.  ##
## ------- ##

check_PROGRAMS =

AM_CXXFLAGS += $(WARNING_CXXFLAGS)
AM_CPPFLAGS += -I$(top_srcdir)

dist_noinst_SCRIPTS += checker
TEST_EXTENSIONS += .chk
CHK_LOG_COMPILER = $(srcdir)/checker
$(unit_TESTS:.chk=.log): $(CHK_LOG_COMPILER) $(all_vcsn)

TESTS = $(dist_TESTS)
# Lazy test suite.
RECHECK_LOGS =

include rat/local.mk
include unit/local.mk

AM_RST2HTMLFLAGS = -d -t
.log.html:
	@list='$(RST2HTML) rst2html rst2html.py';			\
	for r2h in $$list; do						\
	  if ($$r2h --version) >/dev/null 2>&1; then			\
	    R2H=$$r2h;							\
	  fi;								\
	done;								\
	if test -z "$$R2H"; then					\
	  echo >&2 "cannot find rst2html, cannot create $@";		\
	  exit 2;							\
	fi;								\
	$$R2H $(AM_RST2HTMLFLAGS) $(RST2HTMLFLAGS) $< >$@.tmp
	@mv $@.tmp $@

## Taken from Automake's contrib/check-html.am
TEST_SUITE_HTML = $(TEST_SUITE_LOG:.log=.html)

mostlyclean-local: mostlyclean-check-html
.PHONY: mostlyclean-check-html
mostlyclean-check-html:
## Expand $(TEST_LOGS) only once, to avoid exceeding line length limits.
	list='$(TEST_LOGS:.log=.html)'; test -z "$$list" || rm -f $$list
	rm -f $(TEST_SUITE_HTML)

# Be sure to run check first, and then to convert the result.
# Beware of concurrent executions.  Run "check" not "check-TESTS", as
# check-SCRIPTS and other dependencies are rebuilt by the former only.
# And expect check to fail.
check-html recheck-html:
	@target=`echo $@ | sed 's/-html$$//'`; \
	rv=0; $(MAKE) $(AM_MAKEFLAGS) $$target || rv=$$?; \
## The nullification of $(TEST_LOGS) is required to ensure that
## "make recheck-html" do not try to uselessly re-run tests.
	$(MAKE) $(AM_MAKEFLAGS) $(TEST_SUITE_HTML) TEST_LOGS= || exit 4; \
	exit $$rv

.PHONY: check-html recheck-html
.MAKE: check-html recheck-html

## ------------------- ##
## Tests environment.  ##
## ------------------- ##

# By default, tests are buildcheck.
AM_TESTS_ENVIRONMENT = $(BUILDCHECK_ENVIRONMENT)

# Use the wrappers to run the non-installed executables.
BUILDCHECK_ENVIRONMENT +=                                       \
  PATH=$(abs_top_builddir)/bin:$$PATH; export PATH;

INSTALLCHECK_ENVIRONMENT +=                                     \
  PATH=$(DESTDIR)$(bindir):$$PATH; export PATH;

# Run the tests with the install-environment.
installcheck-local:
	$(MAKE) $(AM_MAKEFLAGS)                                 \
	  AM_TESTS_ENVIRONMENT='$$(INSTALLCHECK_ENVIRONMENT)' check
