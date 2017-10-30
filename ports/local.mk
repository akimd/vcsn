## Vcsn, a generic library for finite state machines.
## Copyright (C) 2017 Vcsn Group.
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

.PHONY: Portfile $(srcdir)/%D%/vcsn.macports

# Download the portfile from MacPorts.
Portfile: $(srcdir)/%D%/vcsn.macports

$(srcdir)/%D%/vcsn.macports:
	$(AM_V_GEN) curl 'https://raw.githubusercontent.com/macports/macports-ports/master/devel/vcsn/Portfile' >$@.tmp
	$(AM_V_at) $(move_if_change) $@.tmp $@
