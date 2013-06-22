COMPILE=pdflatex
TRASH=*.aux *.toc *.out *.log *.bbl *.blg *.lof *.lol *.lot
BIBTEX=bibtex

TARGET=presentation

all: $(TARGET).pdf

SRCS := $(shell find sections -name '*.tex')
SVGS := $(shell find figs -name '*.svg')
IMGS := $(SVGS:.svg=.pdf_tex)

%.pdf_tex: %.svg
	inkscape -D -z --file=$? --export-pdf=$(@:.pdf_tex=.pdf) --export-latex

$(TARGET).pdf: $(TARGET).tex $(SRCS) $(IMGS)
	$(COMPILE) $(TARGET).tex || rm -rf $(TARGET).pdf

clean:
	rm -rf $(TRASH) $(TARGET).pdf

.PHONY: all
