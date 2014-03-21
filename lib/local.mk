## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2012-2013 Vaucanson Group.
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

include lib/vcsn/local.mk

# Where the plugins are compiled.
CLEANDIRS += %D%/plugins

## ----------- ##
## Libraries.  ##
## ----------- ##

liblal_char_b    = lib/liblal_char_b.la
liblal_char_br   = lib/liblal_char_br.la
liblal_char_q    = lib/liblal_char_q.la
liblal_char_r    = lib/liblal_char_r.la
liblal_char_z    = lib/liblal_char_z.la
liblal_char_zmin = lib/liblal_char_zmin.la
liblal_char_zr   = lib/liblal_char_zr.la
liblal_char_zrr  = lib/liblal_char_zrr.la
liblan_char_b    = lib/liblan_char_b.la
liblan_char_r    = lib/liblan_char_r.la
liblan_char_z    = lib/liblan_char_z.la
liblan_char_zr   = lib/liblan_char_zr.la
liblao_br        = lib/liblao_br.la
liblao_z         = lib/liblao_z.la
liblaw_char_b    = lib/liblaw_char_b.la
liblaw_char_br   = lib/liblaw_char_br.la
liblaw_char_q    = lib/liblaw_char_q.la
liblaw_char_r    = lib/liblaw_char_r.la
liblaw_char_z    = lib/liblaw_char_z.la
liblaw_char_zmin = lib/liblaw_char_zmin.la
liblaw_char_zr   = lib/liblaw_char_zr.la
liblaw_char_zrr  = lib/liblaw_char_zrr.la

all_libctx =                                    \
  $(liblal_char_b)                              \
  $(liblal_char_br)                             \
  $(liblal_char_q)                              \
  $(liblal_char_r)                              \
  $(liblal_char_z)                              \
  $(liblal_char_zmin)                           \
  $(liblal_char_zr)                             \
  $(liblal_char_zrr)                            \
  $(liblan_char_b)                              \
  $(liblan_char_r)                              \
  $(liblan_char_z)                              \
  $(liblan_char_zr)                             \
  $(liblao_br)                                  \
  $(liblao_z)                                   \
  $(liblaw_char_b)                              \
  $(liblaw_char_br)                             \
  $(liblaw_char_q)                              \
  $(liblaw_char_r)                              \
  $(liblaw_char_z)                              \
  $(liblaw_char_zmin)                           \
  $(liblaw_char_zr)                             \
  $(liblaw_char_zrr)


pkglib_LTLIBRARIES += $(all_libctx)

# char_b.
lib_liblal_char_b_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_b_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_b

lib_liblan_char_b_la_SOURCES = lib/ctx/libctx.cc
lib_liblan_char_b_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lan_char_b

lib_liblaw_char_b_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_b_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_b

# char_br.
lib_liblao_br_la_SOURCES = lib/ctx/libctx.cc
lib_liblao_br_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lao_br

lib_liblal_char_br_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_br_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_br

lib_liblaw_char_br_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_br_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_br

# char_r.
lib_liblal_char_r_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_r_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_r

lib_liblan_char_r_la_SOURCES = lib/ctx/libctx.cc
lib_liblan_char_r_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lan_char_r

lib_liblaw_char_r_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_r_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_r

# char_q.
lib_liblal_char_q_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_q_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_q

lib_liblaw_char_q_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_q_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_q

# char_z.
lib_liblao_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblao_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lao_z

lib_liblal_char_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_z

lib_liblan_char_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblan_char_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lan_char_z

lib_liblaw_char_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_z

# char_zr.
lib_liblal_char_zr_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_zr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_zr

lib_liblan_char_zr_la_SOURCES = lib/ctx/libctx.cc
lib_liblan_char_zr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lan_char_zr

lib_liblaw_char_zr_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_zr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_zr

# char_zrr.
lib_liblal_char_zrr_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_zrr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_zrr

lib_liblaw_char_zrr_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_zrr_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_zrr

# char_zmin.
lib_liblal_char_zmin_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_zmin_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_zmin

lib_liblaw_char_zmin_la_SOURCES = lib/ctx/libctx.cc
lib_liblaw_char_zmin_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=law_char_zmin
