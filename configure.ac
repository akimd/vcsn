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

m4_pattern_forbid([^(BOOST|TC|URBI|VCSN)_])

# VCSN_HEADER

AC_PREREQ([2.61])

AC_INIT([Vaucanson], [2.0a], [vaucanson-bugs@lrde.epita.fr])
AC_CONFIG_AUX_DIR([build-aux/bin])
AC_CONFIG_MACRO_DIR([build-aux/m4])

AM_INIT_AUTOMAKE([1.14 tar-ustar no-define foreign dist-xz
                  color-tests parallel-tests nostdinc silent-rules
                  subdir-objects])
AC_REQUIRE_AUX_FILE([tap-driver.pl])
AM_SILENT_RULES([yes])

LT_PREREQ([2.2.6])
LT_INIT([pic-only shared disable-static])
AC_SUBST([LIBTOOL_DEPS])

# C++ compiler.
AC_PROG_CXX
AC_LANG([C++])
AX_CHECK_COMPILE_FLAG([-std=c++11 -stdlib=libc++],
                      [CXXFLAGS="$CXXFLAGS -std=c++11 -stdlib=libc++"
                       stdpass=true])
if ! "${stdpass-false}"; then
  AX_CHECK_COMPILE_FLAG([-std=c++11],
                        [CXXFLAGS="$CXXFLAGS -std=c++11"
                         stdpass=true])
fi

if ! "${stdpass-false}"; then
  AC_ERROR([unable to turn on C++11 mode with this compiler])
fi

m4_define([_AX_CXX_COMPILE_STDCXX_11_testbody], [
  template <typename T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    typedef check<check<bool>> right_angle_brackets;

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c;
    check_type&& cr = static_cast<check_type&&>(c);

    auto d = a;
])

AC_CACHE_CHECK([whether $CXX $CXXFLAGS supports C++11 features],
               [ax_cv_cxx_compile_cxx11],
[AC_COMPILE_IFELSE([AC_LANG_SOURCE([_AX_CXX_COMPILE_STDCXX_11_testbody])],
  [ax_cv_cxx_compile_cxx11=yes],
  [ax_cv_cxx_compile_cxx11=no])])

test "x$ax_cv_cxx_compile_cxx11" = xyes ||
  AC_ERROR([unable to compile C++11 with this compiler])

# Do not apply to C++: -Wbad-function-cast, -Wmissing-prototypes,
# -Wstrict-prototypes.
TC_CXX_WARNINGS(
  [[-Wall],
   [-Wextra],
   [-Wcast-align],
   [-Wcast-qual],
   [-Wextra],
   [-Wformat],
   [-Wmissing-declarations],
   [-Wno-parentheses],
   [-Wnoexcept],
   [-Woverloaded-virtual],
   [-Wpointer-arith],
   [-Wsuggest-attribute=const -Wno-error=suggest-attribute=const],
   [-Wsuggest-attribute=noreturn -Wno-error=suggest-attribute=noreturn],
   [-Wsuggest-attribute=pure -Wno-error=suggest-attribute=pure],
   [-Wwrite-strings],
   [-Wzero-as-null-pointer-constant]])

# Use -Werror since using -fvisibility under MinGW is only a warning.
# (The option is ignored anyway since this does not make sense under windows).

# When -fvisibility=hidden is on, std:: objects cannot be passed between
# CXX_DEBUG and non CXX_DEBUG code.  As a consequence, _GLIBCXX_DEBUG is
# unusable in our environment.
AC_SUBST([VISIBILITY_CXXFLAGS])
AX_CHECK_COMPILE_FLAG([-Werror -fvisibility=hidden],
  [VISIBILITY_CXXFLAGS="$VISIBILITY_CXXFLAGS -fvisibility=hidden"
  AX_CHECK_COMPILE_FLAG([-fvisibility-inlines-hidden],
    [VISIBILITY_CXXFLAGS="$VISIBILITY_CXXFLAGS -fvisibility-inlines-hidden"])])

# Check for REGEX before _GLIBCXX_DEBUG, see below.
BOOST_REQUIRE([1.49])
VCSN_REGEX

# For some reason, on OS X, enabling _GLIBCXX_DEBUG breaks the
# stringstreams.  This is well known, ancient, yet still the case
# today (G++ 4.7, XCode 4.2).
# http://stackoverflow.com/questions/7623070
#
# If we use Boost.Regex, do not enable _GLIBCXX_DEBUG if the result fails.
# https://svn.boost.org/trac/boost/ticket/5911
AC_CACHE_CHECK([whether to enable _GLIBCXX_DEBUG],
               [ac_cv_GLIBCXX_DEBUG],
[vcsn_save_LIBS=$LIBS
LIBS="$LIBS $BOOST_REGEX_LDFLAGS $BOOST_REGEX_LIBS"
AC_RUN_IFELSE(
[AC_LANG_PROGRAM([#define _GLIBCXX_DEBUG
#define _GLIBCXX_DEBUG_PEDANTIC
#include <cassert>
#include <sstream>
],[
  {
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
     [ac_cv_GLIBCXX_DEBUG='no (cross-compilation)'])

if test "$ac_cv_GLIBCXX_DEBUG" = yes && test -n "$BOOST_REGEX_LIBS"; then
  vcsn_save_LIBS=$LIBS
  LIBS="$LIBS $BOOST_REGEX_LDFLAGS $BOOST_REGEX_LIBS"
  AC_RUN_IFELSE(
  [AC_LANG_PROGRAM([#define _GLIBCXX_DEBUG
  #define _GLIBCXX_DEBUG_PEDANTIC
  #include <cassert>
  #include <sstream>
  #include <boost/regex.hpp>
  ],[
    {
      const boost::regex re("[a-zA-Z]*");
      assert(boost::regex_match("asdf", re));
    }
  ])], [],
       [ac_cv_GLIBCXX_DEBUG='no (Boost.Regexp)'])
  LIBS=$vcsn_save_LIBS
fi
])

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
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
