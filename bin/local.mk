# Vaucanson 2, a generic library for finite state machines.
# Copyright (C) 2012-2014 Vaucanson Group.
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
  %D%/efstcompile                               \
  %D%/efstdecompile                             \
  %D%/vcsn-notebook                             \
  %D%/vcsn-score

nodist_bin_SCRIPTS +=                           \
  %D%/vcsn

## ---------- ##
## Programs.  ##
## ---------- ##

all_vcsn =                                      \
  %D%/vcsn-accessible                           \
  %D%/vcsn-are-equivalent                       \
  %D%/vcsn-aut-to-exp                           \
  %D%/vcsn-cat                                  \
  %D%/vcsn-chain                                \
  %D%/vcsn-coaccessible                         \
  %D%/vcsn-complement                           \
  %D%/vcsn-complete                             \
  %D%/vcsn-concatenate                          \
  %D%/vcsn-constant-term                        \
  %D%/vcsn-de-bruijn                            \
  %D%/vcsn-derivation                           \
  %D%/vcsn-derived-term                         \
  %D%/vcsn-determinize                          \
  %D%/vcsn-difference                           \
  %D%/vcsn-divkbaseb                            \
  %D%/vcsn-double-ring                          \
  %D%/vcsn-eliminate-state                      \
  %D%/vcsn-enumerate                            \
  %D%/vcsn-evaluate                             \
  %D%/vcsn-expand                               \
  %D%/vcsn-infiltration                         \
  %D%/vcsn-is-ambiguous                         \
  %D%/vcsn-is-complete                          \
  %D%/vcsn-is-deterministic                     \
  %D%/vcsn-is-empty                             \
  %D%/vcsn-is-eps-acyclic                       \
  %D%/vcsn-is-normalized                        \
  %D%/vcsn-is-proper                            \
  %D%/vcsn-is-standard                          \
  %D%/vcsn-is-trim                              \
  %D%/vcsn-is-useless                           \
  %D%/vcsn-is-valid                             \
  %D%/vcsn-ladybird                             \
  %D%/vcsn-left-mult                            \
  %D%/vcsn-lift                                 \
  %D%/vcsn-minimize                             \
  %D%/vcsn-power                                \
  %D%/vcsn-product                              \
  %D%/vcsn-proper                               \
  %D%/vcsn-random                               \
  %D%/vcsn-right-mult                           \
  %D%/vcsn-shortest                             \
  %D%/vcsn-shuffle                              \
  %D%/vcsn-split                                \
  %D%/vcsn-standard                             \
  %D%/vcsn-star                                 \
  %D%/vcsn-star-normal-form                     \
  %D%/vcsn-sum                                  \
  %D%/vcsn-thompson                             \
  %D%/vcsn-transpose                            \
  %D%/vcsn-trim                                 \
  %D%/vcsn-u                                    \
  %D%/vcsn-union                                \
  %D%/vcsn-universal

bin_PROGRAMS += $(all_vcsn)
noinst_LTLIBRARIES += %D%/libtafkit.la
%C%_libtafkit_la_SOURCES = %D%/parse-args.hh %D%/parse-args.cc
%C%_libtafkit_la_LIBADD = $(all_libctx) $(libvcsn)

%C%_vcsn_accessible_LDADD       = %D%/libtafkit.la
%C%_vcsn_are_equivalent_LDADD   = %D%/libtafkit.la
%C%_vcsn_aut_to_exp_LDADD       = %D%/libtafkit.la
%C%_vcsn_cat_LDADD              = %D%/libtafkit.la
%C%_vcsn_chain_LDADD            = %D%/libtafkit.la
%C%_vcsn_coaccessible_LDADD     = %D%/libtafkit.la
%C%_vcsn_complement_LDADD       = %D%/libtafkit.la
%C%_vcsn_complete_LDADD         = %D%/libtafkit.la
%C%_vcsn_concatenate_LDADD      = %D%/libtafkit.la
%C%_vcsn_constant_term_LDADD    = %D%/libtafkit.la
%C%_vcsn_de_bruijn_LDADD        = %D%/libtafkit.la
%C%_vcsn_derivation_LDADD       = %D%/libtafkit.la
%C%_vcsn_derived_term_LDADD     = %D%/libtafkit.la
%C%_vcsn_determinize_LDADD      = %D%/libtafkit.la
%C%_vcsn_difference_LDADD       = %D%/libtafkit.la
%C%_vcsn_divkbaseb_LDADD        = %D%/libtafkit.la
%C%_vcsn_double_ring_LDADD      = %D%/libtafkit.la
%C%_vcsn_eliminate_state_LDADD  = %D%/libtafkit.la
%C%_vcsn_enumerate_LDADD        = %D%/libtafkit.la
%C%_vcsn_evaluate_LDADD         = %D%/libtafkit.la
%C%_vcsn_expand_LDADD           = %D%/libtafkit.la
%C%_vcsn_infiltration_LDADD     = %D%/libtafkit.la
%C%_vcsn_is_ambiguous_LDADD     = %D%/libtafkit.la
%C%_vcsn_is_complete_LDADD      = %D%/libtafkit.la
%C%_vcsn_is_deterministic_LDADD = %D%/libtafkit.la
%C%_vcsn_is_empty_LDADD         = %D%/libtafkit.la
%C%_vcsn_is_eps_acyclic_LDADD   = %D%/libtafkit.la
%C%_vcsn_is_normalized_LDADD    = %D%/libtafkit.la
%C%_vcsn_is_proper_LDADD        = %D%/libtafkit.la
%C%_vcsn_is_standard_LDADD      = %D%/libtafkit.la
%C%_vcsn_is_trim_LDADD          = %D%/libtafkit.la
%C%_vcsn_is_useless_LDADD       = %D%/libtafkit.la
%C%_vcsn_is_valid_LDADD         = %D%/libtafkit.la
%C%_vcsn_ladybird_LDADD         = %D%/libtafkit.la
%C%_vcsn_left_mult_LDADD        = %D%/libtafkit.la
%C%_vcsn_lift_LDADD             = %D%/libtafkit.la
%C%_vcsn_minimize_LDADD         = %D%/libtafkit.la
%C%_vcsn_power_LDADD            = %D%/libtafkit.la
%C%_vcsn_product_LDADD          = %D%/libtafkit.la
%C%_vcsn_proper_LDADD           = %D%/libtafkit.la
%C%_vcsn_random_LDADD           = %D%/libtafkit.la
%C%_vcsn_right_mult_LDADD       = %D%/libtafkit.la
%C%_vcsn_shortest_LDADD         = %D%/libtafkit.la
%C%_vcsn_shuffle_LDADD          = %D%/libtafkit.la
%C%_vcsn_split_LDADD            = %D%/libtafkit.la
%C%_vcsn_standard_LDADD         = %D%/libtafkit.la
%C%_vcsn_star_LDADD             = %D%/libtafkit.la
%C%_vcsn_star_normal_form_LDADD = %D%/libtafkit.la
%C%_vcsn_sum_LDADD              = %D%/libtafkit.la
%C%_vcsn_thompson_LDADD         = %D%/libtafkit.la
%C%_vcsn_transpose_LDADD        = %D%/libtafkit.la
%C%_vcsn_trim_LDADD             = %D%/libtafkit.la
%C%_vcsn_u_LDADD                = %D%/libtafkit.la
%C%_vcsn_union_LDADD            = %D%/libtafkit.la
%C%_vcsn_universal_LDADD        = %D%/libtafkit.la
