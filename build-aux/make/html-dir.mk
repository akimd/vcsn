## This file provides support for html_DIR, which is similar to
## html_DATA, except that it makes easier to install directories.
##
## Assign to html_DIR or LAZY_html_DIR the top-level of the trees that
## must be installed in $(htmldir).  What's put in html_DIR is built
## at "make" time, what's in LAZY_html_DIR might be compiled only at
## "make install" time.

html_DIR =
EXTRA_html_DIR =
LAZY_html_DIR =
ALL_html_DIR = $(html_DIR) $(EXTRA_html_DIR) $(LAZY_html_DIR)
## So that "make" does what "make install" wants.
noinst_DATA = $(html_DIR)
CLEANDIRS += $(ALL_html_DIR)

.PHONY: install-html-dir
INSTALL_DATA_HOOKS += install-html-dir

## Doxygen creates *.md5 and *.map files that serve only for
## incremental builds.  Do not propagate them, they are not needed to
## browse the documentation
## (http://osdir.com/ml/text.doxygen.general/2005-04/msg00064.html).
install-html-dir: $(html_DIR) $(LAZY_html_DIR)
	@$(NORMAL_INSTALL)
	$(MKDIR_P) $(DESTDIR)$(htmldir)
	for d in $<;					\
	do						\
	  base=$$(basename $$d);			\
	  dir=$$(dirname $$d);				\
	  tar -C $$dir					\
	    --exclude '*.md5' --exclude '*.map'		\
	    -cf - $$base				\
	    | tar -C $(DESTDIR)$(htmldir) -xf -;	\
	done

uninstall-hook: uninstall-html-dir
uninstall-html-dir:
	@$(NORMAL_UNINSTALL)
	for d in $(html_DIR) $(LAZY_html_DIR);	\
	do					\
	  base=$$(basename $$d);		\
	  rm -rf $(DESTDIR)$(htmldir)/$$base;	\
	done
