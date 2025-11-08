# Master gnuplot script to generate all plots

print "Generating all analysis plots..."

# Solver performance comparison
print "  - Solver performance comparison"
load 'compare_solvers.gp'

# Stress distributions
print "  - Stress distributions"
load 'stress_distribution.gp'

# Error analysis
print "  - Error analysis"
load 'error_analysis.gp'

# Timing comparison
print "  - Timing comparison"
load 'timing_comparison.gp'

# Displacement fields
print "  - Displacement fields"
load 'displacement_fields.gp'

print "All plots generated successfully!"
print "Check the plots/ directory for output PNG files."
