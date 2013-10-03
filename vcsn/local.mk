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

nobase_include_HEADERS =                        \
  vcsn/algos/accessible.hh                      \
  vcsn/algos/are-equivalent.hh                  \
  vcsn/algos/aut-to-exp.hh                      \
  vcsn/algos/complement.hh                      \
  vcsn/algos/complete.hh                        \
  vcsn/algos/concatenate.hh                     \
  vcsn/algos/constant-term.hh                   \
  vcsn/algos/copy.hh                            \
  vcsn/algos/derive.hh                          \
  vcsn/algos/determinize.hh                     \
  vcsn/algos/dot.hh                             \
  vcsn/algos/edit-automaton.hh                  \
  vcsn/algos/efsm.hh                            \
  vcsn/algos/enumerate.hh                       \
  vcsn/algos/eval.hh                            \
  vcsn/algos/fwd.hh                             \
  vcsn/algos/grail.hh                           \
  vcsn/algos/info.hh                            \
  vcsn/algos/is-ambiguous.hh                    \
  vcsn/algos/is-complete.hh                     \
  vcsn/algos/is-deterministic.hh                \
  vcsn/algos/is-eps-acyclic.hh                  \
  vcsn/algos/is-normalized.hh                   \
  vcsn/algos/is-proper.hh                       \
  vcsn/algos/is-valid.hh                        \
  vcsn/algos/left-mult.hh                       \
  vcsn/algos/lift.hh                            \
  vcsn/algos/make-context.hh                    \
  vcsn/algos/print.hh                           \
  vcsn/algos/product.hh                         \
  vcsn/algos/proper.hh                          \
  vcsn/algos/read-weight.hh                     \
  vcsn/algos/standard.hh                        \
  vcsn/algos/star.hh                            \
  vcsn/algos/sum.hh                             \
  vcsn/algos/thompson.hh                        \
  vcsn/algos/tikz.hh                            \
  vcsn/algos/transpose.hh                       \
  vcsn/algos/union.hh                           \
  vcsn/algos/universal.hh                       \
  vcsn/alphabets/char.hh                        \
  vcsn/alphabets/setalpha.hh                    \
  vcsn/core/crange.hh                           \
  vcsn/core/fwd.hh                              \
  vcsn/core/kind.hh                             \
  vcsn/core/mutable_automaton.hh                \
  vcsn/core/rat/fwd.hh                          \
  vcsn/core/rat/info.hh                         \
  vcsn/core/rat/info.hxx                        \
  vcsn/core/rat/printer.hh                      \
  vcsn/core/rat/printer.hxx                     \
  vcsn/core/rat/ratexp.hh                       \
  vcsn/core/rat/ratexp.hxx                      \
  vcsn/core/rat/ratexpset.hh                    \
  vcsn/core/rat/ratexpset.hxx                   \
  vcsn/core/rat/transpose.hh                    \
  vcsn/core/rat/visitor.hh                      \
  vcsn/core/rat/visitor.hxx                     \
  vcsn/core/transition.hh                       \
  vcsn/ctx/ctx.hh                               \
  vcsn/ctx/fwd.hh                               \
  vcsn/ctx/instantiate.hh                       \
  vcsn/ctx/lal_char.hh                          \
  vcsn/ctx/lal_char_b.hh                        \
  vcsn/ctx/lal_char_br.hh                       \
  vcsn/ctx/lal_char_q.hh                        \
  vcsn/ctx/lal_char_r.hh                        \
  vcsn/ctx/lal_char_z.hh                        \
  vcsn/ctx/lal_char_zmin.hh                     \
  vcsn/ctx/lal_char_zr.hh                       \
  vcsn/ctx/lal_char_zrr.hh                      \
  vcsn/ctx/lan_char.hh                          \
  vcsn/ctx/lan_char_b.hh                        \
  vcsn/ctx/lan_char_r.hh                        \
  vcsn/ctx/lan_char_z.hh                        \
  vcsn/ctx/lan_char_zr.hh                       \
  vcsn/ctx/lao_br.hh                            \
  vcsn/ctx/lao_z.hh                             \
  vcsn/ctx/law_char.hh                          \
  vcsn/ctx/law_char_b.hh                        \
  vcsn/ctx/law_char_br.hh                       \
  vcsn/ctx/law_char_q.hh                        \
  vcsn/ctx/law_char_r.hh                        \
  vcsn/ctx/law_char_z.hh                        \
  vcsn/ctx/law_char_zmin.hh                     \
  vcsn/ctx/law_char_zr.hh                       \
  vcsn/ctx/law_char_zrr.hh                      \
  vcsn/dyn/algos.hh                             \
  vcsn/dyn/automaton.hh                         \
  vcsn/dyn/context.hh                           \
  vcsn/dyn/fwd.hh                               \
  vcsn/dyn/ratexp.hh                            \
  vcsn/dyn/ratexpset.hh                         \
  vcsn/dyn/ratexpset.hxx                        \
  vcsn/dyn/weight.hh                            \
  vcsn/dyn/weightset.hh                         \
  vcsn/empty.hh                                 \
  vcsn/factory/de_bruijn.hh                     \
  vcsn/factory/double-ring.hh                   \
  vcsn/factory/divkbaseb.hh                     \
  vcsn/factory/ladybird.hh                      \
  vcsn/factory/random.hh                        \
  vcsn/factory/u.hh                             \
  vcsn/labelset/genset-labelset.hh              \
  vcsn/labelset/letterset.hh                    \
  vcsn/labelset/nullableset.hh                  \
  vcsn/labelset/oneset.hh                       \
  vcsn/labelset/wordset.hh                      \
  vcsn/misc/attributes.hh                       \
  vcsn/misc/cast.hh                             \
  vcsn/misc/const_traits.hh                     \
  vcsn/misc/direction.hh                        \
  vcsn/misc/dynamic_bitset.hh                   \
  vcsn/misc/echo.hh                             \
  vcsn/misc/escape.hh                           \
  vcsn/misc/hash.hh                             \
  vcsn/misc/military_order.hh                   \
  vcsn/misc/random.hh                           \
  vcsn/misc/regex.hh                            \
  vcsn/misc/set.hh                              \
  vcsn/misc/set.hxx                             \
  vcsn/misc/star_status.hh                      \
  vcsn/misc/stream.hh                           \
  vcsn/misc/unordered_set.hh                    \
  vcsn/misc/xalloc.hh                           \
  vcsn/misc/xalloc.hxx                          \
  vcsn/weights/b.hh                             \
  vcsn/weights/entryset.hh                      \
  vcsn/weights/f2.hh                            \
  vcsn/weights/fwd.hh                           \
  vcsn/weights/polynomialset.hh                 \
  vcsn/weights/q.hh                             \
  vcsn/weights/r.hh                             \
  vcsn/weights/z.hh                             \
  vcsn/weights/zmin.hh
