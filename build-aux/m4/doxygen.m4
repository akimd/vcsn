## --------- ##
## Doxygen.  ##
## --------- ##
AC_DEFUN([VCSN_PROG_DOXYGEN],
[VCSN_ARG_ENABLE([enable-doxygen],
                 [generate the documentation with Doxygen],
                 [no|yes], [no])
VCSN_ARG_PROGS([doxygen], [the Doxygen documentation generation program])
AM_CONDITIONAL([ENABLE_DOXYGEN], [test $enable_doxygen = yes])
])
