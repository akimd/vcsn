## Vaucanson 2, a generic library for finite state machines.
## Copyright (C) 2014-2017 Vaucanson Group.
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

include %D%/notebooks/local.mk

## --------- ##
## Doyxgen.  ##
## --------- ##

CLEANFILES += %D%/vcsn.dox
%.dox: %.dox.in
	cd $(top_builddir) && $(SHELL) ./config.status %D%/$@

# We cannot simply use html_DATA here, since Automake does not
# support installing directories.
if ENABLE_DOXYGEN
LAZY_html_DIR += %D%/vcsn.htmldir
endif ENABLE_DOXYGEN


## ------------ ##
## upload-doc.  ##
## ------------ ##

dload_host = doc@perso
dload_dir = /var/www/dload/vcsn/$(PACKAGE_VERSION)
## Doxygen creates *.md5 and *.map files that serve only for
## incremental builds.  Do not propagate them, they are not needed to
## browse the documentation
## (http://osdir.com/ml/text.doxygen.general/2005-04/msg00064.html).
RSYNCFLAGS =                                    \
  --checksum --compress                         \
  --delete --delete-excluded                    \
  --recursive --links --verbose                 \
  --exclude '*.md5' --exclude '*.map'
upload-doc: upload-doxygen
upload-doxygen: %D%/vcsn.htmldir
	ssh $(dload_host) mkdir -p $(dload_dir)
	rsync $(RSYNCFLAGS) $^ $(dload_host):$(dload_dir)
