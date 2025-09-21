cmake_minimum_required(VERSION 3.14)
project(Sudoku)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Add subdirectories
add_subdirectory(tests)

# Add executable
add_executable(sudoku_app main.cpp)

target_include_directories(sudoku_app PRIVATE ${CMAKE_SOURCE_DIR}/Sudoku)