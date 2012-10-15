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

include lib/vcsn/local.mk

# char_b.
pkglib_LTLIBRARIES += lib/libchar_b_lal.la
lib_libchar_b_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_b_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_b_lal

pkglib_LTLIBRARIES += lib/libchar_b_law.la
lib_libchar_b_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_b_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_b_law

# char_br.
pkglib_LTLIBRARIES += lib/libchar_br_lal.la
lib_libchar_br_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_br_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_br_lal

pkglib_LTLIBRARIES += lib/libchar_br_law.la
lib_libchar_br_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_br_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_br_law

# char_z.
pkglib_LTLIBRARIES += lib/libchar_z_lae.la
lib_libchar_z_lae_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_z_lae_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_z_lae

pkglib_LTLIBRARIES += lib/libchar_z_lal.la
lib_libchar_z_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_z_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_z_lal

pkglib_LTLIBRARIES += lib/libchar_z_law.la
lib_libchar_z_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_z_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_z_law

# char_zr.
pkglib_LTLIBRARIES += lib/libchar_zr_lal.la
lib_libchar_zr_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_zr_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_zr_lal

pkglib_LTLIBRARIES += lib/libchar_zr_law.la
lib_libchar_zr_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_zr_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_zr_law

# char_zrr.
pkglib_LTLIBRARIES += lib/libchar_zrr_lal.la
lib_libchar_zrr_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_zrr_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_zrr_lal

pkglib_LTLIBRARIES += lib/libchar_zrr_law.la
lib_libchar_zrr_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_zrr_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_zrr_law

# char_zmin.
pkglib_LTLIBRARIES += lib/libchar_zmin_lal.la
lib_libchar_zmin_lal_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_zmin_lal_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_zmin_lal

pkglib_LTLIBRARIES += lib/libchar_zmin_law.la
lib_libchar_zmin_law_la_SOURCES = lib/ctx/libctx.cc
lib_libchar_zmin_law_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=char_zmin_law
