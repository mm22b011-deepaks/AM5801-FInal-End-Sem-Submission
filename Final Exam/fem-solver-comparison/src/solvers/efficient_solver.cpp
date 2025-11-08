#include "efficient_solver.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>

// ============== LU DECOMPOSITION ==============

std::vector<double> LUSolver::solve(std::vector<std::vector<double>>& K, 
                                     std::vector<double>& F,
                                     bool verbose) {
    int n = F.size();
    
    if (verbose) {
        std::cout << "\n=== LU DECOMPOSITION ===" << std::endl;
        std::cout << "System size: " << n << " x " << n << std::endl;
    }
    
    auto A = K;
    std::vector<int> perm(n);
    for (int i = 0; i < n; i++) perm[i] = i;
    
    decompose(A, perm);
    auto solution = solveLU(A, perm, F);
    
    if (verbose) {
        std::cout << "Solution computed successfully" << std::endl;
    }
    
    return solution;
}

void LUSolver::decompose(std::vector<std::vector<double>>& A, std::vector<int>& perm) {
    int n = A.size();
    
    for (int k = 0; k < n; k++) {
        // Partial pivoting
        double maxVal = std::abs(A[k][k]);
        int maxRow = k;
        
        for (int i = k + 1; i < n; i++) {
            if (std::abs(A[i][k]) > maxVal) {
                maxVal = std::abs(A[i][k]);
                maxRow = i;
            }
        }
        
        if (maxRow != k) {
            std::swap(A[k], A[maxRow]);
            std::swap(perm[k], perm[maxRow]);
        }
        
        // LU decomposition
        for (int i = k + 1; i < n; i++) {
            if (std::abs(A[k][k]) > 1e-15) {
                A[i][k] /= A[k][k];
                
                for (int j = k + 1; j < n; j++) {
                    A[i][j] -= A[i][k] * A[k][j];
                }
            }
        }
    }
}

std::vector<double> LUSolver::solveLU(const std::vector<std::vector<double>>& LU,
                                       const std::vector<int>& perm,
                                       const std::vector<double>& b) {
    int n = b.size();
    std::vector<double> y(n), x(n);
    
    // Forward substitution (Ly = Pb)
    for (int i = 0; i < n; i++) {
        double sum = b[perm[i]];
        for (int j = 0; j < i; j++) {
            sum -= LU[i][j] * y[j];
        }
        y[i] = sum;
    }
    
    // Back substitution (Ux = y)
    for (int i = n - 1; i >= 0; i--) {
        double sum = y[i];
        for (int j = i + 1; j < n; j++) {
            sum -= LU[i][j] * x[j];
        }
        x[i] = sum / LU[i][i];
    }
    
    return x;
}

// ============== CHOLESKY DECOMPOSITION ==============

std::vector<double> CholeskySolver::solve(std::vector<std::vector<double>>& K, 
                                           std::vector<double>& F,
                                           bool verbose) {
    int n = F.size();
    
    if (verbose) {
        std::cout << "\n=== CHOLESKY DECOMPOSITION ===" << std::endl;
        std::cout << "System size: " << n << " x " << n << std::endl;
    }
    
    auto A = K;
    
    if (!decompose(A)) {
        std::cerr << "Cholesky decomposition failed (matrix not positive definite)" << std::endl;
        return std::vector<double>(n, 0.0);
    }
    
    auto solution = solveCholesky(A, F);
    
    if (verbose) {
        std::cout << "Solution computed successfully" << std::endl;
    }
    
    return solution;
}

bool CholeskySolver::decompose(std::vector<std::vector<double>>& A) {
    int n = A.size();
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = A[i][j];
            
            for (int k = 0; k < j; k++) {
                sum -= A[i][k] * A[j][k];
            }
            
            if (i == j) {
                if (sum <= 0.0) {
                    return false;  // Not positive definite
                }
                A[i][j] = sqrt(sum);
            } else {
                if (std::abs(A[j][j]) < 1e-15) {
                    return false;
                }
                A[i][j] = sum / A[j][j];
            }
        }
    }
    
    return true;
}

std::vector<double> CholeskySolver::solveCholesky(const std::vector<std::vector<double>>& L,
                                                   const std::vector<double>& b) {
    int n = b.size();
    std::vector<double> y(n), x(n);
    
    // Forward substitution (Ly = b)
    for (int i = 0; i < n; i++) {
        double sum = b[i];
        for (int j = 0; j < i; j++) {
            sum -= L[i][j] * y[j];
        }
        y[i] = sum / L[i][i];
    }
    
    // Back substitution (L^T x = y)
    for (int i = n - 1; i >= 0; i--) {
        double sum = y[i];
        for (int j = i + 1; j < n; j++) {
            sum -= L[j][i] * x[j];
        }
        x[i] = sum / L[i][i];
    }
    
    return x;
}

// ============== CONJUGATE GRADIENT ==============

std::vector<double> ConjugateGradientSolver::solve(std::vector<std::vector<double>>& K, 
                                                     std::vector<double>& F,
                                                     bool verbose,
                                                     double tolerance,
                                                     int maxIterations) {
    int n = F.size();
    
    if (verbose) {
        std::cout << "\n=== CONJUGATE GRADIENT ===" << std::endl;
        std::cout << "System size: " << n << " x " << n << std::endl;
        std::cout << "Tolerance: " << tolerance << ", Max iterations: " << maxIterations << std::endl;
    }
    
    std::vector<double> x(n, 0.0);
    std::vector<double> r = F;
    std::vector<double> p = r;
    std::vector<double> Ap(n);
    
    double rsold = dotProduct(r, r);
    double initialResidual = sqrt(rsold);
    
    int iter = 0;
    for (iter = 0; iter < maxIterations; iter++) {
        matrixVectorMultiply(K, p, Ap);
        
        double alpha = rsold / dotProduct(p, Ap);
        
        for (int i = 0; i < n; i++) {
            x[i] += alpha * p[i];
            r[i] -= alpha * Ap[i];
        }
        
        double rsnew = dotProduct(r, r);
        double residual = sqrt(rsnew);
        
        if (verbose && iter % 100 == 0) {
            std::cout << "Iteration " << iter << ", Residual: " << residual << std::endl;
        }
        
        if (residual < tolerance) {
            if (verbose) {
                std::cout << "Converged at iteration " << iter << std::endl;
                std::cout << "Final residual: " << residual << std::endl;
            }
            break;
        }
        
        double beta = rsnew / rsold;
        for (int i = 0; i < n; i++) {
            p[i] = r[i] + beta * p[i];
        }
        
        rsold = rsnew;
    }
    
    if (iter == maxIterations && verbose) {
        std::cout << "Warning: Maximum iterations reached" << std::endl;
    }
    
    return x;
}

void ConjugateGradientSolver::matrixVectorMultiply(const std::vector<std::vector<double>>& A,
                                                    const std::vector<double>& x,
                                                    std::vector<double>& result) {
    int n = x.size();
    for (int i = 0; i < n; i++) {
        result[i] = 0.0;
        for (int j = 0; j < n; j++) {
            result[i] += A[i][j] * x[j];
        }
    }
}

double ConjugateGradientSolver::dotProduct(const std::vector<double>& a, 
                                           const std::vector<double>& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

// ============== SPARSE MATRIX SOLVER ==============

std::vector<double> SparseSolver::solve(std::vector<std::vector<double>>& K, 
                                         std::vector<double>& F,
                                         bool verbose) {
    if (verbose) {
        std::cout << "\n=== SPARSE MATRIX SOLVER (CG) ===" << std::endl;
        std::cout << "System size: " << K.size() << " x " << K.size() << std::endl;
    }
    
    auto sparseK = convertToSparse(K);
    
    if (verbose) {
        double sparsity = 100.0 * (1.0 - (double)sparseK.values.size() / (K.size() * K.size()));
        std::cout << "Sparsity: " << std::fixed << std::setprecision(2) << sparsity << "%" << std::endl;
        std::cout << "Non-zero elements: " << sparseK.values.size() << std::endl;
    }
    
    auto solution = sparseCG(sparseK, F, verbose);
    
    return solution;
}

SparseSolver::SparseMatrix SparseSolver::convertToSparse(const std::vector<std::vector<double>>& K,
                                                          double threshold) {
    SparseMatrix sparse;
    sparse.n = K.size();
    sparse.rowPointers.push_back(0);
    
    for (int i = 0; i < sparse.n; i++) {
        for (int j = 0; j < sparse.n; j++) {
            if (std::abs(K[i][j]) > threshold) {
                sparse.values.push_back(K[i][j]);
                sparse.colIndices.push_back(j);
            }
        }
        sparse.rowPointers.push_back(sparse.values.size());
    }
    
    return sparse;
}

void SparseSolver::sparseMatVecMult(const SparseMatrix& A, const std::vector<double>& x,
                                    std::vector<double>& result) {
    for (int i = 0; i < A.n; i++) {
        result[i] = 0.0;
        for (int j = A.rowPointers[i]; j < A.rowPointers[i + 1]; j++) {
            result[i] += A.values[j] * x[A.colIndices[j]];
        }
    }
}

std::vector<double> SparseSolver::sparseCG(const SparseMatrix& A, 
                                           const std::vector<double>& b,
                                           bool verbose) {
    int n = A.n;
    std::vector<double> x(n, 0.0);
    std::vector<double> r = b;
    std::vector<double> p = r;
    std::vector<double> Ap(n);
    
    double rsold = ConjugateGradientSolver::dotProduct(r, r);
    double tolerance = 1e-8;
    int maxIter = 10000;
    
    for (int iter = 0; iter < maxIter; iter++) {
        sparseMatVecMult(A, p, Ap);
        
        double alpha = rsold / ConjugateGradientSolver::dotProduct(p, Ap);
        
        for (int i = 0; i < n; i++) {
            x[i] += alpha * p[i];
            r[i] -= alpha * Ap[i];
        }
        
        double rsnew = ConjugateGradientSolver::dotProduct(r, r);
        double residual = sqrt(rsnew);
        
        if (verbose && iter % 100 == 0) {
            std::cout << "Iteration " << iter << ", Residual: " << residual << std::endl;
        }
        
        if (residual < tolerance) {
            if (verbose) {
                std::cout << "Converged at iteration " << iter << std::endl;
            }
            break;
        }
        
        double beta = rsnew / rsold;
        for (int i = 0; i < n; i++) {
            p[i] = r[i] + beta * p[i];
        }
        
        rsold = rsnew;
    }
    
    return x;
}

// ============== GAUSS-SEIDEL ==============

std::vector<double> GaussSeidelSolver::solve(std::vector<std::vector<double>>& K, 
                                              std::vector<double>& F,
                                              bool verbose,
                                              double tolerance,
                                              int maxIterations) {
    int n = F.size();
    
    if (verbose) {
        std::cout << "\n=== GAUSS-SEIDEL ITERATIVE SOLVER ===" << std::endl;
        std::cout << "System size: " << n << " x " << n << std::endl;
    }
    
    std::vector<double> x(n, 0.0);
    std::vector<double> x_old(n);
    
    for (int iter = 0; iter < maxIterations; iter++) {
        x_old = x;
        
        for (int i = 0; i < n; i++) {
            double sum = F[i];
            
            for (int j = 0; j < i; j++) {
                sum -= K[i][j] * x[j];
            }
            
            for (int j = i + 1; j < n; j++) {
                sum -= K[i][j] * x_old[j];
            }
            
            if (std::abs(K[i][i]) > 1e-15) {
                x[i] = sum / K[i][i];
            }
        }
        
        // Check convergence
        double error = 0.0;
        for (int i = 0; i < n; i++) {
            error += (x[i] - x_old[i]) * (x[i] - x_old[i]);
        }
        error = sqrt(error);
        
        if (verbose && iter % 100 == 0) {
            std::cout << "Iteration " << iter << ", Error: " << error << std::endl;
        }
        
        if (error < tolerance) {
            if (verbose) {
                std::cout << "Converged at iteration " << iter << std::endl;
            }
            break;
        }
    }
    
    return x;
}
