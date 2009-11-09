all: pdf 

pdf: presentation.tex
	make figs
	latex $<
	dvips presentation.dvi
	ps2pdf presentation.ps

show:
	open presentation.pdf

clean:
	rm -f presentation.pdf presentation.dvi presentation.aux
	rm -f presentation.log presentation.nav presentation.out
	rm -f presentation.toc presentation.snm presentation.vrb
	rm -f presentation.ps
