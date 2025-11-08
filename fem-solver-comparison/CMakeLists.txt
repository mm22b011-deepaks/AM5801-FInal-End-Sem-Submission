cmake_minimum_required(VERSION 3.10)
project(FEM_Solver_Comparison VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
set(SOURCES
    src/main.cpp
    src/matrix/stiffness_matrix.cpp
    src/solvers/naive_gauss.cpp
    src/solvers/efficient_solver.cpp
    src/fem/boundary_conditions.cpp
    src/analysis/stress_strain.cpp
    src/utils/benchmark.cpp
)

# Executable
add_executable(fem_solver ${SOURCES})

# Create results and plots directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/results)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/plots)

# Copy gnuplot scripts to build directory
file(COPY ${CMAKE_SOURCE_DIR}/plots/ 
     DESTINATION ${CMAKE_BINARY_DIR}/plots/)

# Custom target to run the program
add_custom_target(run
    COMMAND fem_solver
    DEPENDS fem_solver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running FEM solver..."
)

# Custom target to generate plots
add_custom_target(plots
    COMMAND fem_solver
    COMMAND cd plots && gnuplot generate_all_plots.gp
    DEPENDS fem_solver
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running solver and generating plots..."
)

# Installation
install(TARGETS fem_solver DESTINATION bin)
