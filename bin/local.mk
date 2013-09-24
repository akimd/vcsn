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

dist_bin_SCRIPTS +=                             \
  bin/efstcompile                               \
  bin/efstdecompile                             \
  bin/vcsn

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
vcsn_derive           = bin/vcsn-derive
vcsn_determinize      = bin/vcsn-determinize
vcsn_divkbaseb        = bin/vcsn-divkbaseb
vcsn_double_ring      = bin/vcsn-double-ring
vcsn_evaluate         = bin/vcsn-evaluate
vcsn_enumerate        = bin/vcsn-enumerate
vcsn_is_ambiguous     = bin/vcsn-is-ambiguous
vcsn_is_complete      = bin/vcsn-is-complete
vcsn_is_deterministic = bin/vcsn-is-deterministic
vcsn_is_empty         = bin/vcsn-is-empty
vcsn_is_eps_acyclic   = bin/vcsn-is-eps-acyclic
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
  $(vcsn_derive)                                \
  $(vcsn_determinize)                           \
  $(vcsn_divkbaseb)                             \
  $(vcsn_double_ring)                           \
  $(vcsn_enumerate)                             \
  $(vcsn_evaluate)                              \
  $(vcsn_is_ambiguous)                          \
  $(vcsn_is_complete)                           \
  $(vcsn_is_deterministic)                      \
  $(vcsn_is_empty)                              \
  $(vcsn_is_eps_acyclic)                        \
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

bin_PROGRAMS += $(all_vcsn)
noinst_LTLIBRARIES += bin/libtafkit.la
bin_libtafkit_la_SOURCES = bin/parse-args.hh bin/parse-args.cc
bin_libtafkit_la_LIBADD = $(all_libctx) $(libvcsn)

bin_vcsn_accessible_LDADD       = bin/libtafkit.la
bin_vcsn_are_equivalent_LDADD   = bin/libtafkit.la
bin_vcsn_aut_to_exp_LDADD       = bin/libtafkit.la
bin_vcsn_cat_LDADD              = bin/libtafkit.la
bin_vcsn_chain_LDADD            = bin/libtafkit.la
bin_vcsn_coaccessible_LDADD     = bin/libtafkit.la
bin_vcsn_complement_LDADD       = bin/libtafkit.la
bin_vcsn_complete_LDADD         = bin/libtafkit.la
bin_vcsn_concatenate_LDADD      = bin/libtafkit.la
bin_vcsn_constant_term_LDADD    = bin/libtafkit.la
bin_vcsn_de_bruijn_LDADD        = bin/libtafkit.la
bin_vcsn_derive_LDADD           = bin/libtafkit.la
bin_vcsn_determinize_LDADD      = bin/libtafkit.la
bin_vcsn_divkbaseb_LDADD        = bin/libtafkit.la
bin_vcsn_double_ring_LDADD      = bin/libtafkit.la
bin_vcsn_enumerate_LDADD        = bin/libtafkit.la
bin_vcsn_evaluate_LDADD         = bin/libtafkit.la
bin_vcsn_is_ambiguous_LDADD     = bin/libtafkit.la
bin_vcsn_is_complete_LDADD      = bin/libtafkit.la
bin_vcsn_is_deterministic_LDADD = bin/libtafkit.la
bin_vcsn_is_empty_LDADD         = bin/libtafkit.la
bin_vcsn_is_eps_acyclic_LDADD   = bin/libtafkit.la
bin_vcsn_is_normalized_LDADD    = bin/libtafkit.la
bin_vcsn_is_proper_LDADD        = bin/libtafkit.la
bin_vcsn_is_standard_LDADD      = bin/libtafkit.la
bin_vcsn_is_trim_LDADD          = bin/libtafkit.la
bin_vcsn_is_useless_LDADD       = bin/libtafkit.la
bin_vcsn_is_valid_LDADD         = bin/libtafkit.la
bin_vcsn_ladybird_LDADD         = bin/libtafkit.la
bin_vcsn_left_mult_LDADD        = bin/libtafkit.la
bin_vcsn_lift_LDADD             = bin/libtafkit.la
bin_vcsn_power_LDADD            = bin/libtafkit.la
bin_vcsn_product_LDADD          = bin/libtafkit.la
bin_vcsn_proper_LDADD           = bin/libtafkit.la
bin_vcsn_right_mult_LDADD       = bin/libtafkit.la
bin_vcsn_shortest_LDADD         = bin/libtafkit.la
bin_vcsn_standard_LDADD         = bin/libtafkit.la
bin_vcsn_star_LDADD             = bin/libtafkit.la
bin_vcsn_sum_LDADD              = bin/libtafkit.la
bin_vcsn_thompson_LDADD         = bin/libtafkit.la
bin_vcsn_transpose_LDADD        = bin/libtafkit.la
bin_vcsn_trim_LDADD             = bin/libtafkit.la
bin_vcsn_u_LDADD                = bin/libtafkit.la
bin_vcsn_union_LDADD            = bin/libtafkit.la
bin_vcsn_universal_LDADD        = bin/libtafkit.la
