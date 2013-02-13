COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log *.bbl *.blg
BIBTEX=bibtex

TARGET=pgasap2013

all: $(TARGET).pdf

SRCS := $(shell find . -name '*.tex')

%.pdf : $(SRCS) 
	$(COMPILE) $(TARGET).tex
	$(BIBTEX) $(TARGET).aux
	$(COMPILE) $(TARGET).tex
	$(COMPILE) $(TARGET).tex


# report.tex: $(MD_SOURCES)
#       pandoc $(TEMPLATE) $(CSL) $(BIBLIOGRAPHY) $(OPTIONS) -f markdown -t latex $^ -o $@

clean:
	rm -rf $(TRASH) $(TARGET).pdf

.PHONY: all
