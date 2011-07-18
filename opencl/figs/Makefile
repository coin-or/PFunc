all: platform.eps host_program.eps memory.eps kernel.eps work_groups.eps

platform.eps: platform.fig
	fig2dev -L ps $< platform.ps
	ps2eps -f platform.ps

host_program.eps: host_program.fig
	fig2dev -L ps $< host_program.ps
	ps2eps -f host_program.ps

memory.eps: memory.fig
	fig2dev -L ps $< memory.ps
	ps2eps -f memory.ps

kernel.eps: kernel.fig
	fig2dev -L ps $< kernel.ps
	ps2eps -f kernel.ps

work_groups.eps: work_groups.fig
	fig2dev -L ps $< work_groups.ps
	ps2eps -f work_groups.ps

clean:
	rm -rf *.ps *.eps
