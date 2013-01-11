COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log *.tex
TEXS=report.tex
PDFS=$(TEXS:.tex=.pdf)

MD_SOURCES=report.md

all: report.pdf

report.pdf : report.tex
	$(COMPILE) $^
	$(COMPILE) $^

report.tex: $(MD_SOURCES)
	pandoc --template=template/template.tex -f markdown -t latex $^ -o $@

clean:
	rm -rf $(TRASH) *.pdf
