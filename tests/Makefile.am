include $(top_srcdir)/build-aux/make/init.mk

AM_CXXFLAGS += $(WARNING_CXXFLAGS)
AM_CPPFLAGS += -I$(top_srcdir)

dist_noinst_SCRIPTS += checker
TEST_EXTENSIONS += .chk
CHK_LOG_COMPILER = $(srcdir)/checker

TESTS = $(dist_TESTS)

include rat/local.mk
include unit/local.mk
