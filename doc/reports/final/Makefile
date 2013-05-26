COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log *.bbl *.blg
BIBTEX=bibtex

TARGET=report

all: $(TARGET).pdf

SRCS := $(shell find sections -name '*.tex')

$(TARGET).pdf : $(TARGET).tex $(SRCS)
	$(COMPILE) $(TARGET).tex || rm -rf $(TARGET).pdf
	$(BIBTEX) $(TARGET).aux
	$(COMPILE) $(TARGET).tex || rm -rf $(TARGET).pdf
	$(COMPILE) $(TARGET).tex || rm -rf $(TARGET).pdf


clean:
	rm -rf $(TRASH) $(TARGET).pdf

.PHONY: all
