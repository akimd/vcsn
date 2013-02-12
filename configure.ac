# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012-2013 Vaucanson Group.
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

m4_pattern_forbid([^(TC|VCSN)_])

# VCSN_HEADER

AC_PREREQ([2.61])

AC_INIT([Vaucanson], [2.0a], [vaucanson-bugs@lrde.epita.fr])
AC_CONFIG_AUX_DIR([build-aux/bin])
AC_CONFIG_MACRO_DIR([build-aux/m4])

AM_INIT_AUTOMAKE([1.11 tar-ustar no-define foreign dist-xz
                  color-tests parallel-tests nostdinc silent-rules
                  subdir-objects])
AM_SILENT_RULES([yes])

LT_PREREQ([2.2.6])
LT_INIT([pic-only shared disable-static])
AC_SUBST([LIBTOOL_DEPS])


AC_PROG_CXX
AC_LANG([C++])
AX_CHECK_COMPILE_FLAG([-std=c++11 -stdlib=libc++],
                      [CXXFLAGS="$CXXFLAGS -std=c++11 -stdlib=libc++"
                       stdpass=true])
${stdpass-false} ||
  AX_CHECK_COMPILE_FLAG([-std=c++11],
                        [CXXFLAGS="$CXXFLAGS -std=c++11"
                         stdpass=true])
${stdpass-false} ||
  AC_ERROR([unable to turn on C++11 mode with this compiler.])

CF_GXX_WARNINGS

# For some reason, on OS X, enabling _GLIBCXX_DEBUG breaks the
# stringstreams.  This is well known, ancient, yet still the case
# today (G++ 4.7, XCode 4.2).
# http://stackoverflow.com/questions/7623070/stringstream-not-working-with-doubles-when-glibcxx-debug-enabled
AC_CACHE_CHECK([whether to enable _GLIBCXX_DEBUG],
               [ac_cv_GLIBCXX_DEBUG],
[AC_RUN_IFELSE(
[AC_LANG_PROGRAM([#define _GLIBCXX_DEBUG
#define _GLIBCXX_DEBUG_PEDANTIC
#include <sstream>],
[  {
     std::stringstream o;
     assert (o.str() == "");
  }
  {
     std::stringstream o;
      o << 42;
      assert (o.str() == "42");
  }
])], [ac_cv_GLIBCXX_DEBUG=yes],
     [ac_cv_GLIBCXX_DEBUG='no (breaks stringstreams)'],
     [ac_cv_GLIBCXX_DEBUG='no (cross-compilation)'])])
case $ac_cv_GLIBCXX_DEBUG in yes)
  CPPFLAGS="$CPPFLAGS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC"
esac

## --------------------##
## Define XML headers. ##
## --------------------##

AC_DEFINE_UNQUOTED([VCSN_XMLNS], ["http://vaucanson.lrde.epita.fr"],
       [Root of the XML namespace.])
AC_DEFINE_UNQUOTED([FSMXML_VERSION], ["1.1"],
       [FSMXML version.])

## -------------------- ##
## Auxiliary programs.  ##
## -------------------- ##

TC_PROG([flex], [>= 2.5.4], [FLEX], [Flex scanner generator])
AM_PROG_LEX
TC_PROG([bison], [>= 2.5], [BISON], [Bison parser generator])
AC_CONFIG_FILES([build-aux/bin/bison++], [chmod +x build-aux/bin/bison++])
AC_CONFIG_HEADERS([vcsn/config.h:vcsn/config.in.h])
AC_CONFIG_FILES([
  Makefile
  vcsn/Makefile
  tests/Makefile
])

AC_OUTPUT
