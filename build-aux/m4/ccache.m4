## --------- ##
## ccache.   ##
## --------- ##
AC_DEFUN([VCSN_PROG_CCACHE],
[AC_ARG_VAR([CCACHE], [the compiler cache program])
AC_CHECK_PROGS([CCACHE], [ccache], [no])
if test x"$CCACHE" = xno; then
  AC_MSG_ERROR([ccache is required])
fi
])
