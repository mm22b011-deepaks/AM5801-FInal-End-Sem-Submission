#ifndef NAIVE_GAUSS_H
#define NAIVE_GAUSS_H

#include <vector>
#include <string>

class NaiveGaussSolver {
public:
    // Solve [K]{U} = {F} using naive Gaussian elimination
    static std::vector<double> solve(std::vector<std::vector<double>>& K, 
                                     std::vector<double>& F,
                                     bool verbose = false);
    
    // Forward elimination
    static void forwardElimination(std::vector<std::vector<double>>& A, 
                                   std::vector<double>& b);
    
    // Back substitution
    static std::vector<double> backSubstitution(std::vector<std::vector<double>>& A, 
                                               std::vector<double>& b);
    
    // Check for numerical stability
    static double checkConditionNumber(const std::vector<std::vector<double>>& K);
    
    static std::string getName() { return "Naive Gaussian Elimination"; }
};

#endif
