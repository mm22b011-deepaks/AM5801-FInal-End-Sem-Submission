#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <string>
#include <vector>
#include <functional>

class Benchmark {
public:
    struct Result {
        std::string solverName;
        double timeSeconds;
        double memoryMB;
        int iterations;
        double residual;
        std::vector<double> solution;
    };
    
    // Start timer
    static void start();
    
    // Stop timer and return elapsed time in seconds
    static double stop();
    
    // Measure execution time of a function
    template<typename Func>
    static double measureTime(Func func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        return diff.count();
    }
    
    // Compare solutions
    static double compareSolutions(const std::vector<double>& sol1,
                                   const std::vector<double>& sol2);
    
    // Compute solution norm
    static double solutionNorm(const std::vector<double>& sol);
    
    // Compute relative error
    static double relativeError(const std::vector<double>& computed,
                               const std::vector<double>& reference);
    
    // Compute residual ||K*U - F||
    static double computeResidual(const std::vector<std::vector<double>>& K,
                                 const std::vector<double>& U,
                                 const std::vector<double>& F);
    
    // Export benchmark results
    static void exportBenchmarkResults(const std::vector<Result>& results,
                                      const std::string& filename);
    
    // Print comparison table
    static void printComparisonTable(const std::vector<Result>& results);
    
private:
    static std::chrono::high_resolution_clock::time_point startTime;
};

#endif
