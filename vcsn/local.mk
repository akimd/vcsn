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

nobase_include_HEADERS =                        \
  %D%/algos/accessible.hh                       \
  %D%/algos/are-equivalent.hh                   \
  %D%/algos/aut-to-exp.hh                       \
  %D%/algos/complement.hh                       \
  %D%/algos/complete.hh                         \
  %D%/algos/concatenate.hh                      \
  %D%/algos/constant-term.hh                    \
  %D%/algos/copy.hh                             \
  %D%/algos/derivation.hh                       \
  %D%/algos/determinize.hh                      \
  %D%/algos/dot.hh                              \
  %D%/algos/edit-automaton.hh                   \
  %D%/algos/efsm.hh                             \
  %D%/algos/enumerate.hh                        \
  %D%/algos/eval.hh                             \
  %D%/algos/expand.hh                           \
  %D%/algos/fwd.hh                              \
  %D%/algos/grail.hh                            \
  %D%/algos/info.hh                             \
  %D%/algos/is-ambiguous.hh                     \
  %D%/algos/is-complete.hh                      \
  %D%/algos/is-deterministic.hh                 \
  %D%/algos/is-eps-acyclic.hh                   \
  %D%/algos/is-normalized.hh                    \
  %D%/algos/is-proper.hh                        \
  %D%/algos/is-valid.hh                         \
  %D%/algos/left-mult.hh                        \
  %D%/algos/lift.hh                             \
  %D%/algos/make-context.hh                     \
  %D%/algos/minimize.hh                         \
  %D%/algos/minimize-signature.hh               \
  %D%/algos/print.hh                            \
  %D%/algos/product.hh                          \
  %D%/algos/proper.hh                           \
  %D%/algos/quotient.hh                         \
  %D%/algos/read.hh                             \
  %D%/algos/split.hh                            \
  %D%/algos/standard.hh                         \
  %D%/algos/star.hh                             \
  %D%/algos/star-height.hh                      \
  %D%/algos/star-height.hxx                     \
  %D%/algos/star-normal-form.hh                 \
  %D%/algos/sum.hh                              \
  %D%/algos/thompson.hh                         \
  %D%/algos/tikz.hh                             \
  %D%/algos/transpose.hh                        \
  %D%/algos/union.hh                            \
  %D%/algos/universal.hh                        \
  %D%/alphabets/char.hh                         \
  %D%/alphabets/setalpha.hh                     \
  %D%/core/crange.hh                            \
  %D%/core/fwd.hh                               \
  %D%/core/kind.hh                              \
  %D%/core/mutable_automaton.hh                 \
  %D%/core/rat/copy.hh                          \
  %D%/core/rat/fwd.hh                           \
  %D%/core/rat/hash.hh                          \
  %D%/core/rat/hash.hxx                         \
  %D%/core/rat/info.hh                          \
  %D%/core/rat/info.hxx                         \
  %D%/core/rat/less-than.hh                     \
  %D%/core/rat/printer.hh                       \
  %D%/core/rat/printer.hxx                      \
  %D%/core/rat/ratexp.hh                        \
  %D%/core/rat/ratexp.hxx                       \
  %D%/core/rat/ratexpset.hh                     \
  %D%/core/rat/ratexpset.hxx                    \
  %D%/core/rat/size.hh                          \
  %D%/core/rat/size.hxx                         \
  %D%/core/rat/transpose.hh                     \
  %D%/core/rat/visitor.hh                       \
  %D%/core/rat/visitor.hxx                      \
  %D%/core/transition.hh                        \
  %D%/ctx/context.hh                            \
  %D%/ctx/fwd.hh                                \
  %D%/ctx/instantiate.hh                        \
  %D%/ctx/lal_char.hh                           \
  %D%/ctx/lal_char_b.hh                         \
  %D%/ctx/lal_char_br.hh                        \
  %D%/ctx/lal_char_q.hh                         \
  %D%/ctx/lal_char_r.hh                         \
  %D%/ctx/lal_char_z.hh                         \
  %D%/ctx/lal_char_zmin.hh                      \
  %D%/ctx/lal_char_zr.hh                        \
  %D%/ctx/lal_char_zrr.hh                       \
  %D%/ctx/lan_char.hh                           \
  %D%/ctx/lan_char_b.hh                         \
  %D%/ctx/lan_char_r.hh                         \
  %D%/ctx/lan_char_z.hh                         \
  %D%/ctx/lan_char_zr.hh                        \
  %D%/ctx/lao_br.hh                             \
  %D%/ctx/lao_z.hh                              \
  %D%/ctx/latww_char_b.hh                       \
  %D%/ctx/law_char.hh                           \
  %D%/ctx/law_char_b.hh                         \
  %D%/ctx/law_char_br.hh                        \
  %D%/ctx/law_char_q.hh                         \
  %D%/ctx/law_char_r.hh                         \
  %D%/ctx/law_char_z.hh                         \
  %D%/ctx/law_char_zmin.hh                      \
  %D%/ctx/law_char_zr.hh                        \
  %D%/ctx/law_char_zrr.hh                       \
  %D%/dyn/algos.hh                              \
  %D%/dyn/automaton.hh                          \
  %D%/dyn/context.hh                            \
  %D%/dyn/fwd.hh                                \
  %D%/dyn/polynomial.hh                         \
  %D%/dyn/ratexp.hh                             \
  %D%/dyn/ratexpset.hh                          \
  %D%/dyn/ratexpset.hxx                         \
  %D%/dyn/translate.hh                          \
  %D%/dyn/weight.hh                             \
  %D%/empty.hh                                  \
  %D%/factory/de-bruijn.hh                      \
  %D%/factory/double-ring.hh                    \
  %D%/factory/divkbaseb.hh                      \
  %D%/factory/ladybird.hh                       \
  %D%/factory/random.hh                         \
  %D%/factory/u.hh                              \
  %D%/labelset/fwd.hh                           \
  %D%/labelset/genset-labelset.hh               \
  %D%/labelset/letterset.hh                     \
  %D%/labelset/nullableset.hh                   \
  %D%/labelset/oneset.hh                        \
  %D%/labelset/tupleset.hh                      \
  %D%/labelset/wordset.hh                       \
  %D%/misc/attributes.hh                        \
  %D%/misc/cast.hh                              \
  %D%/misc/const_traits.hh                      \
  %D%/misc/direction.hh                         \
  %D%/misc/dynamic_bitset.hh                    \
  %D%/misc/echo.hh                              \
  %D%/misc/escape.hh                            \
  %D%/misc/export.hh                            \
  %D%/misc/file-library.hh                      \
  %D%/misc/file-library.hxx                     \
  %D%/misc/flex-lexer.hh                        \
  %D%/misc/flyweight.hh                         \
  %D%/misc/hash.hh                              \
  %D%/misc/indent.hh                            \
  %D%/misc/map.hh                               \
  %D%/misc/military-order.hh                    \
  %D%/misc/name.hh                              \
  %D%/misc/pair.hh                              \
  %D%/misc/random.hh                            \
  %D%/misc/path.hh                              \
  %D%/misc/path.hxx                             \
  %D%/misc/raise.hh                             \
  %D%/misc/regex.hh                             \
  %D%/misc/set.hh                               \
  %D%/misc/set.hxx                              \
  %D%/misc/star_status.hh                       \
  %D%/misc/stream.hh                            \
  %D%/misc/tuple.hh                             \
  %D%/misc/unordered_set.hh                     \
  %D%/misc/xalloc.hh                            \
  %D%/misc/xalloc.hxx                           \
  %D%/weights/b.hh                              \
  %D%/weights/f2.hh                             \
  %D%/weights/fwd.hh                            \
  %D%/weights/polynomialset.hh                  \
  %D%/weights/q.hh                              \
  %D%/weights/r.hh                              \
  %D%/weights/z.hh                              \
  %D%/weights/zmin.hh

nobase_nodist_include_HEADERS =                 \
  %D%/config.hh
