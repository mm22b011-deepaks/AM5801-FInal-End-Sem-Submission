# Gnuplot script for timing analysis

set terminal pngcairo enhanced font 'Arial,12' size 1400,600
set output 'plots/timing_comparison.png'

set multiplot layout 1,2

# Left plot: Execution time comparison
set title "Execution Time by Solver" font "Arial,14"
set xlabel "Solver Method" font "Arial,12"
set ylabel "Time (seconds)" font "Arial,12"
set style fill solid 0.8
set boxwidth 0.7
set xtics rotate by -45
set grid ytics
set key off

plot 'results/benchmark_case1.dat' using 2:xtic(1) with boxes lc rgb '#377EB8' notitle

# Right plot: Speedup comparison
set title "Speedup Relative to Slowest Method" font "Arial,14"
set xlabel "Solver Method" font "Arial,12"
set ylabel "Speedup Factor" font "Arial,12"
set yrange [0:*]

# Calculate speedup (assuming slowest is reference)
plot 'results/benchmark_case1.dat' using (1.0/$2):xtic(1) with boxes lc rgb '#E41A1C' notitle

unset multiplot
