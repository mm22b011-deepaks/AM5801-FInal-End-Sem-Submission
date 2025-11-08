#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "matrix/stiffness_matrix.h"
#include "solvers/naive_gauss.h"
#include "solvers/efficient_solver.h"
#include "fem/boundary_conditions.h"
#include "analysis/stress_strain.h"
#include "utils/benchmark.h"

void printHeader() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                            ║\n";
    std::cout << "║         FINITE ELEMENT METHOD SOLVER COMPARISON & BENCHMARKING             ║\n";
    std::cout << "║                    2D Plane Stress Analysis                                ║\n";
    std::cout << "║                                                                            ║\n";
    std::cout << "║  Problem: Rectangular Steel Plate under Various Loading Conditions        ║\n";
    std::cout << "║  Authors: [Your Names Here]                                               ║\n";
    std::cout << "║  Course: AM5801 - Computational Lab                                        ║\n";
    std::cout << "║  Date: November 8, 2025                                                    ║\n";
    std::cout << "║                                                                            ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════════════╝\n";
    std::cout << std::endl;
}

void printProblemStatement() {
    std::cout << "\n════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "                     PROBLEM STATEMENT" << std::endl;
    std::cout << "════════════════════════════════════════════════════════════════\n" << std::endl;
    
    std::cout << "We analyze a 2D rectangular steel plate subjected to plane stress" << std::endl;
    std::cout << "conditions using the finite element method. The plate is:" << std::endl;
    std::cout << "  • Dimensions: 1.0 m × 0.5 m × 5 mm (Length × Height × Thickness)" << std::endl;
    std::cout << "  • Material: Steel with E = 210 GPa, ν = 0.3" << std::endl;
    std::cout << "  • Left edge: Fully fixed (u = v = 0)" << std::endl;
    std::cout << "  • Right edge: Various loading conditions" << std::endl;
    std::cout << "\nThe plate is discretized using 4-node quadrilateral elements." << std::endl;
    std::cout << "We solve [K]{U} = {F} using multiple methods and compare their" << std::endl;
    std::cout << "computational efficiency, accuracy, and convergence properties." << std::endl;
    std::cout << "\n════════════════════════════════════════════════════════════════\n" << std::endl;
}

void printMethodology() {
    std::cout << "\n════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "                     SOLUTION METHODS" << std::endl;
    std::cout << "════════════════════════════════════════════════════════════════\n" << std::endl;
    
    std::cout << "1. NAIVE GAUSSIAN ELIMINATION" << std::endl;
    std::cout << "   • Direct method with partial pivoting" << std::endl;
    std::cout << "   • Complexity: O(n³) for n×n system" << std::endl;
    std::cout << "   • Best for: Small to medium systems, reference solution" << std::endl;
    std::cout << "   • Drawback: Slow for large systems, modifies original matrix\n" << std::endl;
    
    std::cout << "2. LU DECOMPOSITION" << std::endl;
    std::cout << "   • Factorization: A = LU with partial pivoting" << std::endl;
    std::cout << "   • Complexity: O(n³) decomposition, O(n²) per solve" << std::endl;
    std::cout << "   • Best for: Multiple right-hand sides" << std::endl;
    std::cout << "   • Advantage: Efficient for solving with same K\n" << std::endl;
    
    std::cout << "3. CHOLESKY DECOMPOSITION" << std::endl;
    std::cout << "   • Factorization: A = LL^T (for symmetric positive definite)" << std::endl;
    std::cout << "   • Complexity: O(n³/2) - Half the operations of LU" << std::endl;
    std::cout << "   • Best for: Symmetric systems (FEM stiffness matrices)" << std::endl;
    std::cout << "   • Advantage: Most efficient direct method for SPD matrices\n" << std::endl;
    
    std::cout << "4. CONJUGATE GRADIENT (CG)" << std::endl;
    std::cout << "   • Iterative Krylov subspace method" << std::endl;
    std::cout << "   • Complexity: O(kn²) for k iterations" << std::endl;
    std::cout << "   • Best for: Large sparse systems" << std::endl;
    std::cout << "   • Advantage: Low memory, good for well-conditioned matrices\n" << std::endl;
    
    std::cout << "5. SPARSE MATRIX SOLVER (CG with CRS)" << std::endl;
    std::cout << "   • Uses Compressed Row Storage format" << std::endl;
    std::cout << "   • Complexity: O(k×nnz) for k iterations, nnz non-zeros" << std::endl;
    std::cout << "   • Best for: Very large sparse systems" << std::endl;
    std::cout << "   • Advantage: Minimal memory, exploits sparsity\n" << std::endl;
    
    std::cout << "6. GAUSS-SEIDEL ITERATION" << std::endl;
    std::cout << "   • Simple iterative method" << std::endl;
    std::cout << "   • Complexity: O(kn²) for k iterations" << std::endl;
    std::cout << "   • Best for: Well-conditioned diagonally dominant systems" << std::endl;
    std::cout << "   • Advantage: Simple implementation, low memory\n" << std::endl;
    
    std::cout << "════════════════════════════════════════════════════════════════\n" << std::endl;
}

int main() {
    printHeader();
    printProblemStatement();
    printMethodology();
    
    // Problem parameters
    double width = 1.0;   // 1 m
    double height = 0.5;  // 0.5 m
    double thickness = 0.005;  // 5 mm
    
    StiffnessMatrix::MaterialProperties material;
    material.E = 210e9;   // 210 GPa
    material.nu = 0.3;
    material.t = thickness;
    
    // Mesh resolution
    int nx = 20;  // Elements in x-direction
    int ny = 10;  // Elements in y-direction
    
    std::cout << "\n════════════════════════════════════════════════════════════════" << std::endl;
    std::cout << "                  FINITE ELEMENT ANALYSIS" << std::endl;
    std::cout << "════════════════════════════════════════════════════════════════\n" << std::endl;
    
    // Create mesh
    StiffnessMatrix mesh(nx, ny, width, height, material);
    
    // Assemble global stiffness matrix
    mesh.assembleGlobalStiffness();
    
    // Boundary conditions
    BoundaryConditions bc(mesh);
    
    // Stress/strain analysis
    StressStrainAnalysis stressAnalysis(mesh);
    
    // Test all load cases
    std::vector<BoundaryConditions::LoadCase> loadCases = {
        BoundaryConditions::UNIFORM_5KN,
        BoundaryConditions::UNIFORM_10KN,
        BoundaryConditions::LINEAR_VARYING,
        BoundaryConditions::CONCENTRATED_15KN,
        BoundaryConditions::ANGLED_10KN
    };
    
    for (auto loadCase : loadCases) {
        std::cout << "\n\n" << std::string(80, '=') << std::endl;
        std::cout << "ANALYZING LOAD CASE: " 
                  << BoundaryConditions::getLoadCaseDescription(loadCase) << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Generate load vector
        std::vector<double> F;
        bc.generateLoadVector(F, loadCase);
        
        // Apply boundary conditions
        auto K = mesh.getGlobalK();
        bc.applyFixedBoundary(K, F);
        
        // Benchmark different solvers
        std::vector<Benchmark::Result> results;
        
        std::cout << "\n\n╔════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║          BENCHMARKING SOLVER METHODS               ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════╝\n" << std::endl;
        
        // 1. Naive Gaussian Elimination
        {
            std::cout << "\n┌─────────────────────────────────────────────────┐" << std::endl;
            std::cout << "│  Testing: Naive Gaussian Elimination            │" << std::endl;
            std::cout << "└─────────────────────────────────────────────────┘" << std::endl;
            
            auto K_copy = K;
            auto F_copy = F;
            
            Benchmark::Result result;
            result.solverName = NaiveGaussSolver::getName();
            
            Benchmark::start();
            result.solution = NaiveGaussSolver::solve(K_copy, F_copy, true);
            result.timeSeconds = Benchmark::stop();
            
            result.residual = Benchmark::computeResidual(K, result.solution, F);
            result.iterations = 0;
            result.memoryMB = (K.size() * K.size() * sizeof(double)) / (1024.0 * 1024.0);
            
            std::cout << "⏱  Execution time: " << std::fixed << std::setprecision(6) 
                      << result.timeSeconds << " seconds" << std::endl;
            std::cout << "📊 Residual norm: " << std::scientific << result.residual << std::endl;
            
            results.push_back(result);
        }
        
        // 2. LU Decomposition
        {
            std::cout << "\n┌─────────────────────────────────────────────────┐" << std::endl;
            std::cout << "│  Testing: LU Decomposition                      │" << std::endl;
            std::cout << "└─────────────────────────────────────────────────┘" << std::endl;
            
            auto K_copy = K;
            auto F_copy = F;
            
            Benchmark::Result result;
            result.solverName = LUSolver::getName();
            
            Benchmark::start();
            result.solution = LUSolver::solve(K_copy, F_copy, true);
            result.timeSeconds = Benchmark::stop();
            
            result.residual = Benchmark::computeResidual(K, result.solution, F);
            result.iterations = 0;
            result.memoryMB = (K.size() * K.size() * sizeof(double)) / (1024.0 * 1024.0);
            
            std::cout << "⏱  Execution time: " << std::fixed << std::setprecision(6) 
                      << result.timeSeconds << " seconds" << std::endl;
            std::cout << "📊 Residual norm: " << std::scientific << result.residual << std::endl;
            
            results.push_back(result);
        }
        
        // 3. Cholesky Decomposition
        {
            std::cout << "\n┌─────────────────────────────────────────────────┐" << std::endl;
            std::cout << "│  Testing: Cholesky Decomposition                │" << std::endl;
            std::cout << "└─────────────────────────────────────────────────┘" << std::endl;
            
            auto K_copy = K;
            auto F_copy = F;
            
            Benchmark::Result result;
            result.solverName = CholeskySolver::getName();
            
            Benchmark::start();
            result.solution = CholeskySolver::solve(K_copy, F_copy, true);
            result.timeSeconds = Benchmark::stop();
            
            result.residual = Benchmark::computeResidual(K, result.solution, F);
            result.iterations = 0;
            result.memoryMB = (K.size() * K.size() * sizeof(double)) / (1024.0 * 1024.0);
            
            std::cout << "⏱  Execution time: " << std::fixed << std::setprecision(6) 
                      << result.timeSeconds << " seconds" << std::endl;
            std::cout << "📊 Residual norm: " << std::scientific << result.residual << std::endl;
            
            results.push_back(result);
        }
        
        // 4. Conjugate Gradient
        {
            std::cout << "\n┌─────────────────────────────────────────────────┐" << std::endl;
            std::cout << "│  Testing: Conjugate Gradient                    │" << std::endl;
            std::cout << "└─────────────────────────────────────────────────┘" << std::endl;
            
            auto K_copy = K;
            auto F_copy = F;
            
            Benchmark::Result result;
            result.solverName = ConjugateGradientSolver::getName();
            
            Benchmark::start();
            result.solution = ConjugateGradientSolver::solve(K_copy, F_copy, true);
            result.timeSeconds = Benchmark::stop();
            
            result.residual = Benchmark::computeResidual(K, result.solution, F);
            result.iterations = 0;
            result.memoryMB = (K.size() * 3 * sizeof(double)) / (1024.0 * 1024.0);
            
            std::cout << "⏱  Execution time: " << std::fixed << std::setprecision(6) 
                      << result.timeSeconds << " seconds" << std::endl;
            std::cout << "📊 Residual norm: " << std::scientific << result.residual << std::endl;
            
            results.push_back(result);
        }
        
        // 5. Sparse Matrix Solver
        {
            std::cout << "\n┌─────────────────────────────────────────────────┐" << std::endl;
            std::cout << "│  Testing: Sparse Matrix CG                      │" << std::endl;
            std::cout << "└─────────────────────────────────────────────────┘" << std::endl;
            
            auto K_copy = K;
            auto F_copy = F;
            
            Benchmark::Result result;
            result.solverName = SparseSolver::getName();
            
            Benchmark::start();
            result.solution = SparseSolver::solve(K_copy, F_copy, true);
            result.timeSeconds = Benchmark::stop();
            
            result.residual = Benchmark::computeResidual(K, result.solution, F);
            result.iterations = 0;
            result.memoryMB = 0.0; // Calculated by sparse conversion
            
            std::cout << "⏱  Execution time: " << std::fixed << std::setprecision(6) 
                      << result.timeSeconds << " seconds" << std::endl;
            std::cout << "📊 Residual norm: " << std::scientific << result.residual << std::endl;
            
            results.push_back(result);
        }
        
        // 6. Gauss-Seidel
        {
            std::cout << "\n┌─────────────────────────────────────────────────┐" << std::endl;
            std::cout << "│  Testing: Gauss-Seidel Iteration                │" << std::endl;
            std::cout << "└─────────────────────────────────────────────────┘" << std::endl;
            
            auto K_copy = K;
            auto F_copy = F;
            
            Benchmark::Result result;
            result.solverName = GaussSeidelSolver::getName();
            
            Benchmark::start();
            result.solution = GaussSeidelSolver::solve(K_copy, F_copy, true);
            result.timeSeconds = Benchmark::stop();
            
            result.residual = Benchmark::computeResidual(K, result.solution, F);
            result.iterations = 0;
            result.memoryMB = (K.size() * 2 * sizeof(double)) / (1024.0 * 1024.0);
            
            std::cout << "⏱  Execution time: " << std::fixed << std::setprecision(6) 
                      << result.timeSeconds << " seconds" << std::endl;
            std::cout << "📊 Residual norm: " << std::scientific << result.residual << std::endl;
            
            results.push_back(result);
        }
        
        // Print comparison table
        Benchmark::printComparisonTable(results);
        
        // Compute and export stress/strain for best solution
        std::cout << "\n\n╔════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║        STRESS/STRAIN ANALYSIS                      ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════╝\n" << std::endl;
        
        auto stressResults = stressAnalysis.computeStressStrain(results[0].solution);
        stressAnalysis.printStatistics(stressResults);
        
        // Export results
        std::string caseStr = std::to_string(static_cast<int>(loadCase));
        stressAnalysis.exportResults(stressResults, 
                                     "results/stress_case" + caseStr + ".dat");
        Benchmark::exportBenchmarkResults(results, 
                                         "results/benchmark_case" + caseStr + ".dat");
    }
    
    std::cout << "\n\n╔════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    ANALYSIS COMPLETE                               ║" << std::endl;
    std::cout << "║                                                                    ║" << std::endl;
    std::cout << "║  All results have been exported to the 'results/' directory.       ║" << std::endl;
    std::cout << "║  Run the gnuplot scripts to generate visualization plots.         ║" << std::endl;
    std::cout << "║                                                                    ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n" << std::endl;
    
    return 0;
}
