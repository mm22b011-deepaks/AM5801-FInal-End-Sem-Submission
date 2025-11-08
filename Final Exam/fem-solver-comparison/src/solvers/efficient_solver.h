#ifndef EFFICIENT_SOLVER_H
#define EFFICIENT_SOLVER_H

#include <vector>
#include <string>

// LU Decomposition Solver
class LUSolver {
public:
    static std::vector<double> solve(std::vector<std::vector<double>>& K, 
                                     std::vector<double>& F,
                                     bool verbose = false);
    
    static void decompose(std::vector<std::vector<double>>& A, 
                         std::vector<int>& permutation);
    
    static std::vector<double> solveLU(const std::vector<std::vector<double>>& LU,
                                       const std::vector<int>& perm,
                                       const std::vector<double>& b);
    
    static std::string getName() { return "LU Decomposition"; }
};

// Cholesky Decomposition Solver
class CholeskySolver {
public:
    static std::vector<double> solve(std::vector<std::vector<double>>& K, 
                                     std::vector<double>& F,
                                     bool verbose = false);
    
    static bool decompose(std::vector<std::vector<double>>& A);
    
    static std::vector<double> solveCholesky(const std::vector<std::vector<double>>& L,
                                            const std::vector<double>& b);
    
    static std::string getName() { return "Cholesky Decomposition"; }
};

// Conjugate Gradient Solver
class ConjugateGradientSolver {
public:
    static std::vector<double> solve(std::vector<std::vector<double>>& K, 
                                     std::vector<double>& F,
                                     bool verbose = false,
                                     double tolerance = 1e-8,
                                     int maxIterations = 10000);
    
    static void matrixVectorMultiply(const std::vector<std::vector<double>>& A,
                                    const std::vector<double>& x,
                                    std::vector<double>& result);
    
    static double dotProduct(const std::vector<double>& a, const std::vector<double>& b);
    
    static std::string getName() { return "Conjugate Gradient"; }
};

// Sparse Matrix Solver (using Compressed Row Storage)
class SparseSolver {
public:
    struct SparseMatrix {
        std::vector<double> values;
        std::vector<int> colIndices;
        std::vector<int> rowPointers;
        int n;
    };
    
    static std::vector<double> solve(std::vector<std::vector<double>>& K, 
                                     std::vector<double>& F,
                                     bool verbose = false);
    
    static SparseMatrix convertToSparse(const std::vector<std::vector<double>>& K,
                                       double threshold = 1e-12);
    
    static void sparseMatVecMult(const SparseMatrix& A, const std::vector<double>& x,
                                std::vector<double>& result);
    
    static std::vector<double> sparseCG(const SparseMatrix& A, 
                                       const std::vector<double>& b,
                                       bool verbose = false);
    
    static std::string getName() { return "Sparse Matrix CG"; }
};

// Gauss-Seidel Iterative Solver
class GaussSeidelSolver {
public:
    static std::vector<double> solve(std::vector<std::vector<double>>& K, 
                                     std::vector<double>& F,
                                     bool verbose = false,
                                     double tolerance = 1e-8,
                                     int maxIterations = 10000);
    
    static std::string getName() { return "Gauss-Seidel"; }
};

#endif
