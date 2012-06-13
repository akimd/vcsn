# Not check_PROGRAMS, see below why.
EXTRA_PROGRAMS +=				\
  unit/eval					\
  unit/determinize				\
  unit/ladybird-b 				\
  unit/ladybird-z				\
  unit/ladybird-zmin				\
  unit/lift					\
  unit/poly					\
  unit/product					\
  unit/simplaw					\
  unit/simpleaut

unit_ladybird_b_SOURCES = unit/ladybird.cc
unit_ladybird_b_CPPFLAGS = $(AM_CPPFLAGS) -DW=b

unit_ladybird_z_SOURCES = unit/ladybird.cc
unit_ladybird_z_CPPFLAGS = $(AM_CPPFLAGS) -DW=z

unit_ladybird_zmin_SOURCES = unit/ladybird.cc
unit_ladybird_zmin_CPPFLAGS = $(AM_CPPFLAGS) -DW=z_min

unit_TESTS =					\
  unit/eval.chk					\
  unit/determinize.chk				\
  unit/ladybird.chk				\
  unit/lift.chk					\
  unit/poly.chk					\
  unit/product.chk				\
  unit/simplaw.chk				\
  unit/simpleaut.chk				\
  unit/standard_of.chk
dist_TESTS += $(unit_TESTS)

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
unit/eval.log: unit/eval
unit/determinize.log: unit/determinize
unit/ladybird.log: unit/ladybird-b unit/ladybird-z unit/ladybird-zmin
unit/lift.log: unit/lift
unit/poly.log: unit/poly
unit/product.log: unit/product
unit/simplaw.log: unit/simplaw
unit/simpleaut.log: unit/simpleaut
unit/standard_of.log: rat/pprat

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(unit_TESTS)'

