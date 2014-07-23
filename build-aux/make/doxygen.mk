## Copyright (C) 2010-2012, Gostai S.A.S.
##
## This software is provided "as is" without warranty of any kind,
## either expressed or implied, including but not limited to the
## implied warranties of fitness for a particular purpose.
##
## See the LICENSE file for more information.

## \file make/doxygen.mk
## This file is part of build-aux.

## ----------------------- ##
## Doxygen documentation.  ##
## ----------------------- ##

# Make doxygen silent by default, unless V=1.
baux_doxygen_verbose = $(baux_doxygen_verbose_$(V))
baux_doxygen_verbose_ = $(baux_doxygen_verbose_$(AM_DEFAULT_VERBOSITY))
baux_doxygen_verbose_0 = @echo "  DOXYGEN $@";

# Use Doxygen.
%.htmldir: %.dox
	$(AM_V_at)rm -rf $@ $@.tmp
	$(AM_V_at)mkdir -p $@.tmp
	$(baux_doxygen_verbose){		\
	  cat $<;				\
	  echo "OUTPUT_DIRECTORY = $@.tmp";	\
	  if test 'x$(V)' = x1; then		\
	    echo "QUIET = NO";			\
	  else					\
	    echo "QUIET = YES";			\
	  fi					\
	} |					\
	  $(DOXYGEN) $(AM_DOXYGENFLAGS) -
	$(AM_V_at)mv $@.tmp/html $@
	$(AM_V_at)rm -rf $@.tmp
