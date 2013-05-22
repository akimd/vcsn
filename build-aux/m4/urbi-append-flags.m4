## Copyright (C) 2009-2010, 2012, Gostai S.A.S.
##
## This software is provided "as is" without warranty of any kind,
## either expressed or implied, including but not limited to the
## implied warranties of fitness for a particular purpose.
##
## See the LICENSE file for more information.

## \file urbi-append-flags.m4
## This file is part of build-aux.

m4_pattern_forbid([^URBI_])
m4_pattern_forbid([^TC_])

AC_PREREQ([2.60])

AC_DEFUN([_URBI_APPEND_FLAGS],
[
# urbi_append_flags VAR FLAG...
# ----------------------------
# Append the FLAG... to $VAR, separated by spaces, unless it already
# includes it.
urbi_append_flags ()
{
  local var="$[1]"
  local var
  eval "val=\$$var"
  shift
  local v
  for v
  do
    case " $val " in
      (*" $v "*) ;;
      ("  ")    val=$v;;
      (*)       val="$val $v";;
    esac
  done
  eval "$var=\$val"
}
])


# URBI_APPEND_FLAGS VAR FLAGS
# ---------------------------
# Append the FLAG... to $VAR, separated by spaces, unless it already
# includes it.
AC_DEFUN([URBI_APPEND_FLAGS],
[AC_SUBST([$1])dnl
AC_REQUIRE([_URBI_APPEND_FLAGS])dnl
urbi_append_flags "$1" m4_join([ ], m4_shift($@))[]dnl
])

# URBI_APPEND_CONFIGUREFLAGS FLAGS
# --------------------------------
AC_DEFUN([URBI_APPEND_CONFIGUREFLAGS],
[URBI_APPEND_FLAGS([ac_configure_args], $@)])

# URBI_APPEND_CPPFLAGS FLAGS
# --------------------------
AC_DEFUN([URBI_APPEND_CPPFLAGS],
[URBI_APPEND_FLAGS([CPPFLAGS], $@)])

# URBI_APPEND_CFLAGS FLAGS
# ------------------------
AC_DEFUN([URBI_APPEND_CFLAGS],
[URBI_APPEND_FLAGS([CFLAGS], $@)])

# URBI_APPEND_CXXFLAGS FLAGS
# --------------------------
AC_DEFUN([URBI_APPEND_CXXFLAGS],
[URBI_APPEND_FLAGS([CXXFLAGS], $@)])

# URBI_APPEND_COMPILERFLAGS FLAGS
# -------------------------------
AC_DEFUN([URBI_APPEND_COMPILERFLAGS],
[URBI_APPEND_CFLAGS($@)
URBI_APPEND_CXXFLAGS($@)])

# URBI_APPEND_DISTCHECK_CONFIGUREFLAGS
# ------------------------------------
AC_DEFUN([URBI_APPEND_DISTCHECK_CONFIGUREFLAGS],
[URBI_APPEND_FLAGS([DISTCHECK_CONFIGURE_FLAGS], $@)])



## Local Variables:
## mode: Autoconf
## End:
