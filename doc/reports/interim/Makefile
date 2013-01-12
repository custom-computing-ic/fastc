COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log *.tex
TEXS=report.tex
PDFS=$(TEXS:.tex=.pdf)

BIBLIOGRAPHY=--bibliography=bib/bibliography.bib
TEMPLATE=--template=template/template.tex
CSL=--csl=bib/haravard-imperial-college-london.csl

MD_SOURCES=report.md

all: report.pdf

report.pdf : report.tex
	$(COMPILE) $^
	$(COMPILE) $^

report.tex: $(MD_SOURCES)
	pandoc $(TEMPLATE) $(BIBLIOGRAPHY) -f markdown -t latex $^ -o $@

clean:
	rm -rf $(TRASH) *.pdf
