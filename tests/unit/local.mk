check_PROGRAMS +=				\
  unit/ladybird-b 				\
  unit/ladybird-z				\
  unit/ladybird-zmin				\
  unit/poly					\
  unit/simplaw					\
  unit/simpleaut

unit_ladybird_b_SOURCES = unit/ladybird.cc
unit_ladybird_b_CPPFLAGS = $(AM_CPPFLAGS) -DW=b

unit_ladybird_z_SOURCES = unit/ladybird.cc
unit_ladybird_z_CPPFLAGS = $(AM_CPPFLAGS) -DW=z

unit_ladybird_zmin_SOURCES = unit/ladybird.cc
unit_ladybird_zmin_CPPFLAGS = $(AM_CPPFLAGS) -DW=z_min

unit_TESTS =                                    \
  unit/ladybird.chk                             \
  unit/poly.chk                                 \
  unit/simpleaut.chk                            \
  unit/simplaw.chk                              \
  unit/standard_of.chk

unit/standard_of.log: rat/pprat

dist_TESTS += $(unit_TESTS)

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(unit_TESTS)'

