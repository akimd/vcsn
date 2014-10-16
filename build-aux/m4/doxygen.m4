## --------- ##
## Doxygen.  ##
## --------- ##
AC_DEFUN([VCSN_PROG_DOXYGEN],
[AC_SUBST([DOXYGEN], [doxygen])
VCSN_ARG_PROGS([doxygen], [the Doxygen documentation generation program])
])
