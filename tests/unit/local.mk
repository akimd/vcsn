noinst_PROGRAMS =				\
  unit/ladybird-b 				\
  unit/ladybird-z				\
  unit/poly					\
  unit/simplaw					\
  unit/simpleaut

unit_ladybird_b_SOURCES = unit/ladybird.cc
unit_ladybird_b_CPPFLAGS = $(AM_CPPFLAGS) -DW=b

unit_ladybird_z_SOURCES = unit/ladybird.cc
unit_ladybird_z_CPPFLAGS = $(AM_CPPFLAGS) -DW=z

unit_TESTS =					\
  unit/ladybird.chk				\
  unit/simplaw.chk

dist_TESTS += $(unit_TESTS)

.PHONY: check-unit
check-unit:
	$(MAKE) $(AM_MAKEFLAGS) check TESTS='$(unit_TESTS)'

