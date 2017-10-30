## Vcsn, a generic library for finite state machines.
## Copyright (C) 2012-2017 Vcsn Group.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## The complete GNU General Public Licence Notice can be found as the
## `COPYING' file in the root directory.
##
## The Vcsn Group consists of people listed in the `AUTHORS' file.

include lib/vcsn/local.mk

# Where the plugins are compiled.
CLEANDIRS += %D%/plugins

## ----------- ##
## Libraries.  ##
## ----------- ##

liblal_char_b    = lib/liblal_char_b.la
liblal_char_q    = lib/liblal_char_q.la
liblal_char_z    = lib/liblal_char_z.la

all_libctx =                                    \
  $(liblal_char_b)  $(liblal_char_q)  $(liblal_char_z)

pkglib_LTLIBRARIES += $(all_libctx)

# char_b.
lib_liblal_char_b_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_b_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_b

# char_q.
lib_liblal_char_q_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_q_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_q

# char_z.
lib_liblal_char_z_la_SOURCES = lib/ctx/libctx.cc
lib_liblal_char_z_la_CPPFLAGS = $(AM_CPPFLAGS) -DCTX=lal_char_z

# BUILT_SOURCES suffices for the "all" target, but it is not enough
# when running "make lib/liblal_char_b.la" for instance.  So we have
# to add vcsn/dyn/registries.hh as a dependency.  And we must hide this
# from Automake, so that it does not remove the corresponding
# compilation rule.
all_libctx_names =                              \
  liblal_char_b  liblal_char_q  liblal_char_z

$(patsubst %, lib/ctx/lib_%_la-libctx.lo, $(all_libctx_names)):	\
  vcsn/dyn/registries.hh
