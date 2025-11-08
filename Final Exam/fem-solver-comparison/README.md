# FEM Solver Comparison & Benchmarking

A comprehensive Finite Element Method (FEM) solver implementing and comparing multiple solution algorithms for 2D plane stress analysis.

## Project Overview

This project analyzes a rectangular steel plate (1m × 0.5m × 5mm) under plane stress conditions using the finite element method. The plate is discretized using 4-node quadrilateral elements, and multiple solver methods are benchmarked for performance and accuracy.

### Problem Statement

- **Geometry**: 1.0 m × 0.5 m plate with 5 mm thickness
- **Material**: Steel (E = 210 GPa, ν = 0.3)
- **Boundary Conditions**: Left edge fixed, right edge loaded
- **Element Type**: 4-node isoparametric quadrilateral
- **Analysis Type**: Plane stress

### Load Cases

1. **5 kN Uniform Tensile Load** - Uniform tension on right edge
2. **10 kN Uniform Tensile Load** - Higher uniform tension
3. **Linearly Varying Load** - 0 to 10 kN linear distribution
4. **15 kN Concentrated Load** - Point load at midpoint
5. **10 kN Angled Load** - Load at 30° to horizontal at top-right

## Solution Methods Implemented

### 1. Naive Gaussian Elimination
- **Algorithm**: Direct method with partial pivoting
- **Complexity**: O(n³)
- **Best For**: Small systems, reference solutions
- **Characteristics**: Simple, modifies original matrix

### 2. LU Decomposition
- **Algorithm**: A = LU factorization with pivoting
- **Complexity**: O(n³) factorization, O(n²) per solve
- **Best For**: Multiple right-hand sides
- **Characteristics**: Efficient for repeated solves

### 3. Cholesky Decomposition
- **Algorithm**: A = LL^T for symmetric positive definite matrices
- **Complexity**: O(n³/2) - Half the operations of LU
- **Best For**: FEM stiffness matrices (SPD)
- **Characteristics**: Most efficient direct method for SPD

### 4. Conjugate Gradient (CG)
- **Algorithm**: Iterative Krylov subspace method
- **Complexity**: O(kn²) for k iterations
- **Best For**: Large sparse systems
- **Characteristics**: Low memory, good convergence for well-conditioned matrices

### 5. Sparse Matrix Solver
- **Algorithm**: CG with Compressed Row Storage (CRS)
- **Complexity**: O(k×nnz) for k iterations
- **Best For**: Very large sparse systems
- **Characteristics**: Exploits sparsity, minimal memory

### 6. Gauss-Seidel Iteration
- **Algorithm**: Simple iterative method
- **Complexity**: O(kn²) for k iterations
- **Best For**: Diagonally dominant systems
- **Characteristics**: Simple implementation, low memory

## Building the Project

### Using Make

```bash
# Build the project
make

# Build and run
make run

# Build, run, and generate plots
make plots

# Clean build artifacts
make clean

# Clean everything
make cleanall
```

### Using CMake

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make

# Run
./fem_solver

# Or use custom targets
make run
make plots
```

## Project Structure

```
fem-solver-comparison/
├── src/
│   ├── main.cpp                      # Main driver program
│   ├── matrix/
│   │   ├── stiffness_matrix.h        # Stiffness matrix assembly
│   │   └── stiffness_matrix.cpp
│   ├── solvers/
│   │   ├── naive_gauss.h             # Naive Gaussian elimination
│   │   ├── naive_gauss.cpp
│   │   ├── efficient_solver.h        # Advanced solvers
│   │   └── efficient_solver.cpp
│   ├── fem/
│   │   ├── boundary_conditions.h     # BC and load application
│   │   └── boundary_conditions.cpp
│   ├── analysis/
│   │   ├── stress_strain.h           # Stress/strain computation
│   │   └── stress_strain.cpp
│   └── utils/
│       ├── benchmark.h               # Benchmarking utilities
│       └── benchmark.cpp
├── plots/
│   ├── compare_solvers.gp            # Solver comparison plots
│   ├── stress_distribution.gp        # Stress visualization
│   ├── error_analysis.gp             # Error analysis
│   ├── timing_comparison.gp          # Timing plots
│   ├── displacement_fields.gp        # Displacement plots
│   └── generate_all_plots.gp         # Master plot script
├── results/                          # Output data files
├── Makefile                          # Build system
├── CMakeLists.txt                    # CMake configuration
└── README.md                         # This file
```

## Output and Visualization

### Data Files
The program generates the following data files in `results/`:
- `stress_case*.dat` - Element stress/strain data
- `benchmark_case*.dat` - Solver performance metrics
- `displacement_case*.dat` - Nodal displacements

### Plots
Generated plots (in `plots/`):
- **solver_comparison.png** - Performance comparison across all methods
- **stress_distribution.png** - von Mises stress distribution
- **error_analysis.png** - Solution accuracy comparison
- **timing_comparison.png** - Execution time and speedup
- **displacement_fields.png** - Displacement magnitude fields

## Performance Metrics

The benchmark compares solvers on:
- **Execution Time**: Wall-clock time for solution
- **Speedup**: Relative performance vs slowest method
- **Residual**: ||K*U - F|| accuracy measure
- **Relative Error**: Comparison to reference solution
- **Memory Usage**: Approximate memory requirements

## Theoretical Complexity

| Method | Time Complexity | Space Complexity | Notes |
|--------|----------------|------------------|-------|
| Naive Gauss | O(n³) | O(n²) | Reference method |
| LU Decomp | O(n³) + O(n²) | O(n²) | Good for multiple RHS |
| Cholesky | O(n³/2) | O(n²) | Best direct for SPD |
| Conjugate Gradient | O(kn²) | O(n) | k << n for well-conditioned |
| Sparse CG | O(k·nnz) | O(nnz) | Exploits sparsity |
| Gauss-Seidel | O(kn²) | O(n) | Simple iterative |

## Requirements

- **Compiler**: C++17 compatible (g++ 7+, clang++ 5+)
- **Build Tools**: Make or CMake 3.10+
- **Visualization**: gnuplot 5.0+
- **Libraries**: Standard C++ library only (no external dependencies)

## Usage Example

```bash
# Quick start
make run

# Full analysis with plots
make plots

# View results
ls results/
ls plots/
```

## Error Analysis

The program computes:
1. **Residual Norm**: ||K*U - F|| for each solver
2. **Relative Error**: ||U_computed - U_reference|| / ||U_reference||
3. **Convergence**: Iteration counts for iterative methods

## Why This Problem is Interesting

1. **Practical Relevance**: Plate stress analysis is fundamental in structural engineering
2. **Algorithm Comparison**: Direct vs iterative methods showcase different trade-offs
3. **Sparse Structure**: FEM matrices are sparse, allowing specialized algorithms
4. **Scalability**: Performance differences become dramatic as mesh size increases
5. **Numerical Stability**: Tests robustness of different solution strategies

## Key Takeaways

- **Cholesky decomposition** is typically fastest for small-medium FEM problems
- **Sparse iterative methods** scale best for large problems (100k+ DOF)
- **Direct methods** provide exact solutions but don't scale well
- **Iterative methods** trade accuracy for speed and memory
- **Problem conditioning** significantly affects iterative solver performance

## Authors

[Add your names here]

## Course Information

**Course**: AM5801 - Computational Lab  
**Institution**: [Your Institution]  
**Date**: November 8, 2025

## License

This project is created for educational purposes.
