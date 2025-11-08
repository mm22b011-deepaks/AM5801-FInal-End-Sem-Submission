# Gnuplot script for generating stress distribution plots

set terminal pngcairo enhanced font 'Arial,12' size 1400,1000
set multiplot layout 2,3 title "Stress Distribution for All Load Cases" font "Arial,18"

# Define color palette
set palette defined (0 "blue", 1 "cyan", 2 "green", 3 "yellow", 4 "orange", 5 "red")
set pm3d map
set pm3d interpolate 0,0

# Case 1: 5kN Uniform
set title "Load Case 1: 5kN Uniform Tensile"
set xlabel "X Position (m)"
set ylabel "Y Position (m)"
set cblabel "von Mises Stress (MPa)"
splot 'results/stress_case1.dat' using 2:3:($10/1e6) with pm3d notitle

# Case 2: 10kN Uniform
set title "Load Case 2: 10kN Uniform Tensile"
splot 'results/stress_case2.dat' using 2:3:($10/1e6) with pm3d notitle

# Case 3: Linear Varying
set title "Load Case 3: Linearly Varying Load"
splot 'results/stress_case3.dat' using 2:3:($10/1e6) with pm3d notitle

# Case 4: Concentrated
set title "Load Case 4: 15kN Concentrated"
splot 'results/stress_case4.dat' using 2:3:($10/1e6) with pm3d notitle

# Case 5: Angled
set title "Load Case 5: 10kN at 30°"
splot 'results/stress_case5.dat' using 2:3:($10/1e6) with pm3d notitle

unset multiplot
