COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log
#TEXS=report.tex
#PDFS=$(TEXS:.tex=.pdf)

#BIBLIOGRAPHY=--bibliography=bib/bibliography.bib
#TEMPLATE=--template=template/template.tex
#CSL=--csl bib/acm-sig-proceedings.csl
#OPTIONS=--listings

#MD_SOURCES=report.md

TARGET=pgasap2013

all: $(TARGET).pdf

%.pdf : %.tex
	$(COMPILE) $^
	$(COMPILE) $^

# report.tex: $(MD_SOURCES)
#       pandoc $(TEMPLATE) $(CSL) $(BIBLIOGRAPHY) $(OPTIONS) -f markdown -t latex $^ -o $@

clean:
	rm -rf $(TRASH) *.pdf

.PHONY: all
