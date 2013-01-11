COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log
TEXS=report.tex
PDFS=$(TEXS:.tex=.pdf)

all:
	$(COMPILE) report.tex
	$(COMPILE) report.tex

clean:
	rm -rf $(TRASH) *.pdf

.PHONY: all
