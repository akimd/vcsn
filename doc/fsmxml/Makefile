BASE = fsmxml.spec
PDF = $(BASE).pdf
DVI = $(BASE).dvi
SRC = $(BASE).tex

BIBDIR = bib/
DEPDIR = src/

DEP = $(wildcard $(DEPDIR)/*.tex)

TEXI2DVI = texi2dvi
TEXI2DVI_FLAGS = --tidy --build-dir=tmp.t2d --batch -I $(BIBDIR) -I $(DEPDIR)

# --dvipdf is required for Vaucanson-G
TEXI2PDF = $(TEXI2DVI) --pdf --dvipdf
TEXI2PDF_FLAGS = $(TEXI2DVI_FLAGS)

all: $(PDF) $(DVI)

pdf: $(PDF)

dvi: $(DVI)

view: $(PDF)
	xpdf $(PDF)

%.pdf : $(SRC) $(DEP)
	$(TEXI2PDF) $(TEXI2PDF_FLAGS) -o $@ $<

%.dvi : $(SRC) $(DEP)
	$(TEXI2DVI) $(TEXI2DVI_FLAGS) -o $@ $<

clean:
	rm -rf tmp.t2d
	rm -rf $(DVI) $(PDF)
