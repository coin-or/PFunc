all: pdf 

pdf: presentation.tex
	make figs
	pdflatex $<

show:
	open presentation.pdf

clean:
	rm -f presentation.pdf presentation.dvi presentation.aux
	rm -f presentation.log presentation.nav presentation.out
	rm -f presentation.toc presentation.snm presentation.vrb
