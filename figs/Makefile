FIGS=memory_g7jac200.eps memory_twotone.eps

all: $(FIGS)

%.eps: %.plt
	gnuplot < $< > $@

clean:
	rm -f $(FIGS)
