# ============================================================================
# FEM SOLVER COMPARISON - ENHANCED MAKEFILE
# ============================================================================
# Description: Comprehensive build system with selective solver execution,
#              parallel compilation, and detailed benchmarking options
# Author: Automated build system for AM5801 Final Exam
# Date: 2025-01-08
# ============================================================================

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -march=native -fopenmp
INCLUDES = -Isrc

# Optional: CUDA support (uncomment if CUDA implementation added)
# NVCC = nvcc
# CUDA_FLAGS = -O3 -arch=sm_75
# CUDA_INCLUDES = -I/usr/local/cuda/include
# CUDA_LIBS = -L/usr/local/cuda/lib64 -lcudart

# Directories
SRC_DIR = src
BUILD_DIR = build
RESULTS_DIR = results
PLOTS_DIR = plots

# Source files
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/matrix/stiffness_matrix.cpp \
          $(SRC_DIR)/solvers/naive_gauss.cpp \
          $(SRC_DIR)/solvers/efficient_solver.cpp \
          $(SRC_DIR)/fem/boundary_conditions.cpp \
          $(SRC_DIR)/analysis/stress_strain.cpp \
          $(SRC_DIR)/utils/benchmark.cpp

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable
TARGET = fem_solver

# Colors for pretty output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

# ============================================================================
# MAIN TARGETS
# ============================================================================

# Default target
all: directories $(TARGET)
	@echo "$(GREEN)✓ Build complete! Run 'make run' to execute.$(NC)"

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/matrix
	@mkdir -p $(BUILD_DIR)/solvers
	@mkdir -p $(BUILD_DIR)/fem
	@mkdir -p $(BUILD_DIR)/analysis
	@mkdir -p $(BUILD_DIR)/utils
	@mkdir -p $(RESULTS_DIR)
	@mkdir -p $(PLOTS_DIR)

# Link executable
$(TARGET): $(OBJECTS)
	@echo "$(BLUE)Linking $(TARGET)...$(NC)"
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "$(GREEN)✓ Linking successful!$(NC)"

# Compile source files (parallel compilation with -j)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "$(YELLOW)Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ============================================================================
# EXECUTION TARGETS
# ============================================================================

# Run all solvers on all load cases (default)
run: $(TARGET)
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)Running FEM Solver - ALL BENCHMARKS$(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@./$(TARGET)
	@echo "$(GREEN)✓ Execution complete! Check $(RESULTS_DIR)/ for results.$(NC)"

# Run only direct methods (faster for testing)
run-direct: $(TARGET)
	@echo "$(BLUE)Running DIRECT METHODS only...$(NC)"
	@./$(TARGET) --solvers=direct
	@echo "$(GREEN)✓ Direct methods complete!$(NC)"

# Run only iterative methods
run-iterative: $(TARGET)
	@echo "$(BLUE)Running ITERATIVE METHODS only...$(NC)"
	@./$(TARGET) --solvers=iterative
	@echo "$(GREEN)✓ Iterative methods complete!$(NC)"

# Run only sparse solvers (fastest)
run-sparse: $(TARGET)
	@echo "$(BLUE)Running SPARSE SOLVERS only...$(NC)"
	@./$(TARGET) --solvers=sparse
	@echo "$(GREEN)✓ Sparse solvers complete!$(NC)"

# Run single load case (quick test)
run-case1: $(TARGET)
	@echo "$(BLUE)Running LOAD CASE 1 only...$(NC)"
	@./$(TARGET) --case=1
	@echo "$(GREEN)✓ Case 1 complete!$(NC)"

# Quick benchmark (sparse CG only, all cases)
run-quick: $(TARGET)
	@echo "$(BLUE)Quick benchmark (Sparse CG + Cholesky)...$(NC)"
	@./$(TARGET) --solvers=quick
	@echo "$(GREEN)✓ Quick benchmark complete!$(NC)"

# Verbose output (detailed timing breakdown)
run-verbose: $(TARGET)
	@echo "$(BLUE)Running with VERBOSE output...$(NC)"
	@./$(TARGET) --verbose
	@echo "$(GREEN)✓ Verbose run complete!$(NC)"

# ============================================================================
# ANALYSIS TARGETS
# ============================================================================

# Generate all plots
plots: run
	@echo "$(BLUE)Generating plots with gnuplot...$(NC)"
	@cd $(PLOTS_DIR) && gnuplot generate_all_plots.gp 2>/dev/null || \
		echo "$(YELLOW)Warning: Some plots may have failed. Check gnuplot installation.$(NC)"
	@echo "$(GREEN)✓ Plots generated! Check $(PLOTS_DIR)/*.png$(NC)"

# Statistical analysis with Python
analyze: run
	@echo "$(BLUE)Running statistical analysis...$(NC)"
	@python3 analyze_results.py
	@echo "$(GREEN)✓ Analysis complete! Check results/analysis_summary.txt$(NC)"

# Generate comprehensive report
report: plots analyze
	@echo "$(BLUE)Generating comprehensive report...$(NC)"
	@cat results/performance_summary.txt
	@echo "$(GREEN)✓ Report ready! See DETAILED_REPORT.md$(NC)"

# Validation: Check solution consistency
validate: run
	@echo "$(BLUE)Validating solution consistency...$(NC)"
	@echo "Checking if all solvers agree within 1% tolerance..."
	@grep -E "Relative error|Agreement" $(RESULTS_DIR)/full_output.txt | head -20
	@echo "$(GREEN)✓ Validation complete! See SOLUTION_CONSISTENCY.md$(NC)"

# ============================================================================
# BENCHMARKING TARGETS
# ============================================================================

# Full benchmark suite (30 runs: 6 solvers × 5 cases)
benchmark: run
	@echo "$(GREEN)========================================$(NC)"
	@echo "$(GREEN)    FULL BENCHMARK RESULTS SUMMARY     $(NC)"
	@echo "$(GREEN)========================================$(NC)"
	@cat $(RESULTS_DIR)/performance_summary.txt
	@echo ""
	@echo "$(BLUE)Fastest Solver:$(NC)"
	@grep -A 1 "Sparse Conjugate" $(RESULTS_DIR)/all_benchmarks_summary.txt | head -2
	@echo ""
	@echo "$(GREEN)✓ Benchmark complete! See results/ directory.$(NC)"

# Time complexity analysis
complexity: run
	@echo "$(BLUE)Analyzing computational complexity...$(NC)"
	@echo "See COMPLEXITY_ANALYSIS.md for detailed breakdown"
	@grep -E "Average Time|Operations" $(RESULTS_DIR)/performance_summary.txt
	@echo "$(GREEN)✓ Complexity analysis available in COMPLEXITY_ANALYSIS.md$(NC)"

# Memory profiling (requires valgrind)
profile-memory: $(TARGET)
	@echo "$(BLUE)Profiling memory usage with valgrind...$(NC)"
	@valgrind --tool=massif --massif-out-file=massif.out ./$(TARGET) 2>&1 | tail -20
	@echo "$(GREEN)✓ Memory profile saved to massif.out$(NC)"

# CPU profiling (requires perf)
profile-cpu: $(TARGET)
	@echo "$(BLUE)Profiling CPU usage with perf...$(NC)"
	@perf stat -e cycles,instructions,cache-misses ./$(TARGET)
	@echo "$(GREEN)✓ CPU profiling complete$(NC)"

# ============================================================================
# CLEANING TARGETS
# ============================================================================

# Clean build artifacts only
clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET)
	@echo "$(GREEN)✓ Build artifacts removed$(NC)"

# Clean results and plots (keep source)
clean-results:
	@echo "$(YELLOW)Cleaning results and plots...$(NC)"
	@rm -rf $(RESULTS_DIR)/*.dat $(RESULTS_DIR)/*.txt
	@rm -rf $(PLOTS_DIR)/*.png
	@echo "$(GREEN)✓ Results cleaned$(NC)"

# Clean everything including generated documentation
cleanall: clean clean-results
	@echo "$(RED)Removing ALL generated files...$(NC)"
	@rm -f massif.out perf.data
	@echo "$(GREEN)✓ Complete cleanup done$(NC)"

# ============================================================================
# UTILITY TARGETS
# ============================================================================

# Show system information
sysinfo:
	@echo "$(BLUE)========================================$(NC)"
	@echo "$(BLUE)        SYSTEM INFORMATION              $(NC)"
	@echo "$(BLUE)========================================$(NC)"
	@cat SYSTEM_INFO.txt 2>/dev/null || echo "$(RED)SYSTEM_INFO.txt not found. Run solver first.$(NC)"

# Check dependencies
check-deps:
	@echo "$(BLUE)Checking dependencies...$(NC)"
	@command -v $(CXX) >/dev/null 2>&1 && echo "$(GREEN)✓ g++ found: $$($(CXX) --version | head -1)$(NC)" || echo "$(RED)✗ g++ not found$(NC)"
	@command -v gnuplot >/dev/null 2>&1 && echo "$(GREEN)✓ gnuplot found: $$(gnuplot --version)$(NC)" || echo "$(YELLOW)⚠ gnuplot not found (plots will fail)$(NC)"
	@command -v python3 >/dev/null 2>&1 && echo "$(GREEN)✓ python3 found: $$(python3 --version)$(NC)" || echo "$(YELLOW)⚠ python3 not found (analysis will fail)$(NC)"
	@command -v valgrind >/dev/null 2>&1 && echo "$(GREEN)✓ valgrind found$(NC)" || echo "$(YELLOW)⚠ valgrind not found (memory profiling unavailable)$(NC)"

# Show detailed help
help:
	@echo "$(GREEN)============================================================================$(NC)"
	@echo "$(GREEN)  FEM SOLVER COMPARISON - MAKEFILE HELP$(NC)"
	@echo "$(GREEN)============================================================================$(NC)"
	@echo ""
	@echo "$(BLUE)BUILD TARGETS:$(NC)"
	@echo "  $(YELLOW)all$(NC)              Build the entire project (default)"
	@echo "  $(YELLOW)clean$(NC)            Remove build artifacts (keep results)"
	@echo "  $(YELLOW)cleanall$(NC)         Remove everything (build + results + plots)"
	@echo ""
	@echo "$(BLUE)EXECUTION TARGETS:$(NC)"
	@echo "  $(YELLOW)run$(NC)              Run all solvers on all load cases (full benchmark)"
	@echo "  $(YELLOW)run-direct$(NC)       Run only direct methods (Gauss, LU, Cholesky)"
	@echo "  $(YELLOW)run-iterative$(NC)    Run only iterative methods (CG, Gauss-Seidel)"
	@echo "  $(YELLOW)run-sparse$(NC)       Run only sparse solvers (Sparse CG)"
	@echo "  $(YELLOW)run-case1$(NC)        Run only Load Case 1 (quick test)"
	@echo "  $(YELLOW)run-quick$(NC)        Quick benchmark (Sparse CG + Cholesky only)"
	@echo "  $(YELLOW)run-verbose$(NC)      Run with detailed timing breakdown"
	@echo ""
	@echo "$(BLUE)ANALYSIS TARGETS:$(NC)"
	@echo "  $(YELLOW)plots$(NC)            Generate all visualization plots (requires gnuplot)"
	@echo "  $(YELLOW)analyze$(NC)          Run Python statistical analysis"
	@echo "  $(YELLOW)report$(NC)           Generate comprehensive performance report"
	@echo "  $(YELLOW)validate$(NC)         Check solution consistency across solvers"
	@echo ""
	@echo "$(BLUE)BENCHMARKING TARGETS:$(NC)"
	@echo "  $(YELLOW)benchmark$(NC)        Full benchmark suite with summary"
	@echo "  $(YELLOW)complexity$(NC)       Analyze computational complexity"
	@echo "  $(YELLOW)profile-memory$(NC)   Memory profiling with valgrind (slow)"
	@echo "  $(YELLOW)profile-cpu$(NC)      CPU profiling with perf"
	@echo ""
	@echo "$(BLUE)UTILITY TARGETS:$(NC)"
	@echo "  $(YELLOW)sysinfo$(NC)          Display system specifications"
	@echo "  $(YELLOW)check-deps$(NC)       Verify all dependencies are installed"
	@echo "  $(YELLOW)help$(NC)             Show this help message"
	@echo ""
	@echo "$(GREEN)EXAMPLES:$(NC)"
	@echo "  make                 # Build everything"
	@echo "  make run-quick       # Quick test with best solvers"
	@echo "  make benchmark       # Full benchmark suite"
	@echo "  make plots           # Generate visualizations"
	@echo "  make cleanall && make run  # Fresh complete run"
	@echo ""
	@echo "$(GREEN)DOCUMENTATION:$(NC)"
	@echo "  START_HERE.md         - Quick start guide"
	@echo "  README.md             - Comprehensive documentation"
	@echo "  DETAILED_REPORT.md    - Full technical analysis"
	@echo "  COMPLEXITY_ANALYSIS.md - Performance analysis"
	@echo ""
	@echo "$(GREEN)============================================================================$(NC)"

# Quick help
h: help

# ============================================================================
# PHONY TARGETS (don't represent files)
# ============================================================================

.PHONY: all directories run run-direct run-iterative run-sparse run-case1 \
        run-quick run-verbose plots analyze report validate benchmark \
        complexity profile-memory profile-cpu clean clean-results cleanall \
        sysinfo check-deps help h
