pdf: presentation.tex
	make -C figs all
	latex $< 
	latex $< 
	latex $< 
	dvipdf presentation.dvi

clean:
	make -C figs clean
	rm -rf *.dvi *.pdf *.vrb *.log *.snm *.nav *.aux *.out *.toc
