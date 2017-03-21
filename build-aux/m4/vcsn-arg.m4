## Wrappers around AC_ARG_ENABLE and AC_ARG_WITH.

## For instance:
##
##    VCSN_ARG_ENABLE([enable-doxygen],
##                    [enable documentation generation with Doxygen],
##                    [yes|no], [no])
##    if test x$enable_doxygen = xyes; then
##      ...
##    fi
##
## This creates an option --enable-doxygen (so, of course, the default
## value is "no").  Accepted values are "yes" and "no".  The result is
## in $enable_doxygen (even if the option is actually --disable-doxygen).
##
## If you use VCSN_ARG_WITH, the result will be in $with_OPTION instead.


# _VCSN_ARG_OPTION_BRE
# --------------------
# A BRE that matches valid options for _VCSN_ARG.  Groups:
# 1. the kind (enable/disable or with/without)
# 2. the feature actually used
# 3. if defined "=default-value"
# 4. if defined the default-value
m4_define([_VCSN_ARG_OPTION_BRE],
          [^\(disable\|enable\|with\|without\)-\([^=]*\)\(=\(.*\)\)?$])


# _VCSN_ARG_MATCH(STRING, GROUP)
# ------------------------------
m4_define([_VCSN_ARG_MATCH],
[m4_bpatsubst([$1],
              m4_defn([_VCSN_ARG_OPTION_BRE]), [\$2])])


# _VCSN_ARG(KIND, OPTION, HELP-STRING,
#           [RANGE = .*],
#           [DEFAULT], [MORE-HELP])
# ------------------------------------
# KIND is either "enable" (AC_ARG_ENABLE) or "with" (AC_ARG_DISABLE).
#
# RANGE is an ERE pattern (that will be anchored on both ends)
# that accepts the valid values.  It defaults to '.*', i.e., accepting
# anything.
#
# DEFAULT is the default value, defaulting to empty.  Don't forget to
# double-quote if you need square brackets.
#
# OPTION should look like `enable-foo=bar', or `disable-bar' depending
# on the expected --help output.  Should be consistent with the DEFAULT:
# if the default is `no', use `enable-*', otherwise `disable-*'.
#
# MORE-HELP is output as if in the output, while HELP-STRING is
# formatted via AC_HELP_STRING.
AC_DEFUN([_VCSN_ARG],
[AC_REQUIRE([AC_PROG_EGREP])dnl
m4_if([m4_bregexp([$2], m4_defn([_VCSN_ARG_OPTION_BRE]))],
      [-1], [m4_fatal([$0: invalid OPTION: --$2])])dnl
m4_pushdef([AC_feature],
           [_VCSN_ARG_MATCH([$2], [2])])dnl
m4_indir(m4_case([$1],
                 [enable], [[AC_ARG_ENABLE]],
                 [with],   [[AC_ARG_WITH]],
                 [m4_fatal([$0: invalid kind: $1])]),
         AC_feature,
         [AC_HELP_STRING([--$2], [$3 [$5]])[]$6],
   [if echo "$$1val" | $EGREP '^(m4_default([$4], [.*]))$' >/dev/null; then :; else
     AC_MSG_ERROR([--$2: bad value: $$1val])
    fi],
   [$1_[]m4_bpatsubst(AC_feature, [-], [_])=[$5]])
m4_popdef([AC_feature])dnl
])


# VCSN_ARG(OPTION, HELP-STRING, [RANGE = .*], [MORE-HELP])
# --------------------------------------------------------
# For instance:
#
#  VCSN_ARG([with-loquendo=/opt/Loquendo/LTTS7],
#           [path to loquendo library])
AC_DEFUN([VCSN_ARG],
[_VCSN_ARG(_VCSN_ARG_MATCH([$1], [1]),
           _VCSN_ARG_MATCH([$1], [1])-_VCSN_ARG_MATCH([$1], [2]),
           [$2],
           [$3],
           _VCSN_ARG_MATCH([$1], [4]),
           [$4])dnl
])


# VCSN_ARG_ENABLE(OPTION, HELP-STRING,
#                 [RANGE = '.*'], [DEFAULT = ''], [MORE-HELP])
# ------------------------------------------------------------
AC_DEFUN([VCSN_ARG_ENABLE],
[_VCSN_ARG([enable], [$1], [$2], [$3], [$4], [$5])])


# VCSN_ARGLIST_ENABLE(OPTION, HELP-STRING,
#                     RANGE, [DEFAULT = ''], [MORE-HELP])
# -------------------------------------------------------
# Same as VCSN_ARG_ENABLE, but accept a coma-seperated list of args
# in RANGE.
AC_DEFUN([VCSN_ARGLIST_ENABLE],
[VCSN_ARG_ENABLE([$1], [$2], [($3)(,($3))*], [$4], [$5])])


# VCSN_ARG_WITH(OPTION, HELP-STRING,
#                 [RANGE = '.*'], [DEFAULT = ''], [MORE-HELP])
# ------------------------------------------------------------
AC_DEFUN([VCSN_ARG_WITH],
[_VCSN_ARG([with], [$1], [$2], [$3], [$4], [$5])])


# VCSN_ARGLIST_WITH(OPTION, HELP-STRING,
#                   RANGE, [DEFAULT = ''], [MORE-HELP])
# -------------------------------------------------------
AC_DEFUN([VCSN_ARGLIST_WITH],
[VCSN_ARG_WITH([$1], [$2], [($3)(,($3))*], [$4], [$5])])


## Local Variables:
## mode: autoconf
## End:
