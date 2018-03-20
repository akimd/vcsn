# Vcsn 2, a generic library for finite state machines.
# Copyright (C) 2012-2018 Vcsn Group.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `COPYING' file in the root directory.
#
# The Vcsn Group consists of people listed in the `AUTHORS' file.

m4_pattern_forbid([^(AX|BOOST|TC|URBI|VCSN)_])

AC_PREREQ([2.69])

AC_INIT([Vcsn], [2.6.dev],
        [vcsn-bugs@lrde.epita.fr], [],
        [http://vcsn.lrde.epita.fr/])
AC_CONFIG_AUX_DIR([build-aux/bin])
AC_CONFIG_MACRO_DIR([build-aux/m4])
# For Autoconf 2.70:
# AC_CONFIG_MACRO_DIRS([build-aux/m4])

AM_INIT_AUTOMAKE([1.14 tar-ustar no-define foreign dist-xz
                  color-tests parallel-tests nostdinc silent-rules
                  subdir-objects])
# install-sh and GNU install support -C (aka --compare for the
# latter).  Until we discover that we sometimes use other `install`
# programs, let's keep our old time stamps when we can, so that
# repeated `make install` keep `vcsn compile` lazy.
#
# If --preserve-timestamps is supported, use it: headers will be
# installed with their build time, not their install time.  This is
# quite critical for Vcsn: during build, we build for instance
# liblal_b`, installed with `make install`.  But now the headers are
# possibly younger than the libraries when installed, so `vcsn
# compile` will fire a useless compilation of `liblal_b`, as if it
# were obsolete.
#
# Unfortunately, --preserve-timestamps and --compare are mutually
# exclusive in GNU install, not sure why.
if ($INSTALL --preserve-timestamps --help) >/dev/null 2>&1; then
  INSTALL="$INSTALL --preserve-timestamps"
else
  INSTALL="$INSTALL -C"
fi
AC_REQUIRE_AUX_FILE([tap-driver.sh])
AM_SILENT_RULES([yes])

# C++ compiler.
AC_PROG_CXX
AC_LANG([C++])

m4_define([_AX_CXX_COMPILE_STDCXX_14_testbody],
[AC_LANG_SOURCE([
#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>

  // C++11
  template <typename T>
  struct check
  {
    static_assert(sizeof(int) <= sizeof(T), "not big enough");
  };

  using right_angle_brackets = check<check<bool>>;

  auto f = std::make_shared<std::string>("shared_ptr");

  int a;
  decltype(a) b;

  typedef check<int> check_type;
  check_type c;
  check_type&& cr = static_cast<check_type&&>(c);

  auto d = a;

  // Some versions of libstdc++ do not support std::set::emplace.
  void foo()
  {
    std::set<int> is;
    is.emplace(42);
  }

  // Clang++ 3.5, for a while, was unable to process properly
  // the for-loop because its variable, r, is a typedef...
  // It failed as follows:
  //
  // error: unexpected ':' in nested name specifier; did you mean '::'?
  //    for (auto r: std::set<int>{1, 2})
  //               ^
  //               ::
  using r = std::set<int>;
  void bar()
  {
    for (int r: std::set<int>{1, 2})
      continue;
  }

  // C++14
  void mismatch()
  {
    using ints = std::vector<int>;
    auto v1 = ints{1, 2, 3};
    auto v2 = ints{1, 2};
    std::mismatch(std::begin(v1), std::end(v1),
                  std::begin(v2), std::end(v2));
  }
])])

for f in '-std=c++14' '-std=c++14 -stdlib=libc++' \
         '-std=c++1y' '-std=c++1y -stdlib=libc++'
do
  AX_CHECK_COMPILE_FLAG([$f], [CXXFLAGS="$CXXFLAGS $f" stdpass=true], [], [],
                        [_AX_CXX_COMPILE_STDCXX_14_testbody])
  ${stdpass-false} && break
done
if ! "${stdpass-false}"; then
  AC_ERROR([unable to turn on modern C++ mode with this compiler])
fi

# Check for a long-term GCC bug that prevents proper behavior of
# tuplesets.  http://gcc.gnu.org/bugzilla/show_bug.cgi?id=51253
AC_CACHE_CHECK([whether evaluation order in braced-init-list is correct],
               [ac_cv_cxx_have_correct_list_initializer_order],
[AC_RUN_IFELSE([AC_LANG_PROGRAM(
[[struct pass
{
  template <typename... Types>
  pass(Types&&...)
  {}
};

template <int... IS>
int f()
{
  int i = 2;
  pass { i = i * IS + IS... };
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
#
# -Wno-maybe-uninitialized: See http://stackoverflow.com/questions/21755206/,
# there are too many spurious warnings from -Wmaybe-uninitialized, which is
# enabled by -Wall :(.
#
# Disable these, that cause way too many warnings we can't clearly deal with.
# [-Wsuggest-attribute=const -Wno-error=suggest-attribute=const],
# [-Wsuggest-attribute=pure -Wno-error=suggest-attribute=pure],
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
   [-Wno-maybe-uninitialized],
   [-Wsuggest-attribute=noreturn -Wno-error=suggest-attribute=noreturn],
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
# GCC7 changed something, and now the linkage with Boost.Python3 fails
# when we use visibility.  Let's make our lives simpler as a first
# step.
if false && test "$vcsn_cv_cxx_is_clang" = no; then
  AC_SUBST([VISIBILITY_CXXFLAGS])
  AX_CHECK_COMPILE_FLAG([-Werror -fvisibility=hidden],
    [VISIBILITY_CXXFLAGS="$VISIBILITY_CXXFLAGS -fvisibility=hidden"
    AX_CHECK_COMPILE_FLAG([-fvisibility-inlines-hidden],
      [VISIBILITY_CXXFLAGS="$VISIBILITY_CXXFLAGS -fvisibility-inlines-hidden"])])
fi
if test x"$VISIBILITY_CXXFLAGS" != x; then
  AC_DEFINE([HAVE_VISIBILITY_HIDDEN], 1,
            [Define to 1 if we use ELF hidden visibility by default.])
fi

# This may change CXX, which is used by Libtool.  So call _before_ libtool.
VCSN_LDFLAGS_AS_NEEDED

# Shared libs.
LT_PREREQ([2.2.6])
LT_INIT([pic-only shared disable-static])
AC_SUBST([LIBTOOL_DEPS])

BOOST_REQUIRE([1.49])
BOOST_FILESYSTEM
BOOST_FLYWEIGHT
# We will use Boost.Flyweight's intermodule_holder which uses
# Boost.Interprocess, which uses shm_open, which is in -lrt on
# GNU/Linux.
save_LIBS=$LIBS
AC_SEARCH_LIBS([shm_open], [rt])
case $ac_cv_search_shm_open in
  (-l*) AC_SUBST([RT_LIBS], [$ac_cv_search_shm_open]);;
esac
LIBS=$save_LIBS
AM_PATH_PYTHON([3.0])
BOOST_PYTHON

# Check for REGEX before _GLIBCXX_DEBUG, see below.
VCSN_REGEX
# Whether to enable _GLIBCXX_DEBUG.
VCSN_GLIBCXX_DEBUG
# For some reason, that I could not reduce to something understandable
# out of Vcsn, we have failures on ArchLinux bw clang, python, boost,
# and libstdc++: a double free.  I failed to understand what was going
# on, and wasted quite some time on it.
case $ac_cv_GLIBCXX_DEBUG:$vcsn_cv_cxx_is_clang in (yes:no)
  CPPFLAGS="$CPPFLAGS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC"
esac

# We look for our data files at runtime, including to compile.
AC_SUBST([pkgdatadir],    ['${datadir}/${PACKAGE}'])
AC_SUBST([pkglibdir],     ['${libdir}/${PACKAGE}'])
AC_SUBST([pkglibexecdir], ['${libexecdir}/${PACKAGE}'])

AC_SUBST([BINDIR],     [`URBI_RESOLVE_DIR([$bindir])`])
AC_SUBST([DATADIR],    [`URBI_RESOLVE_DIR([$pkgdatadir])`])
AC_SUBST([DOCDIR],     [`URBI_RESOLVE_DIR([$docdir])`])
AC_SUBST([INCLUDEDIR], [`URBI_RESOLVE_DIR([$includedir])`])
AC_SUBST([LIBDIR],     [`URBI_RESOLVE_DIR([$pkglibdir])`])
AC_SUBST([LIBEXECDIR], [`URBI_RESOLVE_DIR([$pkglibexecdir])`])
AC_SUBST([PYEXECDIR],  [`URBI_RESOLVE_DIR([$pyexecdir])`])

# We add $CCACHE when calling CXX, don't stack it twice.
AC_SUBST([CCACHE])
AC_SUBST([VCSN_CPPFLAGS], ["$CPPFLAGS -I$INCLUDEDIR $BOOST_CPPFLAGS"])
AC_SUBST([VCSN_CXX], [$(echo "$CXX" | sed -e "s/^ccache //;s,^$CCACHE ,,")])
AC_SUBST([VCSN_LDFLAGS], ["$LDFLAGS -L$LIBDIR -Wl,-rpath,$LIBDIR $RT_LIBS $BOOST_FLYWEIGHT_LIBS $BOOST_FILESYSTEM_LDFLAGS $BOOST_FILESYSTEM_LIBS $BOOST_SYSTEM_LDFLAGS $BOOST_SYSTEM_LIBS $BOOST_REGEX_LDFLAGS $BOOST_REGEX_LIBS"])

# We compile C++ at runtime.
VCSN_PROG_CCACHE
AC_DEFINE_UNQUOTED([DATADIR], ["$DATADIR"],
                   [Define to the location of the data files.])


## ---------- ##
## Programs.  ##
## ---------- ##

# Don't use AM_PROG_LEX since we don't use Flex support from Automake.
# Flex++ will check the existance of Flex when really needed.
AC_ARG_VAR([FLEX], [Flex, the scanner generator])
AC_CHECK_PROGS([FLEX], [flex])
AC_CONFIG_FILES([build-aux/bin/flex++], [chmod +x build-aux/bin/flex++])
AC_ARG_VAR([BISON], [Bison parser generator])
AC_CHECK_PROGS([BISON], [bison])
AC_CONFIG_FILES([build-aux/bin/bison++], [chmod +x build-aux/bin/bison++])
AC_CHECK_PROGS([PERL], [perl])


## ----------- ##
## Libraries.  ##
## ----------- ##

# ltdl.
AC_CHECK_HEADERS([ltdl.h], [],
                 [AC_MSG_ERROR([libltdl (a component from Libtool) is needed])])

# libyaml-cpp
AC_CHECK_HEADERS([yaml-cpp/yaml.h], [],
                 [AC_MSG_ERROR([yaml-cpp is needed])])
save_LIBS=$LIBS
LIBS="$LIBS -lyaml-cpp"
# This symbol is really in the library.  At least in 0.5.1.
AC_LINK_IFELSE([AC_LANG_PROGRAM([@%:@include <yaml-cpp/yaml.h>],
                                [YAML::Parser p;])],
               [AC_SUBST([YAMLCPP_LIBS], [-lyaml-cpp])],
               [AC_MSG_ERROR([yaml-cpp is needed])])
LIBS=$sa_LIBS
# yaml-cpp 0.5.2 has a broken implementation of remove.
AC_CACHE_CHECK([whether yaml-cpp's Node::remove works],
               [ac_cv_yaml_cpp_remove_works],
    [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([@%:@include <yaml-cpp/yaml.h>],
                                        [YAML::Node n; n.remove("foo");])],
                       [ac_cv_yaml_cpp_remove_works=yes],
                       [ac_cv_yaml_cpp_remove_works=no])])
if test $ac_cv_yaml_cpp_remove_works = yes; then
  AC_DEFINE([YAML_CPP_REMOVE_WORKS], 1,
            [Define if YAML::Node::remove works (broken in 0.5.2)])
fi


## --------------- ##
## Documentation.  ##
## --------------- ##

VCSN_PROG_DOXYGEN
AC_CONFIG_FILES([doc/vcsn.dox])
# Choose a simple file, hopefully fast to convert.
VCSN_PROG_NBCONVERT([$srcdir/doc/notebooks/Algorithms.ipynb])



## --------------- ##
## Configuration.  ##
## --------------- ##

# lib/config.h is unprefixed, and not installed, vcsn/config.hh is
# prefixed and installed.
AC_CONFIG_HEADERS([lib/config.h:vcsn/config.in.h])
AX_PREFIX_CONFIG_H([vcsn/config.hh], [vcsn], [lib/config.h])

AC_SUBST([GITPATCH], [$(if (git rev-parse) >/dev/null 2>&1; then
 echo ".$(git rev-list $(git rev-list --tags --no-walk --max-count=1)..HEAD \
 --count)"
fi)])

AC_CONFIG_FILES([Makefile])

AC_CONFIG_FILES([bin/vcsn], [chmod +x bin/vcsn])
AC_CONFIG_FILES([tests/bin/vcsn], [chmod +x tests/bin/vcsn])

AC_SUBST([PACKAGE_AUTHORS],
         [$(tr '\n' "," < "$srcdir/AUTHORS.md" | sed -e 's/,$//;s/,/, /g')])
AC_CONFIG_FILES([share/vcsn/config.yaml:share/vcsn/config.in.yaml])

AC_OUTPUT
