## Vcsn, a generic library for finite state machines.
## Copyright (C) 2012-2018 Vcsn Group.
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

algo_headers =                                  \
  %D%/algos/a-star.hh                           \
  %D%/algos/accessible.hh                       \
  %D%/algos/add.hh                              \
  %D%/algos/are-equivalent.hh                   \
  %D%/algos/are-isomorphic.hh                   \
  %D%/algos/bellman-ford.hh                     \
  %D%/algos/cerny.hh                            \
  %D%/algos/compare-automaton.hh                \
  %D%/algos/compare.hh                          \
  %D%/algos/complement.hh                       \
  %D%/algos/complete.hh                         \
  %D%/algos/compose-expression.hh               \
  %D%/algos/compose.hh                          \
  %D%/algos/conjugate.hh                        \
  %D%/algos/conjunction-expression.hh           \
  %D%/algos/conjunction.hh                      \
  %D%/algos/constant-term.hh                    \
  %D%/algos/constant.hh                         \
  %D%/algos/copy.hh                             \
  %D%/algos/daut.hh                             \
  %D%/algos/de-bruijn.hh                        \
  %D%/algos/derivation.hh                       \
  %D%/algos/derived-term.hh                     \
  %D%/algos/detail/printer.hh                   \
  %D%/algos/determinize-expansion.hh            \
  %D%/algos/determinize.hh                      \
  %D%/algos/dijkstra-node.hh                    \
  %D%/algos/dijkstra.hh                         \
  %D%/algos/distance.hh                         \
  %D%/algos/divide.hh                           \
  %D%/algos/divkbaseb.hh                        \
  %D%/algos/dot.hh                              \
  %D%/algos/double-ring.hh                      \
  %D%/algos/edit-automaton.hh                   \
  %D%/algos/efsm.hh                             \
  %D%/algos/eppstein.hh                         \
  %D%/algos/epsilon-remover-distance.hh         \
  %D%/algos/epsilon-remover-lazy.hh             \
  %D%/algos/epsilon-remover-separate.hh         \
  %D%/algos/epsilon-remover.hh                  \
  %D%/algos/evaluate.hh                         \
  %D%/algos/expand.hh                           \
  %D%/algos/filter.hh                           \
  %D%/algos/focus.hh                            \
  %D%/algos/fwd.hh                              \
  %D%/algos/grail.hh                            \
  %D%/algos/guess-automaton-format.hh           \
  %D%/algos/has-bounded-lag.hh                  \
  %D%/algos/has-lightening-cycle.hh             \
  %D%/algos/has-twins-property.hh               \
  %D%/algos/identities-of.hh                    \
  %D%/algos/implicit-path.hh                    \
  %D%/algos/inductive.hh                        \
  %D%/algos/infiltrate-expression.hh            \
  %D%/algos/info.hh                             \
  %D%/algos/insplit.hh                          \
  %D%/algos/is-acyclic.hh                       \
  %D%/algos/is-ambiguous.hh                     \
  %D%/algos/is-complete.hh                      \
  %D%/algos/is-deterministic.hh                 \
  %D%/algos/is-free-boolean.hh                  \
  %D%/algos/is-functional.hh                    \
  %D%/algos/is-partial-identity.hh              \
  %D%/algos/is-proper.hh                        \
  %D%/algos/is-synchronized.hh                  \
  %D%/algos/is-valid-expression.hh              \
  %D%/algos/is-valid.hh                         \
  %D%/algos/k-lightest-path.hh                  \
  %D%/algos/ladybird.hh                         \
  %D%/algos/letterize.hh                        \
  %D%/algos/levenshtein.hh                      \
  %D%/algos/lift.hh                             \
  %D%/algos/lightest-automaton.hh               \
  %D%/algos/lightest-path.hh                    \
  %D%/algos/lightest.hh                         \
  %D%/algos/make-context.hh                     \
  %D%/algos/minimize-brzozowski.hh              \
  %D%/algos/minimize-hopcroft.hh                \
  %D%/algos/minimize-moore.hh                   \
  %D%/algos/minimize-signature.hh               \
  %D%/algos/minimize-weighted.hh                \
  %D%/algos/minimize.hh                         \
  %D%/algos/multiply.hh                         \
  %D%/algos/name.hh                             \
  %D%/algos/normalize-expansion.hh              \
  %D%/algos/normalize.hh                        \
  %D%/algos/num-tapes.hh                        \
  %D%/algos/pair.hh                             \
  %D%/algos/partial-identity-expression.hh      \
  %D%/algos/partial-identity.hh                 \
  %D%/algos/path.hh                             \
  %D%/algos/prefix.hh                           \
  %D%/algos/print.hh                            \
  %D%/algos/project-automaton.hh                \
  %D%/algos/project.hh                          \
  %D%/algos/proper.hh                           \
  %D%/algos/push-weights.hh                     \
  %D%/algos/quotient.hh                         \
  %D%/algos/quotkbaseb.hh                       \
  %D%/algos/random-automaton.hh                 \
  %D%/algos/random-expression.hh                \
  %D%/algos/random-weight.hh                    \
  %D%/algos/read-automaton.hh                   \
  %D%/algos/read.hh                             \
  %D%/algos/reduce.hh                           \
  %D%/algos/scc.hh                              \
  %D%/algos/shortest-path-tree.hh               \
  %D%/algos/shortest.hh                         \
  %D%/algos/shuffle-expression.hh               \
  %D%/algos/sort.hh                             \
  %D%/algos/split.hh                            \
  %D%/algos/standard.hh                         \
  %D%/algos/star-height.hh                      \
  %D%/algos/star-normal-form.hh                 \
  %D%/algos/star.hh                             \
  %D%/algos/strip.hh                            \
  %D%/algos/synchronize.hh                      \
  %D%/algos/synchronizing-word.hh               \
  %D%/algos/tags.hh                             \
  %D%/algos/thompson.hh                         \
  %D%/algos/tikz.hh                             \
  %D%/algos/to-expansion.hh                     \
  %D%/algos/to-expression-expansion.hh          \
  %D%/algos/to-expression.hh                    \
  %D%/algos/to-spontaneous.hh                   \
  %D%/algos/transpose.hh                        \
  %D%/algos/trie.hh                             \
  %D%/algos/tuple-automaton.hh                  \
  %D%/algos/tuple.hh                            \
  %D%/algos/u.hh                                \
  %D%/algos/universal.hh                        \
  %D%/algos/weight-series.hh                    \
  %D%/algos/weight.hh                           \
  %D%/algos/zpc.hh

nobase_include_HEADERS =                        \
  $(algo_headers)                               \
  %D%/alphabets/char.hh                         \
  %D%/alphabets/setalpha.hh                     \
  %D%/alphabets/string.hh                       \
  %D%/concepts/automaton.hh                     \
  %D%/core/automaton-decorator.hh               \
  %D%/core/automaton.hh                         \
  %D%/core/automatonset.hh                      \
  %D%/core/expression-automaton.hh              \
  %D%/core/fwd.hh                               \
  %D%/core/join-automata.hh                     \
  %D%/core/join.hh                              \
  %D%/core/kind.hh                              \
  %D%/core/lazy-tuple-automaton.hh              \
  %D%/core/mutable-automaton.hh                 \
  %D%/core/name-automaton.hh                    \
  %D%/core/partition-automaton.hh               \
  %D%/core/permutation-automaton.hh             \
  %D%/core/polystate-automaton.hh               \
  %D%/core/rat/compare.hh                       \
  %D%/core/rat/copy.hh                          \
  %D%/core/rat/dot.hh                           \
  %D%/core/rat/expansionset.hh                  \
  %D%/core/rat/expression.hh                    \
  %D%/core/rat/expression.hxx                   \
  %D%/core/rat/expressionset.hh                 \
  %D%/core/rat/expressionset.hxx                \
  %D%/core/rat/fwd.hh                           \
  %D%/core/rat/hash.hh                          \
  %D%/core/rat/identities.hh                    \
  %D%/core/rat/info.hh                          \
  %D%/core/rat/printer.hh                       \
  %D%/core/rat/printer.hxx                      \
  %D%/core/rat/project.hh                       \
  %D%/core/rat/size.hh                          \
  %D%/core/rat/transpose.hh                     \
  %D%/core/rat/visitor.hh                       \
  %D%/core/rat/visitor.hxx                      \
  %D%/core/state-bimap.hh                       \
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
  %D%/ctx/project-context.hh                    \
  %D%/ctx/traits.hh                             \
  %D%/dyn/algos.hh                              \
  %D%/dyn/algos.hxx                             \
  %D%/dyn/automaton.hh                          \
  %D%/dyn/cast.hh                               \
  %D%/dyn/context.hh                            \
  %D%/dyn/fwd.hh                                \
  %D%/dyn/name.hh                               \
  %D%/dyn/types.hh                              \
  %D%/dyn/value.hh                              \
  %D%/fwd.hh                                    \
  %D%/labelset/fwd.hh                           \
  %D%/labelset/genset-labelset.hh               \
  %D%/labelset/labelset.hh                      \
  %D%/labelset/letterset.hh                     \
  %D%/labelset/nullableset.hh                   \
  %D%/labelset/oneset.hh                        \
  %D%/labelset/stateset.hh                      \
  %D%/labelset/tupleset.hh                      \
  %D%/labelset/word-polynomialset.hh            \
  %D%/labelset/wordset.hh                       \
  %D%/misc/algorithm.hh                         \
  %D%/misc/attributes.hh                        \
  %D%/misc/bimap.hh                             \
  %D%/misc/builtins.hh                          \
  %D%/misc/cast.hh                              \
  %D%/misc/configuration.hh                     \
  %D%/misc/crange.hh                            \
  %D%/misc/cross.hh                             \
  %D%/misc/debug-level.hh                       \
  %D%/misc/deque.hh                             \
  %D%/misc/direction.hh                         \
  %D%/misc/dynamic_bitset.hh                    \
  %D%/misc/empty.hh                             \
  %D%/misc/epsilon-profile.hh                   \
  %D%/misc/escape.hh                            \
  %D%/misc/export.hh                            \
  %D%/misc/fibonacci_heap.hh                    \
  %D%/misc/file-library.hh                      \
  %D%/misc/file-library.hxx                     \
  %D%/misc/filter.hh                            \
  %D%/misc/format.hh                            \
  %D%/misc/functional.hh                        \
  %D%/misc/fwd.hh                               \
  %D%/misc/getargs.hh                           \
  %D%/misc/indent.hh                            \
  %D%/misc/index.hh                             \
  %D%/misc/initializer_list.hh                  \
  %D%/misc/iostream.hh                          \
  %D%/misc/irange.hh                            \
  %D%/misc/location.hh                          \
  %D%/misc/map.hh                               \
  %D%/misc/math.hh                              \
  %D%/misc/memory.hh                            \
  %D%/misc/military-order.hh                    \
  %D%/misc/pair.hh                              \
  %D%/misc/position.hh                          \
  %D%/misc/queue.hh                             \
  %D%/misc/raise.hh                             \
  %D%/misc/random.hh                            \
  %D%/misc/regex.hh                             \
  %D%/misc/set.hh                               \
  %D%/misc/set.hxx                              \
  %D%/misc/show.hh                              \
  %D%/misc/signature.hh                         \
  %D%/misc/sparse-map.hh                        \
  %D%/misc/sparse-set.hh                        \
  %D%/misc/star-status.hh                       \
  %D%/misc/static-if.hh                         \
  %D%/misc/stream.hh                            \
  %D%/misc/symbol.hh                            \
  %D%/misc/to-string.hh                         \
  %D%/misc/to.hh                                \
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


## ----------------------------- ##
## Generated files: registries.  ##
## ----------------------------- ##

# The registries generator.
registries_gen = build-aux/bin/registries-gen
dist_noinst_python += $(registries_gen)
# Files generated by registries_gen.
from_registries_gen =				\
  %D%/dyn/registries.hh				\
  lib/vcsn/algos/algos.cc			\
  lib/vcsn/dyn/context-printer-header-algo.cc
CLEANFILES += %D%/dyn/registries.stamp $(from_registries_gen)
%D%/dyn/registries.stamp: $(registries_gen) $(algo_headers) $(algo_implems)
	$(AM_V_GEN)$(mkdir_p) $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(PYTHON) $(srcdir)/$(registries_gen)			\
	  --bridge lib/vcsn/dyn/context-printer-header-algo.cc.tmp	\
	  --output lib/vcsn/algos/algos.cc.tmp				\
	  --header %D%/dyn/registries.hh.tmp				\
	  --srcdir $(srcdir)						\
	  --headers $(algo_headers)					\
	  --implems $(algo_implems)
	$(AM_V_at)for f in $(from_registries_gen);	\
	do						\
	  $(move_if_change) $$f.tmp $$f || exit 1;	\
	done
	$(AM_V_at)mv -f $@.tmp $@

$(from_registries_gen): %D%/dyn/registries.stamp
	@if test ! -f $@; then			\
	  rm -f $<;				\
	  $(MAKE) $(AM_MAKEFLAGS) $<;		\
	fi

BUILT_SOURCES += $(nobase_nodist_include_HEADERS)
nobase_nodist_include_HEADERS =                 \
  %D%/config.hh                                 \
  %D%/dyn/registries.hh


## ----------------------------- ##
## Generated files: algos.json.  ##
## ----------------------------- ##

# The algos.json generator.
algos_json_gen = build-aux/bin/dyn-json
dist_noinst_python += $(algos_json_gen)
# Files generated by algos_json_gen.
from_algos_json_gen =	\
	%D%/dyn/algos.json
CLEANFILES += %D%/dyn/algos.json.stamp $(from_algos_json_gen)
%D%/dyn/algos.json.stamp: $(algos_json_gen) vcsn/dyn/algos.hh
	$(AM_V_GEN)$(mkdir_p) $(@D)
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(PYTHON) $(srcdir)/$(algos_json_gen)                     \
	  --output %D%/dyn/algos.json.tmp                       \
	  $(srcdir)/%D%/dyn/algos.hh
	$(AM_V_at)for f in $(from_algos_json_gen);          \
	do                                              \
	  $(move_if_change) $$f.tmp $$f || exit 1;      \
	done
	$(AM_V_at)mv -f $@.tmp $@

$(from_algos_json_gen): %D%/dyn/algos.json.stamp
	@if test ! -f $@; then                  \
	  rm -f $<;                             \
	  $(MAKE) $(AM_MAKEFLAGS) $<;           \
	fi

## ----------------------- ##
## Generated files: odyn.  ##
## ----------------------- ##

odyn_gen = build-aux/bin/odyn-gen
dist_noinst_python += $(odyn_gen)
# Files generated by odyn_gen.
from_odyn_gen =				\
  %D%/odyn/odyn.hh lib/vcsn/odyn/odyn.cc
CLEANFILES += %D%/odyn/odyn.stamp $(from_odyn_gen)
%D%/odyn/odyn.stamp: $(odyn_gen) %D%/dyn/algos.json
	$(AM_V_GEN)$(mkdir_p) $(@D) lib/vcsn/odyn
	$(AM_V_at)rm -f $@ $@.tmp
	$(AM_V_at)echo '$@ rebuilt because of: $?' >$@.tmp
	$(AM_V_at)$(PYTHON) $(srcdir)/$(odyn_gen)	\
	  --header %D%/odyn/odyn.hh.tmp			\
	  --output lib/vcsn/odyn/odyn.cc.tmp		\
	  %D%/dyn/algos.json
	$(AM_V_at)for f in $(from_odyn_gen);		\
	do						\
	  $(move_if_change) $$f.tmp $$f || exit 1;	\
	done
	$(AM_V_at)mv -f $@.tmp $@

$(from_odyn_gen): %D%/odyn/odyn.stamp
	@if test ! -f $@; then			\
	  rm -f $<;				\
	  $(MAKE) $(AM_MAKEFLAGS) $<;		\
	fi

nobase_nodist_include_HEADERS +=                \
  %D%/odyn/odyn.hh
