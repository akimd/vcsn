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
liblan_char_z    = lib/liblan_char_z.la
liblan_char_r    = lib/liblan_char_r.la
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

libvcsn = lib/libvcsn.la

## ---------- ##
## Programs.  ##
## ---------- ##

vcsn_accessible       = bin/vcsn-accessible
vcsn_are_equivalent   = bin/vcsn-are-equivalent
vcsn_aut_to_exp       = bin/vcsn-aut-to-exp
vcsn_cat              = bin/vcsn-cat
vcsn_chain            = bin/vcsn-chain
vcsn_coaccessible     = bin/vcsn-coaccessible
vcsn_complement       = bin/vcsn-complement
vcsn_complete         = bin/vcsn-complete
vcsn_concatenate      = bin/vcsn-concatenate
vcsn_constant_term    = bin/vcsn-constant-term
vcsn_de_bruijn        = bin/vcsn-de-bruijn
vcsn_determinize      = bin/vcsn-determinize
vcsn_divkbaseb        = bin/vcsn-divkbaseb
vcsn_double_ring      = bin/vcsn-double-ring
vcsn_evaluate         = bin/vcsn-evaluate
vcsn_is_ambiguous     = bin/vcsn-is-ambiguous
vcsn_is_complete      = bin/vcsn-is-complete
vcsn_is_deterministic = bin/vcsn-is-deterministic
vcsn_is_empty         = bin/vcsn-is-empty
vcsn_is_normalized    = bin/vcsn-is-normalized
vcsn_is_proper        = bin/vcsn-is-proper
vcsn_is_standard      = bin/vcsn-is-standard
vcsn_is_trim          = bin/vcsn-is-trim
vcsn_is_useless       = bin/vcsn-is-useless
vcsn_is_valid         = bin/vcsn-is-valid
vcsn_ladybird         = bin/vcsn-ladybird
vcsn_left_mult        = bin/vcsn-left-mult
vcsn_lift             = bin/vcsn-lift
vcsn_power            = bin/vcsn-power
vcsn_product          = bin/vcsn-product
vcsn_proper           = bin/vcsn-proper
vcsn_right_mult       = bin/vcsn-right-mult
vcsn_shortest         = bin/vcsn-shortest
vcsn_standard         = bin/vcsn-standard
vcsn_star             = bin/vcsn-star
vcsn_sum              = bin/vcsn-sum
vcsn_thompson         = bin/vcsn-thompson
vcsn_transpose        = bin/vcsn-transpose
vcsn_trim             = bin/vcsn-trim
vcsn_u                = bin/vcsn-u
vcsn_union            = bin/vcsn-union
vcsn_universal        = bin/vcsn-universal

all_vcsn =                                      \
  $(vcsn_accessible)                            \
  $(vcsn_are_equivalent)                        \
  $(vcsn_aut_to_exp)                            \
  $(vcsn_cat)                                   \
  $(vcsn_chain)                                 \
  $(vcsn_coaccessible)                          \
  $(vcsn_complement)                            \
  $(vcsn_complete)                              \
  $(vcsn_concatenate)                           \
  $(vcsn_constant_term)                         \
  $(vcsn_de_bruijn)                             \
  $(vcsn_determinize)                           \
  $(vcsn_divkbaseb)                             \
  $(vcsn_double_ring)                           \
  $(vcsn_enumerate)                             \
  $(vcsn_evaluate)                              \
  $(vcsn_is_ambiguous)                          \
  $(vcsn_is_complete)                           \
  $(vcsn_is_deterministic)                      \
  $(vcsn_is_empty)                              \
  $(vcsn_is_normalized)                         \
  $(vcsn_is_proper)                             \
  $(vcsn_is_standard)                           \
  $(vcsn_is_trim)                               \
  $(vcsn_is_useless)                            \
  $(vcsn_is_valid)                              \
  $(vcsn_ladybird)                              \
  $(vcsn_left_mult)                             \
  $(vcsn_lift)                                  \
  $(vcsn_power)                                 \
  $(vcsn_product)                               \
  $(vcsn_proper)                                \
  $(vcsn_right_mult)                            \
  $(vcsn_shortest)                              \
  $(vcsn_standard)                              \
  $(vcsn_star)                                  \
  $(vcsn_sum)                                   \
  $(vcsn_thompson)                              \
  $(vcsn_transpose)                             \
  $(vcsn_trim)                                  \
  $(vcsn_u)                                     \
  $(vcsn_union)                                 \
  $(vcsn_universal)
