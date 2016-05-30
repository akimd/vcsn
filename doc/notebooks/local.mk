## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2014-2016 Vaucanson Group.
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

notebooksdir = $(htmldir)/notebooks

dist_notebooks_DATA =                           \
  %D%/!Read-me-first.ipynb                      \
  %D%/Algorithms.ipynb                          \
  %D%/Automata.ipynb                            \
  %D%/C++-Library.ipynb                         \
  %D%/CIAA-2016.ipynb                           \
  %D%/Contexts.ipynb                            \
  %D%/Executables.ipynb                         \
  %D%/Expressions.ipynb                         \
  %D%/Glossary.ipynb                            \
  %D%/Hacking.ipynb                             \
  %D%/ICTAC-2016.ipynb                          \
  %D%/References.ipynb                          \
  %D%/Spell-checker.ipynb                       \
  %D%/Stackoverflow.ipynb                       \
  %D%/Transducers.ipynb                         \
  %D%/Trouble-Shooting.ipynb                    \
  %D%/automaton.accessible.ipynb                \
  %D%/automaton.add.ipynb                       \
  %D%/automaton.ambiguous_word.ipynb            \
  %D%/automaton.coaccessible.ipynb              \
  %D%/automaton.codeterminize.ipynb             \
  %D%/automaton.cominimize.ipynb                \
  %D%/automaton.complement.ipynb                \
  %D%/automaton.complete.ipynb                  \
  %D%/automaton.compose.ipynb                   \
  %D%/automaton.conjugate.ipynb                 \
  %D%/automaton.conjunction.ipynb               \
  %D%/automaton.context.ipynb                   \
  %D%/automaton.costandard.ipynb                \
  %D%/automaton.delay_automaton.ipynb           \
  %D%/automaton.determinize.ipynb               \
  %D%/automaton.difference.ipynb                \
  %D%/automaton.eliminate_state.ipynb           \
  %D%/automaton.eval.ipynb                      \
  %D%/automaton.expression.ipynb                \
  %D%/automaton.factor.ipynb                    \
  %D%/automaton.filter.ipynb                    \
  %D%/automaton.has_bounded_lag.ipynb           \
  %D%/automaton.has_lightening_cycle.ipynb      \
  %D%/automaton.has_twins_property.ipynb        \
  %D%/automaton.infiltrate.ipynb                \
  %D%/automaton.info.ipynb                      \
  %D%/automaton.insplit.ipynb                   \
  %D%/automaton.is_accessible.ipynb             \
  %D%/automaton.is_ambiguous.ipynb              \
  %D%/automaton.is_coaccessible.ipynb           \
  %D%/automaton.is_codeterministic.ipynb        \
  %D%/automaton.is_complete.ipynb               \
  %D%/automaton.is_costandard.ipynb             \
  %D%/automaton.is_cycle_ambiguous.ipynb        \
  %D%/automaton.is_deterministic.ipynb          \
  %D%/automaton.is_empty.ipynb                  \
  %D%/automaton.is_equivalent.ipynb             \
  %D%/automaton.is_functional.ipynb             \
  %D%/automaton.is_isomorphic.ipynb             \
  %D%/automaton.is_letterized.ipynb             \
  %D%/automaton.is_partial_identity.ipynb       \
  %D%/automaton.is_proper.ipynb                 \
  %D%/automaton.is_realtime.ipynb               \
  %D%/automaton.is_standard.ipynb               \
  %D%/automaton.is_synchronized.ipynb           \
  %D%/automaton.is_trim.ipynb                   \
  %D%/automaton.is_useless.ipynb                \
  %D%/automaton.is_valid.ipynb                  \
  %D%/automaton.ldivide.ipynb                   \
  %D%/automaton.letterize.ipynb                 \
  %D%/automaton.lift.ipynb                      \
  %D%/automaton.lightest.ipynb                  \
  %D%/automaton.lightest_automaton.ipynb        \
  %D%/automaton.lweight.ipynb                   \
  %D%/automaton.minimize.ipynb                  \
  %D%/automaton.multiply.ipynb                  \
  %D%/automaton.operators.ipynb                 \
  %D%/automaton.pair.ipynb                      \
  %D%/automaton.partial_identity.ipynb          \
  %D%/automaton.prefix.ipynb                    \
  %D%/automaton.project.ipynb                   \
  %D%/automaton.proper.ipynb                    \
  %D%/automaton.push_weights.ipynb              \
  %D%/automaton.rdivide.ipynb                   \
  %D%/automaton.realtime.ipynb                  \
  %D%/automaton.reduce.ipynb                    \
  %D%/automaton.rweight.ipynb                   \
  %D%/automaton.scc.ipynb                       \
  %D%/automaton.shortest.ipynb                  \
  %D%/automaton.shuffle.ipynb                   \
  %D%/automaton.standard.ipynb                  \
  %D%/automaton.star.ipynb                      \
  %D%/automaton.strip.ipynb                     \
  %D%/automaton.subword.ipynb                   \
  %D%/automaton.suffix.ipynb                    \
  %D%/automaton.synchronize.ipynb               \
  %D%/automaton.synchronizing_word.ipynb        \
  %D%/automaton.transpose.ipynb                 \
  %D%/automaton.trim.ipynb                      \
  %D%/automaton.tuple.ipynb                     \
  %D%/automaton.type.ipynb                      \
  %D%/automaton.weight_series.ipynb             \
  %D%/context.cerny.ipynb                       \
  %D%/context.cotrie.ipynb                      \
  %D%/context.de_bruijn.ipynb                   \
  %D%/context.divkbaseb.ipynb                   \
  %D%/context.ladybird.ipynb                    \
  %D%/context.levenshtein.ipynb                 \
  %D%/context.quotkbaseb.ipynb                  \
  %D%/context.random_automaton.ipynb            \
  %D%/context.random_expression.ipynb           \
  %D%/context.random_weight.ipynb               \
  %D%/context.trie.ipynb                        \
  %D%/expansion.add.ipynb                       \
  %D%/expansion.lweight.ipynb                   \
  %D%/expansion.operators.ipynb                 \
  %D%/expansion.project.ipynb                   \
  %D%/expansion.rweight.ipynb                   \
  %D%/expression.add.ipynb                      \
  %D%/expression.automaton.ipynb                \
  %D%/expression.complement.ipynb               \
  %D%/expression.conjunction.ipynb              \
  %D%/expression.constant_term.ipynb            \
  %D%/expression.derivation.ipynb               \
  %D%/expression.derived_term.ipynb             \
  %D%/expression.difference.ipynb               \
  %D%/expression.expansion.ipynb                \
  %D%/expression.inductive.ipynb                \
  %D%/expression.infiltrate.ipynb               \
  %D%/expression.info.ipynb                     \
  %D%/expression.is_equivalent.ipynb            \
  %D%/expression.is_valid.ipynb                 \
  %D%/expression.lift.ipynb                     \
  %D%/expression.lweight.ipynb                  \
  %D%/expression.multiply.ipynb                 \
  %D%/expression.operators.ipynb                \
  %D%/expression.project.ipynb                  \
  %D%/expression.rweight.ipynb                  \
  %D%/expression.shortest.ipynb                 \
  %D%/expression.shuffle.ipynb                  \
  %D%/expression.standard.ipynb                 \
  %D%/expression.star_normal_form.ipynb         \
  %D%/expression.thompson.ipynb                 \
  %D%/expression.transpose.ipynb                \
  %D%/expression.transposition.ipynb            \
  %D%/expression.tuple.ipynb                    \
  %D%/expression.zpc.ipynb                      \
  %D%/label.multiply.ipynb                      \
  %D%/label.operators.ipynb                     \
  %D%/polynomial.add.ipynb                      \
  %D%/polynomial.conjunction.ipynb              \
  %D%/polynomial.cotrie.ipynb                   \
  %D%/polynomial.multiply.ipynb                 \
  %D%/polynomial.operators.ipynb                \
  %D%/polynomial.trie.ipynb                     \
  %D%/polynomial.tuple.ipynb                    \
  %D%/weight.add.ipynb                          \
  %D%/weight.multiply.ipynb                     \
  %D%/weight.operators.ipynb

if HAVE_NBCONVERT
nodist_notebooks_DATA = $(dist_notebooks_DATA:.ipynb=.html) %D%/index.html
endif


## ------- ##
## check.  ##
## ------- ##

check-local: check-doc
check-doc:
	nbdir=$(srcdir)/%D% $(PYTHON) $(srcdir)/%D%/check-notebooks
EXTRA_DIST += %D%/check-notebooks


## --------------- ##
## ipynb -> html.  ##
## --------------- ##


%D%/index.html:
	$(AM_V_GEN)cd $(@D) && ln -sf !Read-me-first.html index.html

MATHJAX_BAD = <script src="https://c328740.ssl.cf1.rackcdn.com/mathjax/latest/MathJax.js?config=TeX-AMS_HTML"></script>
MATHJAX_OK = <script type='text/javascript' src='http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS_HTML'></script>
%.html: %.ipynb
	$(AM_V_GEN)$(mkdir_p) $(@D)
# nbconvert appends ".html" to the argument of --output.
	$(AM_V_at)$(NBCONVERT) $(if $(V:0=),,--log-level=CRITICAL) --stdout "$<" >"$@.tmp"
# The generated HTML files still point to the local ipynb files, which
# is ok for nbviewer, but not for static HTML pages.  So
# s/NOTEBOOK.ipynb/NOTEBOOK.html/ for the notebooks, but not for the
# URLs.  So change only [\w.]+.ipynb.
	$(AM_V_at)$(PERL) -pi					\
	   -e 's{\Q$(MATHJAX_BAD)\E}'"{$(MATHJAX_OK)}g;"	\
	   -e 's{(<a href="[\w.]+\.)ipynb}{$$1html}g;'		\
	   "$@.tmp"
	$(AM_V_at)mv -f "$@.tmp" "$@"

upload-doc: upload-notebooks
upload-notebooks: $(dist_notebooks_DATA) $(nodist_notebooks_DATA)
	ssh $(dload_host) mkdir -p $(dload_dir)/notebooks
	rsync $(RSYNCFLAGS) $^ $(dload_host):$(dload_dir)/notebooks
