# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012-2014 Vaucanson Group.
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

m4_pattern_forbid([^(AX|BOOST|TC|URBI|VCSN)_])

AC_PREREQ([2.61])

AC_INIT([Vaucanson], [2b.3],
        [vaucanson-bugs@lrde.epita.fr], [],
        [http://vaucanson.lrde.epita.fr/])
AC_CONFIG_AUX_DIR([build-aux/bin])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AC_CONFIG_MACRO_DIRS([build-aux/m4])

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

m4_define([_AX_CXX_COMPILE_STDCXX_11_testbody],
[AC_LANG_SOURCE([#include <memory>
#include <string>

   template <typename T>
    struct check
    {
      static_assert(sizeof(int) <= sizeof(T), "not big enough");
    };

    typedef check<check<bool>> right_angle_brackets;

    auto f = std::make_shared<std::string>("shared_ptr");

    int a;
    decltype(a) b;

    typedef check<int> check_type;
    check_type c;
    check_type&& cr = static_cast<check_type&&>(c);

    auto d = a;
])])

for f in '-std=c++11' '-std=c++11 -stdlib=libc++'
do
  AX_CHECK_COMPILE_FLAG([$f], [CXXFLAGS="$CXXFLAGS $f" stdpass=true], [], [],
                        [_AX_CXX_COMPILE_STDCXX_11_testbody])
  ${stdpass-false} && break
done
if ! "${stdpass-false}"; then
  AC_ERROR([unable to turn on C++11 mode with this compiler])
fi

# Check for a long-term GCC bug that prevents proper behavior of
# tuplesets.  http://gcc.gnu.org/bugzilla/show_bug.cgi?id=51253
AC_CACHE_CHECK([whether evaluation order in braced-init-list is correct],
               [ac_cv_cxx_have_correct_list_initializer_order],
[AC_RUN_IFELSE([AC_LANG_PROGRAM(
[[struct swallow{
  template<typename... Types>
  swallow(Types &&...){}
};

template<int... IS>
int f()
{
  int i = 2;
  swallow{ i = i * IS + IS... };
  return i;
}]],
[[
  return f<2, 3>() == 21 ? 0 : 1;
]])],
    [ac_cv_cxx_have_correct_list_initializer_order=yes],
    [ac_cv_cxx_have_correct_list_initializer_order=no])])
AM_CONDITIONAL([HAVE_CORRECT_LIST_INITIALIZER_ORDER],
               [test "$ac_cv_cxx_have_correct_list_initializer_order" = yes])
case $ac_cv_cxx_have_correct_list_initializer_order in (yes)
  AC_DEFINE([HAVE_CORRECT_LIST_INITIALIZER_ORDER], 1,
            [Define to 1 if evaluation order in braced-init-list is correct])
esac

# Do not apply to C++: -Wbad-function-cast, -Wmissing-prototypes,
# -Wstrict-prototypes.
TC_CXX_WARNINGS(
  [[-Wall],
   [-Wextra],
   [-Wcast-align],
   [-Wcast-qual],
   [-Wdocumentation],
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
#
# I (Akim) spent a large amount of time trying to understand how to
# get clang++ to behave properly with -fvisibility=hidden, and failed.
# I'm tired of this.  Be my guest, and try to go further, see the
# branch ad/clang-visibility.  See also
# <http://stackoverflow.com/questions/19496643>.
AC_CACHE_CHECK([whether $CXX is clang], [vcsn_cv_cxx_is_clang],
[AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#ifndef __clang__
       choke me
#endif
]])], [vcsn_cv_cxx_is_clang=yes],
      [vcsn_cv_cxx_is_clang=no])])
if test "$vcsn_cv_cxx_is_clang" = no; then
  AC_SUBST([VISIBILITY_CXXFLAGS])
  AX_CHECK_COMPILE_FLAG([-Werror -fvisibility=hidden],
    [VISIBILITY_CXXFLAGS="$VISIBILITY_CXXFLAGS -fvisibility=hidden"
    AX_CHECK_COMPILE_FLAG([-fvisibility-inlines-hidden],
      [VISIBILITY_CXXFLAGS="$VISIBILITY_CXXFLAGS -fvisibility-inlines-hidden"])])
fi

# Check for Python, for the VCSN Python interface.
AM_PATH_PYTHON

BOOST_REQUIRE([1.49])
BOOST_FILESYSTEM
BOOST_FLYWEIGHT
BOOST_PYTHON

# Check for REGEX before _GLIBCXX_DEBUG, see below.
VCSN_REGEX
# Whether to enable _GLIBCXX_DEBUG.
VCSN_GLIBCXX_DEBUG
case $ac_cv_GLIBCXX_DEBUG in (yes)
  CPPFLAGS="$CPPFLAGS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC"
esac

# Use "vcsn", not "vaucanson".
AC_SUBST([pkglibdir], ['${libdir}/vcsn'])

# We look for our data files at runtime, including to compile.
AC_SUBST([DATADIR],    [`URBI_RESOLVE_DIR([$datadir/vcsn])`])
AC_SUBST([INCLUDEDIR], [`URBI_RESOLVE_DIR([$includedir])`])
AC_SUBST([LIBDIR],     [`URBI_RESOLVE_DIR([$pkglibdir])`])
AC_SUBST([PYEXECDIR],  [`URBI_RESOLVE_DIR([$pyexecdir])`])

# We compile C++ at runtime.
AC_DEFINE_UNQUOTED([CPPFLAGS], ["$CPPFLAGS -I$INCLUDEDIR"],
                   [Define to the preprocessor flags.])
AC_DEFINE_UNQUOTED([CXX], ["$CXX"],
                   [Define to the C++ compiler.])
AC_DEFINE_UNQUOTED([CXXFLAGS], ["$CXXFLAGS"],
                   [Define to the C++ compiler flags.])
AC_DEFINE_UNQUOTED([DATADIR], ["$DATADIR"],
                   [Define to the location of data files.])
AC_DEFINE_UNQUOTED([INCLUDEDIR], ["INCLUDEDIR"],
                   [Define to the location of header files.])
AC_DEFINE_UNQUOTED([LDFLAGS], ["$LDFLAGS -L$LIBDIR"],
                   [Define to the linker flags.])
AC_DEFINE_UNQUOTED([LIBDIR], ["$LIBDIR"],
                   [Define to the location of libs files.])

## -------------------- ##
## Auxiliary programs.  ##
## -------------------- ##

# Flex 2.3.33 has a problem with REJECT and produces a scanner that
# does not compile.
TC_PROG([flex], [>= 2.5.35], [FLEX], [Flex scanner generator])
AM_PROG_LEX
AC_CONFIG_FILES([build-aux/bin/flex++], [chmod +x build-aux/bin/flex++])
AC_ARG_VAR([BISON], [Bison parser generator])
AC_CHECK_PROGS([BISON], [bison])
AC_CONFIG_FILES([build-aux/bin/bison++], [chmod +x build-aux/bin/bison++])

# lib/config.h is unprefixed, and not installed, vcsn/config.hh is
# prefixed and installed.
AC_CONFIG_HEADERS([lib/config.h:vcsn/config.in.h])
AX_PREFIX_CONFIG_H([vcsn/config.hh], [vcsn], [lib/config.h])

AC_CONFIG_FILES([Makefile])

AC_CONFIG_FILES([bin/vcsn], [chmod +x bin/vcsn])
AC_CONFIG_FILES([tests/bin/vcsn], [chmod +x tests/bin/vcsn])

AC_CONFIG_FILES([python/vcsn_version.py:python/vcsn_version.py.in])

AC_OUTPUT
