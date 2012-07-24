# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012 Vaucanson Group.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# The complete GNU General Public Licence Notice can be found as the
# `COPYING' file in the root directory.
#
# The Vaucanson Group consists of people listed in the `AUTHORS' file.

AM_CXXFLAGS += $(WARNING_CXXFLAGS)
AM_CPPFLAGS += -I$(top_srcdir)
include lib/vcsn/local.mk

pkglib_LTLIBRARIES += lib/libchar_b_lal.la
lib_libchar_b_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_b_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_b_lal

pkglib_LTLIBRARIES += lib/libchar_b_law.la
lib_libchar_b_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_b_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_b_law

pkglib_LTLIBRARIES += lib/libchar_z_lal.la
lib_libchar_z_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_z_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_z_lal

pkglib_LTLIBRARIES += lib/libchar_z_law.la
lib_libchar_z_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_z_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_z_law

pkglib_LTLIBRARIES += lib/libchar_zmin_lal.la
lib_libchar_zmin_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_zmin_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_zmin_lal

pkglib_LTLIBRARIES += lib/libchar_zmin_law.la
lib_libchar_zmin_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_zmin_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_zmin_law
