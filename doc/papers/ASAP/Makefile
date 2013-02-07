COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log
BIBTEX=bibtex

TARGET=pgasap2013

all: $(TARGET).pdf

%.pdf : %.tex
	$(COMPILE) $^
	$(BIBTEX) $(TARGET).aux
	$(COMPILE) $^
	$(COMPILE) $^

# report.tex: $(MD_SOURCES)
#       pandoc $(TEMPLATE) $(CSL) $(BIBLIOGRAPHY) $(OPTIONS) -f markdown -t latex $^ -o $@

clean:
	rm -rf $(TRASH) *.pdf

.PHONY: all
