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

m4_pattern_forbid([^(TC|VCSN)_])

# VCSN_HEADER

AC_PREREQ([2.61])

AC_INIT([vaucanson], [2.0a], [vaucanson-bugs@lrde.epita.fr])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AM_INIT_AUTOMAKE([1.11 tar-ustar no-define foreign dist-xz
                  color-tests parallel-tests silent-rules])
AM_SILENT_RULES([yes])

AC_PROG_CXX

AC_LANG(C++)

AX_CHECK_COMPILE_FLAG([-std=c++11],
                      [stdfailed=false  CXXFLAGS="$CXXFLAGS -std=c++11"])
if test "${stdfailed-true}"; then
  AX_CHECK_COMPILE_FLAG([-std=c++0x],
                        [stdfailed=false  CXXFLAGS="$CXXFLAGS -std=c++0x"])
fi
if "${stdfailed-true}"; then
  AC_ERROR([Unable to turn on C++11 mode with this compiler.])
fi

CF_GXX_WARNINGS

## -------------------- ##
## Auxiliary programs.  ##
## -------------------- ##

TC_PROG([flex], [>= 2.5.4], [FLEX], [Flex scanner generator])
AM_PROG_LEX
TC_PROG([bison], [>= 2.4], [BISON], [Bison parser generator])
AC_CONFIG_FILES([build-aux/bin/bison++], [chmod +x build-aux/bin/bison++])

AC_CONFIG_FILES([
  Makefile
  vcsn/Makefile
  tests/Makefile
  tests/unit/Makefile
])

AC_OUTPUT
