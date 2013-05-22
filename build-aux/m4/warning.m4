## Copyright (C) 2006-2007, 2009-2012, Gostai S.A.S.
##
## This software is provided "as is" without warranty of any kind,
## either expressed or implied, including but not limited to the
## implied warranties of fitness for a particular purpose.
##
## See the LICENSE file for more information.

## \brief Finding valid warning flags for the C Compiler.

# serial 2

# AC_COMPILE_STDERR_IFELSE([PROGRAM],
#                          [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ------------------------------------------------------------------
# Try to compile PROGRAM, and log the result of the compiler run.
m4_define([AC_COMPILE_STDERR_IFELSE],
[AC_LANG_COMPILER_REQUIRE()dnl
m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
rm -f conftest.$ac_objext
AS_IF([_AC_EVAL_STDERR($ac_compile) &&
         AC_TRY_COMMAND([test -s conftest.$ac_objext])],
      [$2],
      [_AC_MSG_LOG_CONFTEST
m4_ifvaln([$3],[$3])dnl])
rm -f conftest.$ac_objext m4_ifval([$1], [conftest.$ac_ext])[]dnl
])# AC_COMPILE_STDERR_IFELSE


# TC_COMPILER_FLAGS_NAME
# ----------------------
# Return the name of the FLAGS variable for the current language.
m4_define([TC_COMPILER_FLAGS_NAME],
[AC_LANG_CASE([C],   [CFLAGS],
              [C++], [CXXFLAGS])[]dnl
])# TC_COMPILER_FLAGS_NAME


# TC_COMPILER_OPTION_IF(OPTION, [IF-SUPPORTED], [IF-NOT-SUPPORTED])
# -----------------------------------------------------------------
# icc doesn't choke on unknown options, it will just issue warnings
# (even with -Werror).  So we grep stderr for any message that says an
# option was ignored.  For instance on "-Wall" it says:
#
#   icc: Command line warning: ignoring option '-W'; no argument required
#
# And GCC sometimes follows an irregular scheme (this is 3.3):
#
#   g++: unrecognized option `-wd654'
#   cc1plus: warning: "-Wbad-function-cast" is valid for C/ObjC but not for C++.
#   cc1plus: warning: ignoring command line option '-Wbad-function-cast'
#
# clang is happily accepting any -w*, such as those from ICC.
#
#   $ clang -wfoobar /tmp/true.cc -Wall -Werror
#   clang: warning: argument unused during compilation: '-wfoobar'
#
# it also recognizes, but warns about, unrecognized warning:
#
#   $ clang  /tmp/foo.cc -Wfoo
#   warning: unknown warning option '-Wfoo' [-Wunknown-warning-option]
#   1 warning generated.
#
# It can be eliminated thanks to -Werror=unknown-warning-option, but
# currently this framework is not ready to pass such additional flags,
# so let's just grep it.
#
#   $ clang  /tmp/foo.cc  -Werror=unknown-warning-option -Wfoo
#   error: unknown warning option '-Wfoo' [-Wunknown-warning-option]
#
# Pay attention not to give grep a regexp starting with "-".
AC_DEFUN([TC_COMPILER_OPTION_IF],
[AC_REQUIRE([AC_PROG_EGREP])dnl
AS_VAR_PUSHDEF([ac_Option],
               [ac_cv_[]_AC_LANG_ABBREV[]_supports_$1])dnl
AC_CACHE_CHECK([whether _AC_LANG compiler accepts $1],
               [ac_Option],
[ac_save_[]TC_COMPILER_FLAGS_NAME=$TC_COMPILER_FLAGS_NAME
URBI_APPEND_FLAGS([TC_COMPILER_FLAGS_NAME], [$1])
AS_VAR_SET([ac_Option], [no])
AC_COMPILE_STDERR_IFELSE([AC_LANG_PROGRAM],
  [# Get the first option, in case we are testing several at once
   # (bundled options that we either take all together, or not at all).
   set x $1
   ac_tc_compiler_first=$[]2
   if ($EGREP "m4_join([|],
        [ignoring option],
        [ignoring command line option .$ac_tc_compiler_first],
        [unrecognized option .*$ac_tc_compiler_first],
        [option.*$ac_tc_compiler_first.*not supported],
        [warning: argument unused during compilation: .$ac_tc_compiler_first],
        [warning: unknown warning option .*$ac_tc_compiler_first],
        [$ac_tc_compiler_first.* is valid for.*but not for])" \
          conftest.err
      ) >/dev/null 2>&1; then :; else
      AS_VAR_SET([ac_Option], [yes])
   fi])
TC_COMPILER_FLAGS_NAME=$ac_save_[]TC_COMPILER_FLAGS_NAME
])
AS_IF([test AS_VAR_GET(ac_Option) = yes], [$2], [$3])
AS_VAR_POPDEF([ac_Option])dnl
])# TC_COMPILER_OPTION_IF


# TC_CXX_WARNINGS(OPTIONS)
# ------------------------
# Check whether the C++ compiler supports all the OPTIONS (space separated).
# If it does append them to WARNING_CXXFLAGS.
# It is important to try the options all together, not just individually,
# as for instance for GCC 3.2 '-Wbad-function-cast' after '-W' gives:
#
# cc1plus: warning: ignoring command line option '-Wbad-function-cast'
#
# but not when used alone.  This also demonstrates the importance of the
# order.
AC_DEFUN([TC_CXX_WARNINGS],
[AC_LANG_PUSH([C++])
ac_save_compiler_flags=$TC_COMPILER_FLAGS_NAME
m4_foreach([AC_Option], [$1],
    [TC_COMPILER_OPTION_IF(AC_Option,
           [URBI_APPEND_FLAGS([WARNING_CXXFLAGS], "AC_Option")])])
TC_COMPILER_FLAGS_NAME=$ac_save_compiler_flags
AC_SUBST([WARNING_CXXFLAGS])
AC_LANG_POP([C++])
])# TC_CXX_WARNINGS(OPTIONS)


## Local Variables:
## mode: autoconf
## End:
