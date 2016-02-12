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
  %D%/Algorithms.ipynb                          \
  %D%/Automata.ipynb                            \
  %D%/Contexts.ipynb                            \
  %D%/Expressions.ipynb                         \
  %D%/Glossary.ipynb                            \
  %D%/Hacking.ipynb                             \
  %D%/References.ipynb                          \
  %D%/Spell-checker.ipynb                       \
  %D%/Stackoverflow.ipynb                       \
  %D%/Transducers.ipynb                         \
  %D%/automaton.accessible.ipynb                \
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
  %D%/automaton.expression.ipynb                \
  %D%/automaton.factor.ipynb                    \
  %D%/automaton.filter.ipynb                    \
  %D%/automaton.has_bounded_lag.ipynb           \
  %D%/automaton.has_lightening_cycle.ipynb      \
  %D%/automaton.has_twins_property.ipynb        \
  %D%/automaton.infiltration.ipynb              \
  %D%/automaton.info.ipynb                      \
  %D%/automaton.insplit.ipynb                   \
  %D%/automaton.is_accessible.ipynb             \
  %D%/automaton.is_ambiguous.ipynb              \
  %D%/automaton.is_coaccessible.ipynb           \
  %D%/automaton.is_codeterministic.ipynb        \
  %D%/automaton.is_complete.ipynb               \
  %D%/automaton.is_cycle_ambiguous.ipynb        \
  %D%/automaton.is_deterministic.ipynb          \
  %D%/automaton.is_empty.ipynb                  \
  %D%/automaton.is_equivalent.ipynb             \
  %D%/automaton.is_functional.ipynb             \
  %D%/automaton.is_isomorphic.ipynb             \
  %D%/automaton.is_letterized.ipynb             \
  %D%/automaton.is_partial_identity.ipynb       \
  %D%/automaton.is_realtime.ipynb               \
  %D%/automaton.is_standard.ipynb               \
  %D%/automaton.is_synchronized.ipynb           \
  %D%/automaton.is_trim.ipynb                   \
  %D%/automaton.is_useless.ipynb                \
  %D%/automaton.is_valid.ipynb                  \
  %D%/automaton.letterize.ipynb                 \
  %D%/automaton.lift.ipynb                      \
  %D%/automaton.lightest.ipynb                  \
  %D%/automaton.lightest_automaton.ipynb        \
  %D%/automaton.minimize.ipynb                  \
  %D%/automaton.multiply.ipynb                  \
  %D%/automaton.operators.ipynb                 \
  %D%/automaton.pair.ipynb                      \
  %D%/automaton.partial_identity.ipynb          \
  %D%/automaton.prefix.ipynb                    \
  %D%/automaton.project.ipynb                   \
  %D%/automaton.proper.ipynb                    \
  %D%/automaton.push_weights.ipynb              \
  %D%/automaton.realtime.ipynb                  \
  %D%/automaton.reduce.ipynb                    \
  %D%/automaton.scc.ipynb                       \
  %D%/automaton.shortest.ipynb                  \
  %D%/automaton.shuffle.ipynb                   \
  %D%/automaton.standard.ipynb                  \
  %D%/automaton.star.ipynb                      \
  %D%/automaton.subword.ipynb                   \
  %D%/automaton.suffix.ipynb                    \
  %D%/automaton.sum.ipynb                       \
  %D%/automaton.synchronize.ipynb               \
  %D%/automaton.synchronizing_word.ipynb        \
  %D%/automaton.transpose.ipynb                 \
  %D%/automaton.trim.ipynb                      \
  %D%/automaton.type.ipynb                      \
  %D%/automaton.weight_series.ipynb             \
  %D%/context.cerny.ipynb                       \
  %D%/context.cotrie.ipynb                      \
  %D%/context.de_bruijn.ipynb                   \
  %D%/context.divkbaseb.ipynb                   \
  %D%/context.ladybird.ipynb                    \
  %D%/context.levenshtein.ipynb                 \
  %D%/context.trie.ipynb                        \
  %D%/expansion.operators.ipynb                 \
  %D%/expression.automaton.ipynb                \
  %D%/expression.complement.ipynb               \
  %D%/expression.conjunction.ipynb              \
  %D%/expression.derivation.ipynb               \
  %D%/expression.derived_term.ipynb             \
  %D%/expression.difference.ipynb               \
  %D%/expression.expansion.ipynb                \
  %D%/expression.is_equivalent.ipynb            \
  %D%/expression.is_valid.ipynb                 \
  %D%/expression.multiply.ipynb                 \
  %D%/expression.operators.ipynb                \
  %D%/expression.shortest.ipynb                 \
  %D%/expression.standard.ipynb                 \
  %D%/expression.star_normal_form.ipynb         \
  %D%/expression.sum.ipynb                      \
  %D%/expression.thompson.ipynb                 \
  %D%/expression.transpose.ipynb                \
  %D%/expression.transposition.ipynb            \
  %D%/expression.zpc.ipynb                      \
  %D%/index.ipynb                               \
  %D%/label.operators.ipynb                     \
  %D%/polynomial.conjunction.ipynb              \
  %D%/polynomial.cotrie.ipynb                   \
  %D%/polynomial.multiply.ipynb                 \
  %D%/polynomial.operators.ipynb                \
  %D%/polynomial.trie.ipynb                     \
  %D%/weight.operators.ipynb

if HAVE_NBCONVERT
nodist_notebooks_DATA = $(dist_notebooks_DATA:.ipynb=.html)
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


MATHJAX_BAD = <script src="https://c328740.ssl.cf1.rackcdn.com/mathjax/latest/MathJax.js?config=TeX-AMS_HTML"></script>
MATHJAX_OK = <script type='text/javascript' src='http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS_HTML'></script>
%.html: %.ipynb
	$(AM_V_GEN)$(mkdir_p) $(@D)
# nbconvert appends ".html" to the argument of --output.
	$(AM_V_at)$(NBCONVERT) $(if $(V:0=),,--log-level=CRITICAL) --output="$*.tmp" "$<"
# The generated HTML files still point to ipynb files, which is ok for
# nbviewer, but not for us.  So s/ipynb/html/.
	$(AM_V_at)$(PERL) -pi					\
	   -e 's{(<a href=".*?\.)ipynb([#"])}{$$1html$$2}g;'	\
	   -e 's{\Q$(MATHJAX_BAD)\E}'"{$(MATHJAX_OK)}g;"	\
	   "$*.tmp.html"
	$(AM_V_at)mv -f "$*.tmp.html" "$@"

upload-doc: upload-notebooks
upload-notebooks: $(dist_notebooks_DATA) $(nodist_notebooks_DATA)
	ssh $(dload_host) mkdir -p $(dload_dir)/notebooks
	rsync $(RSYNCFLAGS) $^ $(dload_host):$(dload_dir)/notebooks
