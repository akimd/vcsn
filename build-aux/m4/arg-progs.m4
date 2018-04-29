# VCSN_ARG_PROGS(PROG, DOC, [PROGRAMS])
# -------------------------------------
# Look for PROG, registered as an ARG_VAR with DOC, and store in
# $PROG (upper cased).  If not available, set as "missing PROG".
#
# For instance:
#
#   VCSN_ARG_PROGS([ipython], [the IPython environment])

AC_DEFUN([VCSN_ARG_PROGS],
[AC_ARG_VAR(m4_toupper([$1]), [$2])
AC_CHECK_PROGS(m4_toupper([$1]), [m4_default([$3], [$1])])
])


## Local Variables:
## mode: Autoconf
## End:
