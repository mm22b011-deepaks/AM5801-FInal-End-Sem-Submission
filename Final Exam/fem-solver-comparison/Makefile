# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -march=native
INCLUDES = -Isrc

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

# Default target
all: directories $(TARGET)

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
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build successful!"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the program
run: $(TARGET)
	@echo "Running FEM solver..."
	./$(TARGET)

# Generate plots
plots: run
	@echo "Generating plots with gnuplot..."
	cd $(PLOTS_DIR) && gnuplot generate_all_plots.gp
	@echo "Plots generated successfully!"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

# Clean everything including results
cleanall: clean
	@echo "Cleaning results and plots..."
	rm -rf $(RESULTS_DIR)/*.dat
	rm -rf $(PLOTS_DIR)/*.png

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  run       - Build and run the FEM solver"
	@echo "  plots     - Run solver and generate all plots"
	@echo "  clean     - Remove build artifacts"
	@echo "  cleanall  - Remove all generated files"
	@echo "  help      - Show this help message"

.PHONY: all directories run plots clean cleanall help
