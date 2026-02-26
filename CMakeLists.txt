cmake_minimum_required(VERSION 3.10)
project(TrainTicketSystem)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

# Find all C++ source files in the current directory only (not subdirectories)
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Exclude test files from the main build
list(FILTER SOURCES EXCLUDE REGEX ".*test_.*\\.cpp$")

# Create executable named "code"
add_executable(code ${SOURCES})
