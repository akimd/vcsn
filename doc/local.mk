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


include $(top_srcdir)/build-aux/make/doxygen.mk
include $(top_srcdir)/build-aux/make/html-dir.mk

include %D%/manual/local.mk

EXTRA_DIST +=					\
  %D%/figs/vcsn.png				\
  %D%/figs/vcsn.mini.png

## ----- ##
## Doc.  ##
## ----- ##

CLEANFILES += %D%/vcsn.dox
%.dox: %.dox.in
	cd $(top_builddir) && $(SHELL) ./config.status %D%/$@

# We cannot simply use html_DATA here, since Automake does not
# support installing directories.
html_DIR += %D%/vcsn.htmldir

# FIXME: developed independently; I'm not necessarily saying
# this needs to stay, but I find it convenient.  --LS
.PHONY: doxygen
doxygen:
	@if [ @DOXYGEN@ = nodoxygen ]; then \
	  echo 'Not generating doxygen documentation'; \
	else \
	  cd $(abs_top_srcdir) \
	  && $(DOXYGEN) \
          && $(MAKE) -C $(abs_top_srcdir)/doc/doxygen/latex pdf; \
	fi

.PHONY: doc
doc: doxygen # FIXME: add more doc dependencies

# PDF manual
.PHONY: manual
manual:
	$(top_srcdir)/doc/manual/COMPILE-MANUAL
