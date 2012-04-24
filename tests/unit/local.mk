# Not check_PROGRAMS, see below why.
EXTRA_PROGRAMS +=				\
  unit/eval					\
  unit/ladybird-b 				\
  unit/ladybird-z				\
  unit/ladybird-zmin				\
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
  unit/ladybird.chk				\
  unit/poly.chk					\
  unit/product.chk				\
  unit/simpleaut.chk				\
  unit/simplaw.chk				\
  unit/standard_of.chk
dist_TESTS += $(unit_TESTS)

# Instead of using check_PROGRAMS, use EXTRA_PROGRAMS, but spell out
# the dependencies, so that the test suite does not make useless
# compilations.
unit/eval.log: unit/eval
unit/ladybird.log: unit/ladybird-b unit/ladybird-z unit/ladybird-zmin
unit/poly.log: unit/poly
unit/product.log: unit/product
unit/simpleaut.log: unit/simpleaut
unit/simplaw.log: unit/simplaw
unit/standard_of.log: rat/pprat

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(unit_TESTS)'

