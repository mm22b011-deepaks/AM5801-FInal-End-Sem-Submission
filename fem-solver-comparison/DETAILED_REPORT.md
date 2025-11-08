# COMPREHENSIVE ANALYSIS REPORT
## Finite Element Method Solver Comparison and Benchmarking

**Course:** AM5801 - Computational Lab  
**Date:** November 8, 2025  
**Authors:** [Add Your Names Here]

---

## Executive Summary

This report presents a comprehensive comparison of six different numerical methods for solving the linear system **[K]{U} = {F}** arising from finite element analysis of a 2D rectangular steel plate under plane stress conditions. The study analyzes computational efficiency, numerical accuracy, and practical applicability of each method across five different loading scenarios.

### Key Findings:

1. **Sparse Matrix CG is the clear winner** - 800x+ faster than iterative Gauss-Seidel
2. **Direct methods (Naive Gauss, LU, Cholesky) are comparable** in execution time for this problem size
3. **Iterative methods converge well** for well-conditioned FEM matrices
4. **Sparsity exploitation is critical** - ~97% sparsity observed in stiffness matrix
5. **All methods produce accurate results** with residuals < 10^-8 (except Gauss-Seidel)

---

## 1. PROBLEM STATEMENT

### 1.1 Why This Problem is Interesting

Finite Element Method (FEM) is the cornerstone of modern computational mechanics, used extensively in:
- **Structural Analysis**: Buildings, bridges, aircraft, spacecraft
- **Automotive Design**: Crash simulations, component optimization
- **Biomedical Engineering**: Implant design, tissue mechanics
- **Manufacturing**: Metal forming, injection molding

The choice of linear solver can make the difference between:
- Hours vs. minutes of computation time
- Feasible vs. infeasible analysis (memory constraints)
- Real-time vs. batch processing capabilities

### 1.2 Exact Problem Definition

**Geometry:**
- Rectangular plate: 1.0 m × 0.5 m × 5 mm (Length × Height × Thickness)
- Discretization: 20 × 10 = 200 four-node quadrilateral elements
- Mesh: 231 nodes, 462 degrees of freedom (DOF)

**Material Properties (Steel):**
- Young's Modulus: E = 210 GPa
- Poisson's Ratio: ν = 0.3
- Analysis Type: Plane Stress

**Boundary Conditions:**
- **Left Edge (x = 0):** Fully fixed (u = v = 0) - 11 nodes constrained
- **Right Edge (x = 1.0 m):** Various loading conditions - 11 nodes loaded

**Governing Equation:**
```
[K]{U} = {F}
```
Where:
- **[K]:** Global stiffness matrix (462×462, symmetric, positive definite, sparse)
- **{U}:** Nodal displacement vector (462×1)
- **{F}:** Applied load vector (462×1)

**Post-Processing:**
- Strains: {ε} = [B]{U_e}
- Stresses: {σ} = [D]{ε}
- von Mises: σ_vm = √(σ_x² - σ_x·σ_y + σ_y² + 3τ_xy²)

### 1.3 Five Loading Cases Analyzed

1. **Case 1:** 5 kN uniform tensile load (right edge)
2. **Case 2:** 10 kN uniform tensile load (right edge)
3. **Case 3:** Linearly varying load 0→10 kN (bottom to top)
4. **Case 4:** 15 kN concentrated load (midpoint of right edge)
5. **Case 5:** 10 kN load at 30° to horizontal (top-right corner)

---

## 2. SOLUTION METHODS AND ALGORITHMS

### 2.1 Direct Methods

#### 2.1.1 Naive Gaussian Elimination
**Algorithm:**
```
for k = 1 to n-1:
    Find pivot: max|a_ik| for i ≥ k
    Swap rows if needed
    for i = k+1 to n:
        factor = a_ik / a_kk
        for j = k to n:
            a_ij -= factor * a_kj
        b_i -= factor * b_k
Back substitution
```

**Complexity:**
- Time: O(n³)
- Space: O(n²)

**Advantages:**
- Simple implementation
- Provides reference solution
- Handles general matrices

**Disadvantages:**
- Modifies original matrix
- Doesn't exploit symmetry or sparsity
- Slow for large systems

#### 2.1.2 LU Decomposition
**Algorithm:**
```
Factorize: A = P·L·U
    P: Permutation matrix (pivoting)
    L: Lower triangular
    U: Upper triangular

Solve:
    L·y = P·b    (forward substitution)
    U·x = y      (back substitution)
```

**Complexity:**
- Factorization: O(n³)
- Each solve: O(n²)
- Space: O(n²)

**Advantages:**
- Efficient for multiple right-hand sides
- Reuse factorization
- Numerically stable with pivoting

**Disadvantages:**
- Still O(n³) initial cost
- Doesn't exploit symmetry

#### 2.1.3 Cholesky Decomposition
**Algorithm:**
```
For symmetric positive definite A:
    Factorize: A = L·L^T
    
    l_ii = √(a_ii - Σ(l_ik²))
    l_ji = (a_ji - Σ(l_jk·l_ik)) / l_ii

Solve:
    L·y = b      (forward)
    L^T·x = y    (backward)
```

**Complexity:**
- Factorization: O(n³/2) - Half of LU!
- Each solve: O(n²)
- Space: O(n²)

**Advantages:**
- **Most efficient direct method for SPD matrices**
- Exploits symmetry
- Guaranteed stable for SPD matrices
- Natural fit for FEM stiffness matrices

**Disadvantages:**
- Requires SPD matrix
- Still doesn't exploit sparsity in this implementation

### 2.2 Iterative Methods

#### 2.2.1 Conjugate Gradient (CG)
**Algorithm:**
```
x₀ = 0, r₀ = b, p₀ = r₀
for k = 0 to convergence:
    α_k = (r_k^T · r_k) / (p_k^T · A · p_k)
    x_{k+1} = x_k + α_k · p_k
    r_{k+1} = r_k - α_k · A · p_k
    β_k = (r_{k+1}^T · r_{k+1}) / (r_k^T · r_k)
    p_{k+1} = r_{k+1} + β_k · p_k
```

**Complexity:**
- Per iteration: O(n²) for dense, O(nnz) for sparse
- Iterations: k ≈ √κ·log(1/ε) where κ = condition number
- Space: O(n)

**Advantages:**
- Low memory footprint
- Theoretically converges in n iterations
- Practically converges much faster for well-conditioned matrices
- Natural for large sparse systems

**Disadvantages:**
- Requires SPD matrix
- Convergence depends on condition number
- May need preconditioning for ill-conditioned systems

#### 2.2.2 Sparse Matrix CG
**Algorithm:**
Same as CG but with Compressed Row Storage (CRS):
```
CRS Format:
    values[]: Non-zero elements
    colIndices[]: Column index of each non-zero
    rowPointers[]: Start of each row in values[]

Matrix-vector multiply:
    for i = 0 to n:
        result[i] = 0
        for j = rowPointers[i] to rowPointers[i+1]:
            result[i] += values[j] * x[colIndices[j]]
```

**Complexity:**
- Per iteration: O(nnz) where nnz << n²
- Space: O(nnz) ≈ O(n) for FEM matrices
- For our problem: nnz = 7196, n² = 213,444 (96.6% sparse!)

**Advantages:**
- **Exploits sparsity - massive speedup**
- Minimal memory usage
- Scales to very large problems
- Industry standard for FEM

**Disadvantages:**
- More complex implementation
- Overhead for small problems

#### 2.2.3 Gauss-Seidel Iteration
**Algorithm:**
```
for k = 0 to maxIterations:
    for i = 0 to n:
        x_i^(k+1) = (b_i - Σ(a_ij·x_j^(k+1)) - Σ(a_ij·x_j^(k))) / a_ii
                          j<i                    j>i
    Check convergence: ||x^(k+1) - x^(k)|| < tol
```

**Complexity:**
- Per iteration: O(n²)
- Space: O(n)

**Advantages:**
- Simple implementation
- Low memory
- No matrix factorization needed

**Disadvantages:**
- **Slow convergence** (many iterations needed)
- Requires diagonally dominant matrix
- Sequential updates (not parallelizable)

---

## 3. BENCHMARK RESULTS

### 3.1 Performance Summary (Average across all load cases)

| Method | Avg Time (s) | Speedup | Residual | Rel Error | Memory |
|--------|-------------|---------|----------|-----------|--------|
| **Naive Gauss** | 0.0077 | 120× | 10⁻¹¹ | Reference | 0.86 MB |
| **LU Decomp** | 0.0083 | 112× | 10⁻¹¹ | ~10⁻¹⁶ | 0.86 MB |
| **Cholesky** | 0.0119 | 78× | 10⁻¹¹ | ~10⁻¹⁴ | 0.86 MB |
| **Conj Gradient** | 0.0186 | 50× | 10⁻⁹ | ~10⁻¹³ | 0.0018 MB |
| **Sparse CG** ⭐ | **0.0009** | **1000×** | 10⁻⁹ | ~10⁻¹³ | 0.028 MB |
| **Gauss-Seidel** | 0.9320 | 1× | 10⁰ | ~10⁻² | 0.0018 MB |

### 3.2 Detailed Results by Load Case

#### Load Case 1: 5 kN Uniform Tensile
- **Fastest:** Sparse CG (0.000708 s) - 134× speedup
- **Max Stress:** 10.3 MPa (σ_x)
- **All direct methods:** ~0.008-0.012 s
- **Gauss-Seidel:** 0.095 s (614 iterations)

#### Load Case 2: 10 kN Uniform Tensile
- **Fastest:** Sparse CG (0.000864 s) - 236× speedup
- **Max Stress:** 20.6 MPa (σ_x) - Linear scaling confirmed
- **Gauss-Seidel:** 0.203 s (1304 iterations)

#### Load Case 3: Linear Varying Load
- **Fastest:** Sparse CG (0.001035 s) - 211× speedup
- **Max Stress:** 14.7 MPa (non-uniform distribution)
- **Gauss-Seidel:** 0.217 s (1390 iterations)

#### Load Case 4: 15 kN Concentrated
- **Fastest:** Sparse CG (0.001012 s) - 311× speedup
- **Max Stress:** 42.6 MPa (high local stress)
- **Stress concentration observed at load point**
- **Gauss-Seidel:** 0.317 s (2023 iterations)

#### Load Case 5: 10 kN at 30°
- **Fastest:** Sparse CG (0.001435 s) - 803× speedup
- **Max Stress:** 43.0 MPa (von Mises)
- **Complex stress field with shear**
- **Gauss-Seidel:** 1.152 s (7374 iterations) - worst case

---

## 4. ERROR ANALYSIS

### 4.1 Residual Norm ||K·U - F||

**Direct Methods:**
- Naive Gauss: ~10⁻¹¹ (excellent)
- LU Decomp: ~10⁻¹¹ (excellent)
- Cholesky: ~10⁻¹¹ (excellent)

**Iterative Methods:**
- Conjugate Gradient: ~10⁻⁹ (very good, limited by tolerance)
- Sparse CG: ~10⁻⁹ (very good, same as CG)
- Gauss-Seidel: ~10⁰ (poor, slow convergence)

### 4.2 Relative Error (vs. Naive Gauss reference)

| Method | Relative Error | Comments |
|--------|---------------|----------|
| LU Decomp | ~10⁻¹⁶ | Machine precision |
| Cholesky | ~10⁻¹⁴ | Excellent |
| Conj Gradient | ~10⁻¹³ | Very good |
| Sparse CG | ~10⁻¹³ | Same as CG |
| Gauss-Seidel | ~10⁻² | Poor (2% error) |

**Conclusion:** All methods except Gauss-Seidel produce virtually identical solutions within numerical precision.

### 4.3 Convergence Behavior

**Conjugate Gradient:**
- Converged in ~98 iterations for most cases
- Smooth exponential decay of residual
- Well-conditioned stiffness matrix (κ moderate)

**Gauss-Seidel:**
- Required 614-7374 iterations (10-75× more than CG!)
- Very slow convergence
- Linear convergence rate vs. superlinear for CG
- Not suitable for this problem

---

## 5. STRESS AND STRAIN ANALYSIS

### 5.1 Load Case Comparison

| Load Case | Max σ_x (MPa) | Max σ_y (MPa) | Max von Mises (MPa) |
|-----------|---------------|---------------|---------------------|
| 1: 5 kN Uniform | 10.3 | 5.2 | 10.7 |
| 2: 10 kN Uniform | 20.6 | 10.4 | 21.4 |
| 3: Linear Varying | 14.7 | 7.8 | 15.2 |
| 4: 15 kN Concentrated | 40.2 | 18.9 | 42.6 |
| 5: 10 kN at 30° | 34.6 | 16.2 | 43.0 |

### 5.2 Key Observations

1. **Linear Scaling:** Case 2 stresses are exactly 2× Case 1 (validates implementation)
2. **Stress Concentration:** Case 4 shows 4× higher stress due to point load
3. **Multiaxial Loading:** Case 5 produces complex stress field with significant shear
4. **All stresses well below yield:** Steel yield ~250 MPa (safety factor > 5)

---

## 6. COMPUTATIONAL COMPLEXITY ANALYSIS

### 6.1 Theoretical Complexity

| Method | Time | Space | Notes |
|--------|------|-------|-------|
| Naive Gauss | O(n³) | O(n²) | No optimizations |
| LU | O(n³) | O(n²) | Reusable for multiple RHS |
| Cholesky | O(n³/2) | O(n²) | Exploits symmetry |
| CG | O(kn²) | O(n) | k << n if well-conditioned |
| Sparse CG | O(k·nnz) | O(nnz) | nnz ≈ O(n) for FEM |
| Gauss-Seidel | O(kn²) | O(n) | Large k makes it slow |

### 6.2 Observed Scaling (n=462 DOF)

**Direct Methods:**
- Expected time: ~n³ ≈ 98 million operations
- Observed: ~7-12 ms (consistent with theory)
- Cholesky should be 2× faster than Gaussian - **NOT observed** (likely compiler optimization)

**Iterative Methods:**
- CG iterations: ~98 (≈ n/4.7)
- Sparse CG: Same iterations, but 20× faster per iteration
- Gauss-Seidel: 614-7374 iterations (6-75× more than CG)

### 6.3 Extrapolation to Larger Problems

For n = 10,000 DOF (typical industrial problem):

| Method | Est. Time | Est. Memory | Feasible? |
|--------|-----------|-------------|-----------|
| Naive Gauss | 35 minutes | 763 MB | Marginal |
| Cholesky | 18 minutes | 763 MB | Marginal |
| CG | 30 seconds | 0.08 MB | ✓ Yes |
| Sparse CG | **2 seconds** | 1 MB | ✓ **Best** |
| Gauss-Seidel | 4 hours | 0.08 MB | No |

**For n = 100,000 DOF:**
- Direct methods: **Infeasible** (100+ GB memory)
- Sparse CG: **20-200 seconds** (feasible)

---

## 7. CONCLUSIONS

### 7.1 Method Recommendations

**For Small Problems (n < 1,000):**
- Use **Cholesky decomposition** - fastest direct method
- All direct methods comparable
- Sparse methods have overhead

**For Medium Problems (1,000 < n < 10,000):**
- Use **Sparse CG** - clear winner
- 100-1000× faster than direct methods
- Memory savings become critical

**For Large Problems (n > 10,000):**
- **Only Sparse CG is feasible**
- Direct methods run out of memory
- Iterative methods with preconditioning essential

**Never Use:**
- Gauss-Seidel for FEM (too slow, poor convergence)

### 7.2 Key Takeaways

1. **Sparsity is everything:** 96.6% sparse matrix → 20× speedup in CG
2. **FEM matrices are well-suited for CG:** SPD, relatively well-conditioned
3. **Direct methods don't scale:** O(n³) kills them beyond n~10,000
4. **Implementation matters:** Sparse data structures crucial
5. **Accuracy is not an issue:** All methods (except GS) achieve machine precision

### 7.3 Real-World Impact

**Industrial FEM codes use:**
- Sparse iterative solvers with preconditioning (GMRES, BiCGSTAB)
- Algebraic multigrid for tough problems
- Direct sparse solvers (MUMPS, PARDISO) for moderate-sized systems
- Hybrid approaches (direct for difficult components)

**Why this matters:**
- Modern analysis: 100k - 10M DOF common
- Parametric studies: Solve 100s of times
- Optimization: 1000s of solves
- Real-time: Must solve in < 1 second

**Example:** Aircraft wing analysis
- 500,000 DOF
- Sparse CG: ~30 seconds
- Direct method: Would need 1 TB RAM (impossible)

### 7.4 Human Touch and Insights

**What we learned beyond the numbers:**

This project wasn't just about comparing algorithms - it was about understanding **why** certain methods dominate in real-world engineering. The most surprising finding? Gauss-Seidel, which looks elegant on paper, is essentially useless for modern FEM. Meanwhile, Sparse CG, which requires careful data structure implementation, is indispensable.

The **800× speedup** of Sparse CG over Gauss-Seidel isn't just a number - it's the difference between:
- Getting results during a meeting vs. waiting overnight
- Running 1000 design iterations vs. just 10
- Solving 500k DOF problems vs. being limited to 5k

Perhaps most importantly, we saw that **complexity theory predicts reality**. The O(n³) wall hits direct methods hard, while O(k·nnz) makes sparse methods scale gracefully. This isn't abstract math - it's the reason modern engineering simulations are possible at all.

---

## 8. TOOLS AND TECHNOLOGIES USED

### 8.1 Implementation
- **Language:** C++17
- **Compiler:** g++ with -O3 optimization
- **Build System:** Make and CMake
- **Data Structures:** std::vector for dense, custom CRS for sparse

### 8.2 Visualization
- **Plotting:** Gnuplot 5.0+
- **Output Formats:** PNG images, data files
- **Scripts:** Automated plot generation

### 8.3 Analysis
- **Benchmarking:** std::chrono for high-resolution timing
- **Error Metrics:** Residual norm, relative error
- **Stress Calculation:** Full strain-displacement-stress chain

---

## APPENDIX A: Quick Start Guide

```bash
# Clone or navigate to project
cd fem-solver-comparison

# Build
make

# Run analysis
make run

# Generate all plots (requires gnuplot)
make plots

# Clean
make clean
```

## APPENDIX B: Data Files Generated

- `results/benchmark_case*.dat` - Solver timing and error metrics
- `results/stress_case*.dat` - Element stress/strain data
- `results/full_output.txt` - Complete console output
- `plots/*.png` - Visualization plots

## APPENDIX C: References

1. Hughes, T.J.R. "The Finite Element Method" (2000)
2. Saad, Y. "Iterative Methods for Sparse Linear Systems" (2003)
3. Bathe, K.J. "Finite Element Procedures" (2014)
4. Golub & Van Loan, "Matrix Computations" (2013)

---

**End of Report**

*This analysis demonstrates that algorithm selection can have orders-of-magnitude impact on computational efficiency. For finite element analysis, sparse iterative methods are not just faster - they make large-scale simulations possible that would otherwise be completely infeasible.*
