# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# Project name and version
project(university-management-system VERSION 1.0.0 LANGUAGES CXX)

# Enforce C++17 standard as requested
# C++17 provides std::variant, std::filesystem, and std::optional which are crucial for our architecture.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF) # Disable compiler-specific extensions for portability

# Define testing option before dependencies so it can be used there
option(BUILD_TESTING "Build the testing tree" ON)

# Fetch and configure dependencies
include(cmake/Dependencies.cmake)

# Set output directories to keep the build directory clean and organized
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add source and test directories
add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
