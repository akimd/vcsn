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

# Unfortunately, we can't deal with space in file names in Automake,
# and it is really unacceptable to forbid them.  So work around
# Automake.
dist_notebooks_data =                                   \
  '%D%/Contexts.ipynb'                                  \
  '%D%/Editing Automata.ipynb'                          \
  '%D%/References.ipynb'                                \
  '%D%/Some developer documentation.ipynb'              \
  '%D%/algos/automaton.accessible.ipynb'                \
  '%D%/algos/automaton.ambiguous_word.ipynb'            \
  '%D%/algos/automaton.coaccessible.ipynb'              \
  '%D%/algos/automaton.complete.ipynb'                  \
  '%D%/algos/automaton.determinize.ipynb'               \
  '%D%/algos/automaton.has_twins_property.ipynb'        \
  '%D%/algos/automaton.is_ambiguous.ipynb'              \
  '%D%/algos/automaton.is_complete.ipynb'               \
  '%D%/algos/automaton.is_cycle_ambiguous.ipynb'        \
  '%D%/algos/automaton.is_deterministic.ipynb'          \
  '%D%/algos/automaton.is_functional.ipynb'             \
  '%D%/algos/automaton.is_standard.ipynb'               \
  '%D%/algos/automaton.proper.ipynb'                    \
  '%D%/algos/automaton.reduce.ipynb'                    \
  '%D%/algos/automaton.standard.ipynb'                  \
  '%D%/algos/automaton.synchronizing_word.ipynb'        \
  '%D%/algos/automaton.transpose.ipynb'                 \
  '%D%/algos/automaton.trim.ipynb'                      \
  '%D%/algos/index.ipynb'                               \
  '%D%/algos/ratexp.derivation.ipynb'                   \
  '%D%/algos/ratexp.derived_term.ipynb'                 \
  '%D%/algos/ratexp.expansion.ipynb'                    \
  '%D%/algos/ratexp.is_valid.ipynb'                     \
  '%D%/algos/ratexp.standard.ipynb'                     \
  '%D%/algos/ratexp.transpose.ipynb'                    \
  '%D%/algos/ratexp.transposition.ipynb'                \
  '%D%/index.ipynb'

dist-hook: dist-hook-%C%
dist-hook-%C%:
	@$(MKDIR_P) $(distdir)/%D%/algos
	@for file in $(dist_notebooks_data);			\
	do							\
	  test -f "$(distdir)/$$file"				\
	  || cp -p "$(srcdir)/$$file" "$(distdir)/$$file"	\
	  || exit 1;						\
	done

# $f: share/vcsn/notebooks/algos/index.ipynb
# $b: algos/index.ipynb
install-data-hook: install-hook-%C%
install-hook-%C%:
	@$(NORMAL_INSTALL)
	@dir="$(DESTDIR)$(notebooksdir)";			\
	for f in $(dist_notebooks_data);			\
	do							\
	  b=$$(echo "$$f" | sed -e 's,^%D%/,,');		\
	  if test ! -f "$$dir/$$b"; then			\
	    dest=$$(dirname "$$dir/$$b");			\
	    echo " $(MKDIR_P) '$$dest'";			\
	    $(MKDIR_P) "$$dest" || exit 1;			\
	    echo " $(INSTALL_DATA) '$(srcdir)/$$f' '$$dest'";	\
	    $(INSTALL_DATA) "$(srcdir)/$$f" "$$dest" || exit 1;	\
	  fi							\
	done

uninstall-hook: uninstall-hook-%C%
uninstall-hook-%C%:
	@$(NORMAL_UNINSTALL)
	@dir="$(DESTDIR)$(notebooksdir)";		\
	for f in $(dist_notebooks_data);		\
	do						\
	  b=$$(echo "$$f" | sed -e 's,^%D%/,,');	\
	  if test -f "$$dir/$$b"; then			\
	    echo " rm -f '$$dir/$$b'";			\
	    rm -f "$$dir/$$b" || exit 1;		\
	  fi						\
	done

# The generated HTML files still point to ipynb files, which is ok for
# nbviewer, but not for us.  So s/ipynb/html/.
IPYTHON = ipython
NBCONVERT = $(IPYTHON) nbconvert
MATHJAX_BAD = <script src="https://c328740.ssl.cf1.rackcdn.com/mathjax/latest/MathJax.js?config=TeX-AMS_HTML"></script>
MATHJAX_OK = <script type='text/javascript' src='http://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS_HTML'></script>
html:
	rm -rf notebooks
	for f in $(dist_notebooks_data);				 \
	do								 \
	  out=$$(echo "$$f" 						 \
	         | sed -e 's,^%D%/,notebooks/,;s/\.ipynb$$//');		 \
	  dest=$$(dirname "$$out");					 \
	  $(MKDIR_P) "$$dest" || exit 1;				 \
	  $(NBCONVERT) --output="$$out" $(abs_top_srcdir)/"$$f"		 \
	    || exit 1;							 \
	  $(PERL) -pi                                                    \
	     -e 's{(<a href=".*?\.)ipynb(">)}{$$1html$$2};'              \
	     -e 's{\Q$(MATHJAX_BAD)\E}'"{$(MATHJAX_OK)};"                \
	     "$$out.html"                                                \
	    || exit 1;							 \
	done
