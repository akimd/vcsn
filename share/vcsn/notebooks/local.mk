## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2014 Vaucanson Group.
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

notebooksdir = $(datadir)/vcsn/notebooks

dist_notebooks_DATA =                           \
  %D%/Algorithms.ipynb                          \
  %D%/Contexts.ipynb                            \
  %D%/Editing-Automata.ipynb                    \
  %D%/Hacking.ipynb                             \
  %D%/References.ipynb                          \
  %D%/automaton.accessible.ipynb                \
  %D%/automaton.ambiguous_word.ipynb            \
  %D%/automaton.coaccessible.ipynb              \
  %D%/automaton.complete.ipynb                  \
  %D%/automaton.determinize.ipynb               \
  %D%/automaton.has_twins_property.ipynb        \
  %D%/automaton.is_ambiguous.ipynb              \
  %D%/automaton.is_complete.ipynb               \
  %D%/automaton.is_cycle_ambiguous.ipynb        \
  %D%/automaton.is_deterministic.ipynb          \
  %D%/automaton.is_equivalent.ipynb             \
  %D%/automaton.is_functional.ipynb             \
  %D%/automaton.is_standard.ipynb               \
  %D%/automaton.pair.ipynb                      \
  %D%/automaton.proper.ipynb                    \
  %D%/automaton.reduce.ipynb                    \
  %D%/automaton.standard.ipynb                  \
  %D%/automaton.synchronizing_word.ipynb        \
  %D%/automaton.transpose.ipynb                 \
  %D%/automaton.trim.ipynb                      \
  %D%/context.cerny.ipynb                       \
  %D%/index.ipynb                               \
  %D%/ratexp.derivation.ipynb                   \
  %D%/ratexp.derived_term.ipynb                 \
  %D%/ratexp.expansion.ipynb                    \
  %D%/ratexp.is_equivalent.ipynb                \
  %D%/ratexp.is_valid.ipynb                     \
  %D%/ratexp.standard.ipynb                     \
  %D%/ratexp.thompson.ipynb                     \
  %D%/ratexp.transpose.ipynb                    \
  %D%/ratexp.transposition.ipynb

if HAVE_NBCONVERT
nodist_notebooks_DATA = $(dist_notebooks_DATA:.ipynb=.html)
endif

## --------------- ##
## ipynb -> html.  ##
## --------------- ##


MATHJAX_BAD = <script src="https://c328740.ssl.cf1.rackcdn.com/mathjax/latest/MathJax.js?config=TeX-AMS_HTML"></script>
MATHJAX_OK = <script type='text/javascript' src='http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS_HTML'></script>
%.html: %.ipynb
	$(AM_V_GEN)$(mkdir_p) $(@D)
# nbconvert appends ".html" to the argument of --output.
	$(AM_V_at)$(NBCONVERT) $(if $(V:0=),,--quiet) --output="$*.tmp" "$<"
# The generated HTML files still point to ipynb files, which is ok for
# nbviewer, but not for us.  So s/ipynb/html/.
	$(AM_V_at)$(PERL) -pi					\
	   -e 's{(<a href=".*?\.)ipynb([#"])}{$$1html$$2}g;'	\
	   -e 's{\Q$(MATHJAX_BAD)\E}'"{$(MATHJAX_OK)}g;"	\
	   "$*.tmp.html"
	$(AM_V_at)mv -f "$*.tmp.html" "$@"

upload-doc: upload-notebooks
upload-notebooks: $(dist_notebooks_DATA) $(nodist_notebooks_DATA)
	rsync $(RSYNCFLAGS) $^ $(dload_host):$(dload_dir)/notebooks
