## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2012-2014 Vaucanson Group.
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

algo_headers =                                  \
  %D%/algos/accessible.hh                       \
  %D%/algos/are-equivalent.hh                   \
  %D%/algos/are-isomorphic.hh                   \
  %D%/algos/cerny.hh                            \
  %D%/algos/complement.hh                       \
  %D%/algos/complete.hh                         \
  %D%/algos/compose.hh                          \
  %D%/algos/conjunction.hh                      \
  %D%/algos/constant-term.hh                    \
  %D%/algos/constant.hh                         \
  %D%/algos/copy.hh                             \
  %D%/algos/de-bruijn.hh                        \
  %D%/algos/derivation.hh                       \
  %D%/algos/derived-term.hh                     \
  %D%/algos/determinize.hh                      \
  %D%/algos/distance.hh                         \
  %D%/algos/divide.hh                           \
  %D%/algos/divkbaseb.hh                        \
  %D%/algos/dot.hh                              \
  %D%/algos/double-ring.hh                      \
  %D%/algos/edit-automaton.hh                   \
  %D%/algos/efsm.hh                             \
  %D%/algos/epsilon-remover-distance.hh         \
  %D%/algos/epsilon-remover-separate.hh         \
  %D%/algos/epsilon-remover.hh                  \
  %D%/algos/eval.hh                             \
  %D%/algos/expand.hh                           \
  %D%/algos/filter.hh                           \
  %D%/algos/focus.hh                            \
  %D%/algos/fwd.hh                              \
  %D%/algos/grail.hh                            \
  %D%/algos/has-bounded-lag.hh                  \
  %D%/algos/has-twins-property.hh               \
  %D%/algos/identities.hh                       \
  %D%/algos/info.hh                             \
  %D%/algos/insplit.hh                          \
  %D%/algos/is-ambiguous.hh                     \
  %D%/algos/is-complete.hh                      \
  %D%/algos/is-deterministic.hh                 \
  %D%/algos/is-eps-acyclic.hh                   \
  %D%/algos/is-functional.hh                    \
  %D%/algos/is-partial-identity.hh              \
  %D%/algos/is-proper.hh                        \
  %D%/algos/is-synchronized.hh                  \
  %D%/algos/is-valid-expression.hh              \
  %D%/algos/is-valid.hh                         \
  %D%/algos/ladybird.hh                         \
  %D%/algos/left-mult.hh                        \
  %D%/algos/letterize.hh                        \
  %D%/algos/less-than.hh                        \
  %D%/algos/lift.hh                             \
  %D%/algos/make-context.hh                     \
  %D%/algos/minimize-brzozowski.hh              \
  %D%/algos/minimize-moore.hh                   \
  %D%/algos/minimize-signature.hh               \
  %D%/algos/minimize-weighted.hh                \
  %D%/algos/minimize.hh                         \
  %D%/algos/multiply.hh                         \
  %D%/algos/normalize.hh                        \
  %D%/algos/pair.hh                             \
  %D%/algos/prefix.hh                           \
  %D%/algos/print.hh                            \
  %D%/algos/proper.hh                           \
  %D%/algos/push-weights.hh                     \
  %D%/algos/quotient.hh                         \
  %D%/algos/random.hh                           \
  %D%/algos/read.hh                             \
  %D%/algos/reduce.hh                           \
  %D%/algos/scc.hh                              \
  %D%/algos/shortest.hh                         \
  %D%/algos/sort.hh                             \
  %D%/algos/split.hh                            \
  %D%/algos/standard.hh                         \
  %D%/algos/star-height.hh                      \
  %D%/algos/star-normal-form.hh                 \
  %D%/algos/star.hh                             \
  %D%/algos/strip.hh                            \
  %D%/algos/sum.hh                              \
  %D%/algos/synchronize.hh                      \
  %D%/algos/synchronizing-word.hh               \
  %D%/algos/thompson.hh                         \
  %D%/algos/tikz.hh                             \
  %D%/algos/to-expansion.hh                     \
  %D%/algos/to-expression.hh                    \
  %D%/algos/transpose.hh                        \
  %D%/algos/trie.hh                             \
  %D%/algos/u.hh                                \
  %D%/algos/union.hh                            \
  %D%/algos/universal.hh                        \
  %D%/algos/weight-series.hh                    \
  %D%/algos/zpc.hh

nobase_include_HEADERS =                        \
  $(algo_headers)				\
  %D%/alphabets/char.hh                         \
  %D%/alphabets/setalpha.hh                     \
  %D%/core/automaton-decorator.hh               \
  %D%/core/expression-automaton.hh              \
  %D%/core/fwd.hh                               \
  %D%/core/join-automata.hh                     \
  %D%/core/join.hh                              \
  %D%/core/kind.hh                              \
  %D%/core/mutable-automaton.hh                 \
  %D%/core/partition-automaton.hh               \
  %D%/core/permutation-automaton.hh             \
  %D%/core/rat/copy.hh                          \
  %D%/core/rat/expansionset.hh                  \
  %D%/core/rat/expression.hh                    \
  %D%/core/rat/expression.hxx                   \
  %D%/core/rat/expressionset.hh                 \
  %D%/core/rat/expressionset.hxx                \
  %D%/core/rat/fwd.hh                           \
  %D%/core/rat/hash.hh                          \
  %D%/core/rat/hash.hxx                         \
  %D%/core/rat/identities.hh                    \
  %D%/core/rat/info.hh                          \
  %D%/core/rat/info.hxx                         \
  %D%/core/rat/less.hh                          \
  %D%/core/rat/printer.hh                       \
  %D%/core/rat/printer.hxx                      \
  %D%/core/rat/size.hh                          \
  %D%/core/rat/size.hxx                         \
  %D%/core/rat/transpose.hh                     \
  %D%/core/rat/visitor.hh                       \
  %D%/core/rat/visitor.hxx                      \
  %D%/core/transition-map.hh                    \
  %D%/core/transition.hh                        \
  %D%/core/tuple-automaton.hh                   \
  %D%/ctx/context.hh                            \
  %D%/ctx/fwd.hh                                \
  %D%/ctx/instantiate.hh                        \
  %D%/ctx/lal_char.hh                           \
  %D%/ctx/lal_char_b.hh                         \
  %D%/ctx/lal_char_br.hh                        \
  %D%/ctx/lal_char_q.hh                         \
  %D%/ctx/lal_char_r.hh                         \
  %D%/ctx/lal_char_z.hh                         \
  %D%/ctx/lal_char_zr.hh                        \
  %D%/ctx/lal_char_zrr.hh                       \
  %D%/ctx/lan_char.hh                           \
  %D%/ctx/lan_char_b.hh                         \
  %D%/ctx/lan_char_r.hh                         \
  %D%/ctx/lan_char_z.hh                         \
  %D%/ctx/lan_char_zr.hh                        \
  %D%/ctx/lao_br.hh                             \
  %D%/ctx/lao_z.hh                              \
  %D%/ctx/law_char.hh                           \
  %D%/ctx/law_char_b.hh                         \
  %D%/ctx/law_char_br.hh                        \
  %D%/ctx/law_char_q.hh                         \
  %D%/ctx/law_char_r.hh                         \
  %D%/ctx/law_char_z.hh                         \
  %D%/ctx/law_char_zr.hh                        \
  %D%/ctx/law_char_zrr.hh                       \
  %D%/ctx/traits.hh                             \
  %D%/dyn/algos.hh                              \
  %D%/dyn/automaton.hh                          \
  %D%/dyn/context-parser.hh                     \
  %D%/dyn/context-printer.hh                    \
  %D%/dyn/context-visitor.hh                    \
  %D%/dyn/context.hh                            \
  %D%/dyn/expansion.hh                          \
  %D%/dyn/expression.hh                         \
  %D%/dyn/fwd.hh                                \
  %D%/dyn/label.hh                              \
  %D%/dyn/polynomial.hh                         \
  %D%/dyn/signature-printer.hh                  \
  %D%/dyn/translate.hh                          \
  %D%/dyn/type-ast.hh                           \
  %D%/dyn/weight.hh                             \
  %D%/fwd.hh                                    \
  %D%/labelset/fwd.hh                           \
  %D%/labelset/genset-labelset.hh               \
  %D%/labelset/labelset.hh                      \
  %D%/labelset/letterset.hh                     \
  %D%/labelset/nullableset.hh                   \
  %D%/labelset/oneset.hh                        \
  %D%/labelset/tupleset.hh                      \
  %D%/labelset/word-polynomialset.hh            \
  %D%/labelset/wordset.hh                       \
  %D%/misc/algorithm.hh                         \
  %D%/misc/attributes.hh                        \
  %D%/misc/builtins.hh                          \
  %D%/misc/cast.hh                              \
  %D%/misc/crange.hh                            \
  %D%/misc/cross.hh                             \
  %D%/misc/debug-level.hh                       \
  %D%/misc/direction.hh                         \
  %D%/misc/dynamic_bitset.hh                    \
  %D%/misc/empty.hh                             \
  %D%/misc/epsilon-profile.hh                   \
  %D%/misc/escape.hh                            \
  %D%/misc/export.hh                            \
  %D%/misc/file-library.hh                      \
  %D%/misc/file-library.hxx                     \
  %D%/misc/flex-lexer.hh                        \
  %D%/misc/functional.hh                        \
  %D%/misc/fwd.hh                               \
  %D%/misc/getargs.hh                           \
  %D%/misc/indent.hh                            \
  %D%/misc/initializer_list.hh                  \
  %D%/misc/iostream.hh                          \
  %D%/misc/map.hh                               \
  %D%/misc/math.hh                              \
  %D%/misc/memory.hh                            \
  %D%/misc/military-order.hh                    \
  %D%/misc/name.hh                              \
  %D%/misc/pair.hh                              \
  %D%/misc/path.hh                              \
  %D%/misc/path.hxx                             \
  %D%/misc/raise.hh                             \
  %D%/misc/random.hh                            \
  %D%/misc/regex.hh                             \
  %D%/misc/set.hh                               \
  %D%/misc/set.hxx                              \
  %D%/misc/show.hh                              \
  %D%/misc/signature.hh                         \
  %D%/misc/star-status.hh                       \
  %D%/misc/stream.hh                            \
  %D%/misc/symbol.hh                            \
  %D%/misc/tuple.hh                             \
  %D%/misc/type_traits.hh                       \
  %D%/misc/unordered_map.hh                     \
  %D%/misc/unordered_set.hh                     \
  %D%/misc/vector.hh                            \
  %D%/misc/wet.hh                               \
  %D%/misc/xalloc.hh                            \
  %D%/misc/xalloc.hxx                           \
  %D%/misc/zip-maps.hh                          \
  %D%/misc/zip.hh                               \
  %D%/weightset/b.hh                            \
  %D%/weightset/f2.hh                           \
  %D%/weightset/fwd.hh                          \
  %D%/weightset/log.hh                          \
  %D%/weightset/min-plus.hh                     \
  %D%/weightset/nmin.hh                         \
  %D%/weightset/polynomialset.hh                \
  %D%/weightset/q.hh                            \
  %D%/weightset/qmp.hh                          \
  %D%/weightset/r.hh                            \
  %D%/weightset/rmin.hh                         \
  %D%/weightset/weightset.hh                    \
  %D%/weightset/z.hh                            \
  %D%/weightset/zmin.hh

# Unfortunately Automake 1.14 does not generate this for us.
vcsn/config.hh: $(top_builddir)/lib/config.h
	cd $(top_builddir) && $(SHELL) ./config.status $@
BUILT_SOURCES += lib/config.h %D%/config.hh
DISTCLEANFILES += %D%/config.hh


## ----------------- ##
## Generated files.  ##
## ----------------- ##

# The generator.
registers_gen = build-aux/bin/registers-gen
EXTRA_DIST += $(registers_gen)
# Files generated by registers_gen.
from_registers_gen =				\
  %D%/dyn/registers.hh				\
  lib/vcsn/algos/algos.cc			\
  lib/vcsn/dyn/context-printer-header-algo.cc
CLEANFILES += %D%/dyn/registers.stamp $(from_registers_gen)
move_if_change = $(srcdir)/build-aux/bin/move-if-change --color --verbose
%D%/dyn/registers.stamp: $(registers_gen) $(algo_headers)
	$(AM_V_GEN)$(mkdir_p) $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(srcdir)/$(registers_gen)				\
	  --bridge lib/vcsn/dyn/context-printer-header-algo.cc.tmp	\
	  --output lib/vcsn/algos/algos.cc.tmp				\
	  --header %D%/dyn/registers.hh.tmp				\
	  -s $(srcdir)							\
	  $(algo_headers)
	$(AM_V_at)for f in $(from_registers_gen);	\
	do						\
	  $(move_if_change) $$f.tmp $$f || exit 1;	\
	done
	$(AM_V_at)mv -f $@.tmp $@

$(from_registers_gen): %D%/dyn/registers.stamp
	@if test ! -f $@; then			\
	  rm -f $<;				\
	  $(MAKE) $(AM_MAKEFLAGS) $<;		\
	fi

BUILT_SOURCES += $(nobase_nodist_include_HEADERS)
nobase_nodist_include_HEADERS =                 \
  %D%/config.hh                                 \
  %D%/dyn/registers.hh
