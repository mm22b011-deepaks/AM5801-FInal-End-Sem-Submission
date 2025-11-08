#include "naive_gauss.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include <algorithm>

std::vector<double> NaiveGaussSolver::solve(std::vector<std::vector<double>>& K, 
                                             std::vector<double>& F,
                                             bool verbose) {
    int n = F.size();
    
    if (verbose) {
        std::cout << "\n=== NAIVE GAUSSIAN ELIMINATION ===" << std::endl;
        std::cout << "System size: " << n << " x " << n << std::endl;
    }
    
    // Make copies to avoid modifying original
    std::vector<std::vector<double>> A = K;
    std::vector<double> b = F;
    
    // Forward elimination
    forwardElimination(A, b);
    
    // Back substitution
    auto solution = backSubstitution(A, b);
    
    if (verbose) {
        std::cout << "Solution computed successfully" << std::endl;
    }
    
    return solution;
}

void NaiveGaussSolver::forwardElimination(std::vector<std::vector<double>>& A, 
                                          std::vector<double>& b) {
    int n = b.size();
    
    for (int k = 0; k < n - 1; k++) {
        // Find pivot
        double maxVal = std::abs(A[k][k]);
        int maxRow = k;
        
        for (int i = k + 1; i < n; i++) {
            if (std::abs(A[i][k]) > maxVal) {
                maxVal = std::abs(A[i][k]);
                maxRow = i;
            }
        }
        
        // Swap rows if needed (partial pivoting)
        if (maxRow != k) {
            std::swap(A[k], A[maxRow]);
            std::swap(b[k], b[maxRow]);
        }
        
        // Check for singular matrix
        if (std::abs(A[k][k]) < 1e-15) {
            std::cerr << "Warning: Near-singular matrix at row " << k << std::endl;
            continue;
        }
        
        // Eliminate column k
        for (int i = k + 1; i < n; i++) {
            double factor = A[i][k] / A[k][k];
            b[i] -= factor * b[k];
            
            for (int j = k; j < n; j++) {
                A[i][j] -= factor * A[k][j];
            }
        }
    }
}

std::vector<double> NaiveGaussSolver::backSubstitution(std::vector<std::vector<double>>& A, 
                                                       std::vector<double>& b) {
    int n = b.size();
    std::vector<double> x(n, 0.0);
    
    for (int i = n - 1; i >= 0; i--) {
        double sum = b[i];
        
        for (int j = i + 1; j < n; j++) {
            sum -= A[i][j] * x[j];
        }
        
        if (std::abs(A[i][i]) < 1e-15) {
            std::cerr << "Warning: Near-zero diagonal at row " << i << std::endl;
            x[i] = 0.0;
        } else {
            x[i] = sum / A[i][i];
        }
    }
    
    return x;
}

double NaiveGaussSolver::checkConditionNumber(const std::vector<std::vector<double>>& K) {
    // Simple estimate: ratio of max to min diagonal elements
    int n = K.size();
    double maxDiag = 0.0, minDiag = 1e100;
    
    for (int i = 0; i < n; i++) {
        double val = std::abs(K[i][i]);
        if (val > maxDiag) maxDiag = val;
        if (val < minDiag && val > 1e-15) minDiag = val;
    }
    
    return maxDiag / minDiag;
}
