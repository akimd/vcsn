# VCSN
VCSN ?= vcsn
EFSTDECOMPILE = $(VCSN) efstdecompile

# DATA
SYMBOLS = latin.sym
SYNTAX = syntactic.efsm
GRAPHEME = graphemic.efsm

# BIN
STATIC = static/sms-to-fr
DYN = dyn/sms-to-fr

# EFSM
%.efsm: %.fst
	$(EFSTDECOMPILE) --symbols $(SYMBOLS) $< > $@.tmp
	mv $@.tmp $@

# PYTHON
run-python: $(GRAPHEME) $(SYNTAX)
	$(VCSN) python3 python/tests.py -g $(GRAPHEME) -s $(SYNTAX)

# STATIC
$(STATIC):
	$(VCSN) compile $@.cc

run-static: $(STATIC) $(GRAPHEME) $(SYNTAX)
	$(VCSN) run $(STATIC) -g $(GRAPHEME) -s $(SYNTAX)

# DYN
$(DYN):
	$(VCSN) compile $@.cc

run-dyn: $(DYN) $(GRAPHEME) $(SYNTAX)
	$(VCSN) run $(DYN) -g $(GRAPHEME) -s $(SYNTAX)

# CLEAN
clean:
	$(RM) $(STATIC) $(DYN) $(SYNTAX) $(GRAPHEME)
