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
dist_notebooks_data =				\
  '%D%/00 - Welcome to Vaucanson.ipynb'		\
  '%D%/01 - Playing with contexts.ipynb'	\
  '%D%/02 - Basic operations on automata.ipynb'	\
  '%D%/Some developer documentation.ipynb'

dist-hook: dist-hook-%C%
dist-hook-%C%:
	@$(MKDIR_P) $(distdir)/%D%/algos
	@for file in $(dist_notebooks_data);			\
	do							\
	  test -f "$(distdir)/$$file"				\
	  || cp -p "$(srcdir)/$$file" "$(distdir)/$$file"	\
	  || exit 1;						\
	done

# $f: share/vcsn/notebooks/algos/Index.ipynb
# $b: algos/Index.ipynb
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
