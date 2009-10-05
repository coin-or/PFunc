set terminal post eps enhanced color 16 
set style data lines
set style line 1 linetype 1 linewidth 5 ps 2
set style line 2 linetype 2 linewidth 5 ps 2
set style line 3 linetype 3 linewidth 5 ps 2
set style line 4 linetype 4 linewidth 5 ps 2
set style line 5 linetype 5 linewidth 5 ps 2
set style line 6 linetype 6 linewidth 5 ps 2
set style line 7 linetype 7 linewidth 5 ps 2
set xlabel "Time (seconds)"
set ylabel "Heap Memory (normalized)"
set grid
## $2 is scaled by the largest
plot \
"memory_numbers/omp.twotone.dot.8" using 1:($2/107696) every 20 ls 1 title "Open MP", \
"memory_numbers/cilk.twotone.dot.8" using 1:($2/107696) every 20 ls 2 title "Cilk", \
"memory_numbers/tbb.twotone.dot.8" using 1:($2/107696) every 20 ls 3 title "TBB", \
"memory_numbers/pfunc_dag_heap.twotone.dot.8" using 1:($2/107696) every 20 ls 4 title "PFunc (demand-driven)"
