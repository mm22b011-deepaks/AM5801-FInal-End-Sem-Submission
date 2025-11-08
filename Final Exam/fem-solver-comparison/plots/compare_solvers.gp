# Gnuplot script for comparing solver performance

set terminal pngcairo enhanced font 'Arial,12' size 1200,800
set output 'plots/solver_comparison.png'

set title "Solver Performance Comparison Across All Load Cases" font "Arial,16"
set xlabel "Load Case" font "Arial,14"
set ylabel "Execution Time (seconds)" font "Arial,14"
set grid
set style data histogram
set style histogram clustered gap 1
set style fill solid 0.8 border -1
set boxwidth 0.9
set key outside right top

# Set colors for each solver
set style line 1 lc rgb '#E41A1C' # Red
set style line 2 lc rgb '#377EB8' # Blue
set style line 3 lc rgb '#4DAF4A' # Green
set style line 4 lc rgb '#984EA3' # Purple
set style line 5 lc rgb '#FF7F00' # Orange
set style line 6 lc rgb '#FFFF33' # Yellow

set xtics ("5kN Uniform" 1, "10kN Uniform" 2, "Linear Varying" 3, "15kN Concentrated" 4, "10kN Angled" 5)

plot 'results/benchmark_case1.dat' using 2:xtic(1) title 'Naive Gauss' ls 1, \
     '' using 3 title 'LU Decomp' ls 2, \
     '' using 4 title 'Cholesky' ls 3, \
     '' using 5 title 'Conjugate Gradient' ls 4, \
     '' using 6 title 'Sparse CG' ls 5, \
     '' using 7 title 'Gauss-Seidel' ls 6
