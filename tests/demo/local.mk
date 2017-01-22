## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2013-2017 Vaucanson Group.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## The complete GNU General Public Licence Notice can be found as the
## `COPYING' file in the root directory.
##
## The Vaucanson Group consists of people listed in the `AUTHORS' file.

# Not check_PROGRAMS, see below why.
EXTRA_PROGRAMS +=                               \
  %D%/compose                                   \
  %D%/operators                                 \
  %D%/prod-eval                                 \
  %D%/sms2fr-static                             \
  %D%/sms2fr-dyn

EXTRA_DIST += %D%/sms2fr.py %D%/sms2fr-tests.py

%C%_compose_LDADD = $(all_libctx) $(unit_ldadd)
%C%_operators_LDADD = $(all_libctx) $(unit_ldadd)
%C%_prod_eval_LDADD = $(all_libctx) $(unit_ldadd)
%C%_sms2fr_static_LDADD = $(all_libctx) $(unit_ldadd)
%C%_sms2fr_dyn_LDADD = $(all_libctx) $(unit_ldadd)

%C%_TESTS =                                     \
  %D%/compose.chk                               \
  %D%/operators.chk                             \
  %D%/prod-eval.chk                             \
  %D%/sms2fr.chk

dist_TESTS += $(%C%_TESTS)

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
%D%/compose.log: %D%/compose
%D%/operators.log: %D%/operators
%D%/prod-eval.log: %D%/prod-eval libexec/vcsn-tools
%D%/sms2fr.log: %D%/sms2fr-static %D%/sms2fr-dyn %D%/sms2fr.py %D%/sms2fr-tests.py

.PHONY: check-demo
check-demo:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(%C%_TESTS)'
