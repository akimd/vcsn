# Not check_PROGRAMS, see below why.
EXTRA_PROGRAMS +=				\
  unit/determinize				\
  unit/eval					\
  unit/ladybird-b 				\
  unit/ladybird-z				\
  unit/ladybird-zmin				\
  unit/lift					\
  unit/poly					\
  unit/product					\
  unit/simplaw					\
  unit/simpleaut				\
  unit/transpose

unit_ladybird_b_SOURCES = unit/ladybird.cc
unit_ladybird_b_CPPFLAGS = $(AM_CPPFLAGS) -DW=b

unit_ladybird_z_SOURCES = unit/ladybird.cc
unit_ladybird_z_CPPFLAGS = $(AM_CPPFLAGS) -DW=z

unit_ladybird_zmin_SOURCES = unit/ladybird.cc
unit_ladybird_zmin_CPPFLAGS = $(AM_CPPFLAGS) -DW=z_min

unit_transpose_LDADD = $(top_builddir)/vcsn/librat.la

unit_TESTS =					\
  unit/determinize.chk				\
  unit/eval.chk					\
  unit/ladybird.chk				\
  unit/lift.chk					\
  unit/poly.chk					\
  unit/product.chk				\
  unit/simplaw.chk				\
  unit/simpleaut.chk				\
  unit/standard_of.chk				\
  unit/transpose.chk
dist_TESTS += $(unit_TESTS)

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
unit/aut_to_exp.log: rat/pprat
unit/determinize.log: unit/determinize
unit/eval.log: unit/eval
unit/ladybird.log: unit/ladybird-b unit/ladybird-z unit/ladybird-zmin
unit/lift.log: unit/lift
unit/poly.log: unit/poly
unit/product.log: unit/product
unit/simplaw.log: unit/simplaw
unit/simpleaut.log: unit/simpleaut
unit/standard_of.log: rat/pprat
unit/transpose.log: unit/transpose

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(unit_TESTS)'

