COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log *.bbl *.blg
BIBTEX=bibtex

TARGET=pgasap2013

all: $(TARGET).pdf

SRCS := $(shell find sections -name '*.tex')

$(TARGET).pdf : $(TARGET).tex $(SRCS) 
	$(COMPILE) $(TARGET).tex || rm -rf $(TARGET).pdf
	$(BIBTEX) $(TARGET).aux 
	$(COMPILE) $(TARGET).tex || rm -rf $(TARGET).pdf
	$(COMPILE) $(TARGET).tex || rm -rf $(TARGET).pdf


# report.tex: $(MD_SOURCES)
#       pandoc $(TEMPLATE) $(CSL) $(BIBLIOGRAPHY) $(OPTIONS) -f markdown -t latex $^ -o $@

clean:
	rm -rf $(TRASH) $(TARGET).pdf

.PHONY: all
