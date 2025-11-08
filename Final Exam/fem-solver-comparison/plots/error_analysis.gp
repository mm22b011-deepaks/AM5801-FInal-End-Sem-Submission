# Gnuplot script for error analysis

set terminal pngcairo enhanced font 'Arial,12' size 1200,900
set output 'plots/error_analysis.png'

set title "Solver Accuracy Comparison (Relative Error)" font "Arial,16"
set xlabel "Solver Method" font "Arial,14"
set ylabel "Relative Error (log scale)" font "Arial,14"
set logscale y
set grid ytics
set style fill solid 0.8
set boxwidth 0.7

set xtics rotate by -45
set key off

# Bar colors
set style line 1 lc rgb '#4DAF4A'

plot 'results/error_comparison.dat' using 2:xtic(1) with boxes ls 1 notitle
