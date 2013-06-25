# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012-2013 Vaucanson Group.
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
pkglib_LTLIBRARIES += lib/liblal_char_b.la
lib_liblal_char_b_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_b_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_b

pkglib_LTLIBRARIES += lib/liblan_char_b.la
lib_liblan_char_b_la_SOURCES = lib/ctx/libctx.cc
lib_liblan_char_b_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lan_char_b

pkglib_LTLIBRARIES += lib/liblaw_char_b.la
lib_liblaw_char_b_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_b_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_b

# char_br.
pkglib_LTLIBRARIES += lib/liblao_br.la
lib_liblao_br_la_SOURCES = lib/ctx/libctx.cc
lib_liblao_br_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lao_br

pkglib_LTLIBRARIES += lib/liblal_char_br.la
lib_liblal_char_br_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_br_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_br

pkglib_LTLIBRARIES += lib/liblaw_char_br.la
lib_liblaw_char_br_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_br_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_br

# char_r.
pkglib_LTLIBRARIES += lib/liblal_char_r.la
lib_liblal_char_r_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_r_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_r

pkglib_LTLIBRARIES += lib/liblaw_char_r.la
lib_liblaw_char_r_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_r_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_r

# char_z.
pkglib_LTLIBRARIES += lib/liblao_z.la
lib_liblao_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblao_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lao_z

pkglib_LTLIBRARIES += lib/liblal_char_z.la
lib_liblal_char_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_z

pkglib_LTLIBRARIES += lib/liblan_char_z.la
lib_liblan_char_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblan_char_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lan_char_z

pkglib_LTLIBRARIES += lib/liblaw_char_z.la
lib_liblaw_char_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_z

# char_zr.
pkglib_LTLIBRARIES += lib/liblal_char_zr.la
lib_liblal_char_zr_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_zr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_zr

pkglib_LTLIBRARIES += lib/liblaw_char_zr.la
lib_liblaw_char_zr_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_zr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_zr

# char_zrr.
pkglib_LTLIBRARIES += lib/liblal_char_zrr.la
lib_liblal_char_zrr_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_zrr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_zrr

pkglib_LTLIBRARIES += lib/liblaw_char_zrr.la
lib_liblaw_char_zrr_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_zrr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_zrr

# char_zmin.
pkglib_LTLIBRARIES += lib/liblal_char_zmin.la
lib_liblal_char_zmin_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_zmin_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_zmin

pkglib_LTLIBRARIES += lib/liblaw_char_zmin.la
lib_liblaw_char_zmin_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_zmin_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_zmin
