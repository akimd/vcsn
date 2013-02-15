* To add a new pre-built context "foo"
** create a new vcsn/ctx/foo.hh file
** register it in vcsn/Makefile.am
alphabetical order
** register it in common.mk
libfoo    = $(top_builddir)/lib/libfoo.la
...
all_libctx = ...
  ... $(libfoo)

** register it in lib/local.mk
pkglib_LTLIBRARIES += lib/libfoo.la
lib_libfoo_la_SOURCES = lib/ctx/libctx.cc
lib_libfoo_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=foo

Local Variables:
mode: outline
End:
