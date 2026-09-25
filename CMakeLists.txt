cmake_minimum_required(VERSION 3.10)
project(LRU_Cache VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directories
include_directories(include)

# Executable
add_executable(LRU_Cache src/main.cpp)
