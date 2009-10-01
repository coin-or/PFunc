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
set grid
set ylabel "Heap Memory (normalized)"
## $2 is scaled by the largest
plot \
"memory_numbers/omp.g7jac200.dot.8" using 1:($2/816502) every 200 ls 1 title "Open MP", \
"memory_numbers/cilk.g7jac200.dot.8" using 1:($2/816502) every 200 ls 2 title "Cilk", \
"memory_numbers/tbb.g7jac200.dot.8" using 1:($2/816502) every 200 ls 3 title "TBB", \
"memory_numbers/pfunc_dag_heap.g7jac200.dot.8" using 1:($2/816502) every 200 ls 4 title "PFunc (demand-driven)"
