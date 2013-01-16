COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log *.tex
TEXS=report.tex
PDFS=$(TEXS:.tex=.pdf)

BIBLIOGRAPHY=--bibliography=bib/bibliography.bib
TEMPLATE=--template=template/template.tex
#CSL=--csl=bib/haravard-imperial-college-london.csl
#CSL=--csl bib/acm-sig-proceedings.csl
CSL=--csl bib/ieee-w-url.csl
OPTIONS=--listings --toc

MD_SOURCES=report.md

all: report.pdf

report.pdf : report.tex
	$(COMPILE) $^
	$(COMPILE) $^

report.tex: $(MD_SOURCES)
	pandoc $(TEMPLATE) $(CSL) $(BIBLIOGRAPHY) $(OPTIONS) -f markdown -t latex $^ -o $@

clean:
	rm -rf $(TRASH) *.pdf
