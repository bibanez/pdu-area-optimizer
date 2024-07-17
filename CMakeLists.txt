cmake_minimum_required(VERSION 3.10)

# Project name
project(AreaOptimizer CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set clangd compatibility
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Check Matplot++ library
find_package(Matplot++ REQUIRED)

# Add compiler flags
add_compile_options(-Wall -Wextra -O3 -g)

# Add executable
add_executable(program
    main.cc
    area_optimizer.cc
)

# Link Matplot++
target_link_libraries(program Matplot++::matplot)

# Set output name
set_target_properties(program PROPERTIES OUTPUT_NAME "program")
