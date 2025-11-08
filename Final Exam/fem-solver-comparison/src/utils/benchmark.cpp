#include "benchmark.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>

std::chrono::high_resolution_clock::time_point Benchmark::startTime;

void Benchmark::start() {
    startTime = std::chrono::high_resolution_clock::now();
}

double Benchmark::stop() {
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = endTime - startTime;
    return diff.count();
}

double Benchmark::compareSolutions(const std::vector<double>& sol1,
                                   const std::vector<double>& sol2) {
    if (sol1.size() != sol2.size()) {
        std::cerr << "Error: Solutions have different sizes" << std::endl;
        return -1.0;
    }
    
    double diff = 0.0;
    for (size_t i = 0; i < sol1.size(); i++) {
        double d = sol1[i] - sol2[i];
        diff += d * d;
    }
    
    return sqrt(diff);
}

double Benchmark::solutionNorm(const std::vector<double>& sol) {
    double norm = 0.0;
    for (double val : sol) {
        norm += val * val;
    }
    return sqrt(norm);
}

double Benchmark::relativeError(const std::vector<double>& computed,
                               const std::vector<double>& reference) {
    double error = compareSolutions(computed, reference);
    double refNorm = solutionNorm(reference);
    
    if (refNorm < 1e-15) {
        return error;
    }
    
    return error / refNorm;
}

double Benchmark::computeResidual(const std::vector<std::vector<double>>& K,
                                 const std::vector<double>& U,
                                 const std::vector<double>& F) {
    int n = U.size();
    std::vector<double> KU(n, 0.0);
    
    // Compute K*U
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            KU[i] += K[i][j] * U[j];
        }
    }
    
    // Compute ||K*U - F||
    double residual = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = KU[i] - F[i];
        residual += diff * diff;
    }
    
    return sqrt(residual);
}

void Benchmark::exportBenchmarkResults(const std::vector<Result>& results,
                                      const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    file << "# Solver benchmark results" << std::endl;
    file << "# Solver Time(s) Memory(MB) Iterations Residual" << std::endl;
    
    for (const auto& r : results) {
        file << r.solverName << " "
             << r.timeSeconds << " "
             << r.memoryMB << " "
             << r.iterations << " "
             << r.residual << std::endl;
    }
    
    file.close();
    std::cout << "Benchmark results exported to: " << filename << std::endl;
}

void Benchmark::printComparisonTable(const std::vector<Result>& results) {
    if (results.empty()) return;
    
    std::cout << "\n" << std::string(90, '=') << std::endl;
    std::cout << "SOLVER PERFORMANCE COMPARISON" << std::endl;
    std::cout << std::string(90, '=') << std::endl;
    
    std::cout << std::left << std::setw(30) << "Solver" 
              << std::right << std::setw(15) << "Time (s)"
              << std::setw(15) << "Speedup"
              << std::setw(15) << "Residual"
              << std::setw(15) << "Error" << std::endl;
    std::cout << std::string(90, '-') << std::endl;
    
    // Find slowest time for speedup calculation
    double slowestTime = 0.0;
    for (const auto& r : results) {
        if (r.timeSeconds > slowestTime) slowestTime = r.timeSeconds;
    }
    
    // Reference solution (first solver)
    const auto& reference = results[0].solution;
    
    for (const auto& r : results) {
        double speedup = slowestTime / r.timeSeconds;
        double error = relativeError(r.solution, reference);
        
        std::cout << std::left << std::setw(30) << r.solverName
                  << std::right << std::fixed << std::setprecision(6)
                  << std::setw(15) << r.timeSeconds
                  << std::setw(15) << std::setprecision(2) << speedup << "x"
                  << std::setw(15) << std::scientific << std::setprecision(3) << r.residual
                  << std::setw(15) << error << std::endl;
    }
    
    std::cout << std::string(90, '=') << std::endl;
    
    // Find fastest solver
    auto fastest = std::min_element(results.begin(), results.end(),
                                   [](const Result& a, const Result& b) {
                                       return a.timeSeconds < b.timeSeconds;
                                   });
    
    std::cout << "\n✓ FASTEST SOLVER: " << fastest->solverName 
              << " (" << std::fixed << std::setprecision(6) 
              << fastest->timeSeconds << " seconds)" << std::endl;
    
    // Performance improvement
    double improvement = (slowestTime / fastest->timeSeconds - 1.0) * 100.0;
    std::cout << "✓ PERFORMANCE IMPROVEMENT: " << std::fixed << std::setprecision(1)
              << improvement << "% faster than slowest method" << std::endl;
}
