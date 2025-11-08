#!/usr/bin/env python3
"""
Analysis script for FEM solver benchmark results
Generates summary statistics and comparison tables
"""

import os
import glob

def parse_benchmark_file(filename):
    """Parse a single benchmark file"""
    results = []
    with open(filename, 'r') as f:
        for line in f:
            if line.startswith('#') or not line.strip():
                continue
            parts = line.strip().split()
            if len(parts) >= 5:
                # Handle multi-word solver names
                if parts[0] == "Naive":
                    solver = "Naive Gaussian Elimination"
                    idx = 3
                elif parts[0] == "LU":
                    solver = "LU Decomposition"
                    idx = 2
                elif parts[0] == "Cholesky":
                    solver = "Cholesky Decomposition"
                    idx = 2
                elif parts[0] == "Conjugate":
                    solver = "Conjugate Gradient"
                    idx = 2
                elif parts[0] == "Sparse":
                    solver = "Sparse Matrix CG"
                    idx = 3
                elif parts[0] == "Gauss-Seidel":
                    solver = "Gauss-Seidel"
                    idx = 1
                else:
                    continue
                
                time = float(parts[idx])
                residual = float(parts[idx + 3])
                results.append({'solver': solver, 'time': time, 'residual': residual})
    return results

def main():
    # Find all benchmark files
    benchmark_files = sorted(glob.glob('results/benchmark_case*.dat'))
    
    if not benchmark_files:
        print("No benchmark files found!")
        return
    
    print("=" * 80)
    print("FEM SOLVER BENCHMARK ANALYSIS")
    print("=" * 80)
    print()
    
    # Collect all results
    all_results = {}
    for i, filename in enumerate(benchmark_files, 1):
        case = i
        results = parse_benchmark_file(filename)
        all_results[case] = results
        
        print(f"Load Case {case}: {os.path.basename(filename)}")
        print("-" * 80)
        print(f"{'Solver':<30} {'Time (s)':>12} {'Residual':>15}")
        print("-" * 80)
        
        for r in results:
            print(f"{r['solver']:<30} {r['time']:>12.6f} {r['residual']:>15.3e}")
        
        # Find fastest
        fastest = min(results, key=lambda x: x['time'])
        slowest = max(results, key=lambda x: x['time'])
        speedup = slowest['time'] / fastest['time']
        
        print("-" * 80)
        print(f"Fastest: {fastest['solver']} ({fastest['time']:.6f} s)")
        print(f"Slowest: {slowest['solver']} ({slowest['time']:.6f} s)")
        print(f"Speedup: {speedup:.1f}x")
        print()
    
    # Compute average performance across all cases
    print("=" * 80)
    print("AVERAGE PERFORMANCE ACROSS ALL LOAD CASES")
    print("=" * 80)
    print()
    
    solver_names = list(set(r['solver'] for results in all_results.values() for r in results))
    solver_stats = {}
    
    for solver in solver_names:
        times = []
        residuals = []
        for case_results in all_results.values():
            for r in case_results:
                if r['solver'] == solver:
                    times.append(r['time'])
                    residuals.append(r['residual'])
        
        if times:
            solver_stats[solver] = {
                'avg_time': sum(times) / len(times),
                'min_time': min(times),
                'max_time': max(times),
                'avg_residual': sum(residuals) / len(residuals)
            }
    
    # Sort by average time
    sorted_solvers = sorted(solver_stats.items(), key=lambda x: x[1]['avg_time'])
    
    print(f"{'Solver':<30} {'Avg Time (s)':>15} {'Min':>10} {'Max':>10} {'Avg Residual':>15}")
    print("-" * 90)
    
    for solver, stats in sorted_solvers:
        print(f"{solver:<30} {stats['avg_time']:>15.6f} {stats['min_time']:>10.6f} "
              f"{stats['max_time']:>10.6f} {stats['avg_residual']:>15.3e}")
    
    # Speedup analysis
    print()
    print("=" * 80)
    print("SPEEDUP ANALYSIS")
    print("=" * 80)
    print()
    
    fastest = sorted_solvers[0]
    slowest = sorted_solvers[-1]
    
    print(f"Fastest Method: {fastest[0]}")
    print(f"  Average Time: {fastest[1]['avg_time']:.6f} seconds")
    print()
    print(f"Slowest Method: {slowest[0]}")
    print(f"  Average Time: {slowest[1]['avg_time']:.6f} seconds")
    print()
    print(f"Overall Speedup: {slowest[1]['avg_time'] / fastest[1]['avg_time']:.1f}x")
    print()
    
    print("Speedup relative to fastest method:")
    print("-" * 60)
    for solver, stats in sorted_solvers:
        speedup = stats['avg_time'] / fastest[1]['avg_time']
        print(f"  {solver:<30} {speedup:>8.2f}x slower")
    
    # Save summary
    with open('results/performance_summary.txt', 'w') as f:
        f.write("FEM Solver Performance Summary\n")
        f.write("=" * 80 + "\n\n")
        f.write(f"{'Solver':<30} {'Avg Time (s)':>15} {'Speedup':>10} {'Residual':>15}\n")
        f.write("-" * 80 + "\n")
        
        for solver, stats in sorted_solvers:
            speedup = stats['avg_time'] / fastest[1]['avg_time']
            f.write(f"{solver:<30} {stats['avg_time']:>15.6f} {speedup:>10.2f}x "
                   f"{stats['avg_residual']:>15.3e}\n")
    
    print()
    print("Summary saved to: results/performance_summary.txt")

if __name__ == '__main__':
    main()
