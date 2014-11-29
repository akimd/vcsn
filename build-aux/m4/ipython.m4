## ----------- ##
## nbconvert.  ##
## ----------- ##

m4_define([_vcsn_try],
[echo "$1" >&AS_MESSAGE_LOG_FD
$1 >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD
])

# _VCSN_PROG_NBCONVERT(CACHE-ID, NB-FILE)
# ---------------------------------------
# Check whether ipython supports the nbconvert subcommand.
# Unfortunately, it appears there is no clean way to run 'ipython
# nbconvert' on /dev/null for instance to check for its support, so
# require a real notebook for the test.  Checking --help only does not
# suffice, because it may work, but not nbconvert, if for instance,
# neither node.js nor pandoc is installed.
AC_DEFUN([_VCSN_PROG_NBCONVERT],
[test -f "$2" ||
  AC_MSG_ERROR([cannot read the nbconvert test file: $2])
_vcsn_try([$IPYTHON nbconvert --output=conftest "$2"])
if test -f conftest.html; then
  $1='$(IPYTHON) nbconvert'
else
  $1='no'
fi
])

# VCSN_PROG_NBCONVERT(NB-FILE)
# ----------------------------
AC_DEFUN([VCSN_PROG_NBCONVERT],
[VCSN_ARG_PROGS([ipython], [the IPython environment])
AC_CACHE_CHECK([for ipython nbconvert],
               [vcsn_cv_nbconvert],
               [_VCSN_PROG_NBCONVERT([vcsn_cv_nbconvert], [$1])])
AC_SUBST([NBCONVERT], [$vcsn_cv_nbconvert])
AM_CONDITIONAL([HAVE_NBCONVERT], [test x"$NBCONVERT" != xno])
])
