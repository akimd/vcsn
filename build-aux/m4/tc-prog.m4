#                                                       -*- Autoconf -*-

# TC_PROG(PROGRAM, REQUIREMENT, VARIABLE, DESCRIPTION)
# ----------------------------------------------------
# Check whether PROGRAM version matches the REQUIREMENT.
# REQUIREMENT ::= OP VERS.
#          OP ::= < | <= | == | >= | > | !=
#        VERS ::= <digit>(.<digit>)*
#
# Define VARIABLE as an Autoconf variable for this program, along with
# its DESCRIPTION.
#
# The result is cached but with a name depending on VARIABLE, not PROGRAM,
# so that different requirement can be implemented for the same program.
AC_DEFUN([TC_PROG],
[AC_ARG_VAR([$3], [$4])
AC_CHECK_PROGS([$3], [$1])

m4_pushdef([version_re], [[^ *\([<>]\|[<>!=]=\) *\([0-9][0-9.]*\) *$]])dnl
m4_bmatch([$2],
          version_re, [m4_bregexp([$2], version_re,
                                  [tc_ver='\2'; tc_req='\1'])],
          [m4_fatal([$0: invalid argument 2: "$2"])])
m4_popdef([version_re])dnl

if test -n "$$3" ; then
  AC_CACHE_CHECK([for $1 $tc_req $tc_ver],
    [ac_cv_$3_version],
    [ac_cv_$3_actual_version=`$$3 --version | \
	sed -n ['/^[^0-9]*\([0-9][0-9.]*[0-9]\).*$/{s//\1/;p;q;}']`
    if test -z "$ac_cv_$3_actual_version"; then
    	ac_cv_$3_version=no
    else
	ac_cv_$3_version=yes
    fi
    if test x$ac_cv_$3_version = xyes ; then
      AS_VERSION_COMPARE([$ac_cv_$3_actual_version], [$tc_ver],
			 [tc_cmp='<'],
			 [tc_cmp='='],
			 [tc_cmp='>'])
      case $tc_req:$tc_cmp in
	'<:<' | \
	'<=:<' | '<=:=' |\
	'==:=' | \
	'!=:<' | '!=:>' |\
	'>=:>' | '>=:=' |\
	'>:>')
	  ac_cv_$3_version=yes
	  ;;
	*)
	  ac_cv_$3_version=no
	  ;;
      esac
    fi])
fi
if test x$ac_cv_$3_version != xyes; then
  AC_MSG_ERROR([$1 $tc_req $tc_ver is required, have $ac_cv_$3_actual_version])
fi
])
