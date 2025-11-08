# Gnuplot script for displacement field visualization

set terminal pngcairo enhanced font 'Arial,12' size 1400,1000
set output 'plots/displacement_fields.png'

set multiplot layout 2,3 title "Displacement Magnitude for All Load Cases" font "Arial,18"

set palette defined (0 "blue", 1 "cyan", 2 "green", 3 "yellow", 4 "red")
set pm3d map
set view map

# Case 1
set title "Load Case 1: 5kN Uniform"
set xlabel "X (m)"
set ylabel "Y (m)"
set cblabel "Displacement (mm)"
splot 'results/displacement_case1.dat' using 1:2:(sqrt($3*$3+$4*$4)*1000) with pm3d notitle

# Case 2
set title "Load Case 2: 10kN Uniform"
splot 'results/displacement_case2.dat' using 1:2:(sqrt($3*$3+$4*$4)*1000) with pm3d notitle

# Case 3
set title "Load Case 3: Linear Varying"
splot 'results/displacement_case3.dat' using 1:2:(sqrt($3*$3+$4*$4)*1000) with pm3d notitle

# Case 4
set title "Load Case 4: 15kN Concentrated"
splot 'results/displacement_case4.dat' using 1:2:(sqrt($3*$3+$4*$4)*1000) with pm3d notitle

# Case 5
set title "Load Case 5: 10kN at 30°"
splot 'results/displacement_case5.dat' using 1:2:(sqrt($3*$3+$4*$4)*1000) with pm3d notitle

unset multiplot
