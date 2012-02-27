dist_noinst_SCRIPTS = rat_exp/rat_exp.pl
TEST_EXTENSIONS = .rat_exp
RAT_EXP_LOG_COMPILER = $(srcdir)/rat_exp/rat_exp.pl
AM_RAT_EXP_LOG_FLAGS = $(top_builddir)/vcsn/io/rat-exp

dist_TESTS += rat_exp/test.rat_exp
