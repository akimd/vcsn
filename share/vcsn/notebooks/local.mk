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
# Unfortunately, we can't deal with space in file names in Automake.
dist_notebooks_DATA =                           \
  %D%/00_-_Welcome_to_Vaucanson.ipynb           \
  %D%/01_-_Playing_with_contexts.ipynb          \
  %D%/02_-_Basic_operations_on_automata.ipynb   \
  %D%/Some_developer_documentation.ipynb
