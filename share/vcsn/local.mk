## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2013-2014 Vaucanson Group.
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

jsdir = $(datadir)/vcsn/js
dist_js_DATA =                                         \
  %D%/js/AutomatonD3Widget.js

jupyter_profiledir = $(datadir)/vcsn/jupyter_profile
dist_jupyter_profile_DATA =			\
  %D%/jupyter_profile/custom/custom.css	\
  %D%/jupyter_profile/custom/custom.js	\
  %D%/jupyter_profile/custom/logo.png


notebook_profiledir = $(datadir)/vcsn/notebook_profile
dist_notebook_profile_DATA =			\
  %D%/notebook_profile/static/custom/custom.css	\
  %D%/notebook_profile/static/custom/custom.js	\
  %D%/notebook_profile/static/custom/logo.png

lal_char_bdir = $(datadir)/vcsn/lal_char_b
dist_lal_char_b_DATA =				\
  %D%/lal_char_b/a1.gv				\
  %D%/lal_char_b/b1.gv				\
  %D%/lal_char_b/evena.gv			\
  %D%/lal_char_b/oddb.gv

lal_char_zdir = $(datadir)/vcsn/lal_char_z
dist_lal_char_z_DATA =				\
  %D%/lal_char_z/b1.gv				\
  %D%/lal_char_z/binary.gv			\
  %D%/lal_char_z/c1.gv				\
  %D%/lal_char_z/d1.gv

lal_char_zmindir = $(datadir)/vcsn/lal_char_zmin
dist_lal_char_zmin_DATA =			\
  %D%/lal_char_zmin/minab.gv			\
  %D%/lal_char_zmin/minblocka.gv		\
  %D%/lal_char_zmin/slowgrow.gv
