## \file init.mk

# Initialize some common variables so that we can always use +=.  Do
# not initialize TESTS, otherwise Automake sees empty test suites
# everywhere.

AM_CFLAGS =
AM_CPPFLAGS =
AM_CXXFLAGS =
AM_DEFAULT_SOURCE_EXT = .cc
AM_LDFLAGS =
AUTOMAKE_OPTIONS = 1.11.3
# Use bzip2 only.
AUTOMAKE_OPTIONS += no-dist-gzip dist-bzip2
BUILDCHECK_ENVIRONMENT = $(CHECK_ENVIRONMENT)
# Automake 1.14 bug: no dependency from all to python_PYTHON.
BUILT_SOURCES = $(python_PYTHON) $(nodist_python_PYTHON)
CHECK_CLEANFILES =
CHECK_ENVIRONMENT =
CLEANDIRS =
CLEANFILES = $(EXTRA_PROGRAMS)
DISTCLEANFILES =
EXTRA_DATA =
EXTRA_DIST = $(dist_TESTS) $(dist_noinst_python)
EXTRA_HEADERS =
EXTRA_LTLIBRARIES =
EXTRA_PROGRAMS =
INSTALLCHECK_ENVIRONMENT = $(CHECK_ENVIRONMENT)
LIBS =
M4SHFLAGS =
MAINTAINERCLEANFILES =
MOSTLYCLEANFILES =
SUFFIXES =
TEST_EXTENSIONS =
## Do not initialize TESTS: automake would import the TESTS support
## into every Makefile.
TESTS_ENVIRONMENT =
TFAIL_TESTS =
XFAIL_TESTS =
bin_PROGRAMS =
bin_SCRIPTS =
dist_TESTS =
dist_bin_SCRIPTS =
dist_noinst_DATA =
# Cannot use dist_noinst_PYTHON with Automake 1.15 and before.
# See http://debbugs.gnu.org/cgi/bugreport.cgi?bug=24507.
dist_noinst_python =
dist_noinst_SCRIPTS =
include_HEADERS =
lib_LTLIBRARIES =
nodist_TESTS =
nodist_bin_SCRIPTS =
nodist_check_DATA =
nodist_check_SCRIPTS =
nodist_noinst_DATA =
nodist_noinst_SCRIPTS =
noinst_LTLIBRARIES =

# Because we use sysexists, which uses 77, we can't use the
# traditional 77 to mean SKIP tests.  Rather, use 176 consistently.
SKIP_STATUS = 176

# Files which are not shipped should be cleaned.
CLEANFILES += $(nodist_check_SCRIPTS) $(nodist_noinst_SCRIPTS)

# Automake still does not honor AM_LDADD.
LDADD = $(AM_LDADD)

## Useful variables.
build_aux_builddir = $(top_builddir)/build-aux
build_aux_dir = $(build_aux_srcdir)
build_aux_srcdir = $(top_srcdir)/build-aux


## ------- ##
## Clean.  ##
## ------- ##

# CLEAN_LOCAL: instead of hooking clean-local, which is troublesome
# when there are AM_CONDITIONAL used, append the target to
# CLEAN_LOCAL.
CLEAN_LOCAL = clean-cleandirs
.PHONY: $(CLEAN_LOCAL)
clean-local: $(CLEAN_LOCAL)

clean-cleandirs:
	-test -z "$(CLEANDIRS)" || rm -rf $(CLEANDIRS)


# Sometimes it is really convenient to see the output of the
# preprocessor.  But it's a pain to run the command by hand.
SUFFIXES += .i .ii
.c.i:
	$(COMPILE) -E -o $@ $<

.cc.ii:
	$(CXXCOMPILE) -E -o $@ $<

CLEANFILES += *.i *.ii

# Sometimes, the remaining # lines are a nuisance: the error is not
# reported against the preprocessed file.  And then we also want to
# compile these files, that's why I used extensions that can be
# directly compiled (*.c and *.cc).
SUFFIXES += .i.c .ii.cc
.i.i.c:
	grep -v '#' $< | tr -s '\n' '\n' >$@.tmp
	mv $@.tmp $@

.ii.ii.cc:
	grep -v '#' $< | tr -s '\n' '\n' >$@.tmp
	mv $@.tmp $@

CLEANFILES += *.i.c *.ii.cc

INSTALL_DATA_HOOKS =
install-data-hook: $(INSTALL_DATA_HOOKS)

INSTALL_EXEC_HOOKS =
install-exec-hook: $(INSTALL_EXEC_HOOKS)

UNINSTALL_HOOKS =
uninstall-hook: $(UNINSTALL_HOOKS)
