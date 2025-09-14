# Sudoku Generator and Solver

A C++ implementation of a Sudoku generator and solver. Generates valid Sudoku puzzles of varying difficulty and solves them using both naive and heuristic-based backtracking.

---

## Features

- Generate complete Sudoku boards.
- Remove cells to create puzzles with unique solutions.
- Difficulty levels: easy, medium, hard, expert.
- Solving algorithms:
  - Naive backtracking
  - Minimum Remaining Values (MRV) heuristic
- Performance measurement using a lightweight `Timer` utility.
- Pretty-print puzzles to the terminal.

---

## Build Instructions

### Requirements
- C++17 or later
- CMake (optional, if you provide a CMakeLists.txt)
- A standard C++ compiler (g++, clang++, MSVC)

### Example (with g++)
```bash
g++ -std=c++17 -O2 main.cpp -o sudoku
```

---

## Usage

### Random or chosen difficulty
```cpp
#include "Sudoku.hpp"

int main() {
    // Chosen difficulty:
    Sudoku sudoku_chosen("easy"), sudoku_random;
    std::cout << sudoku_chosen << sudoku_random;
    sudoku_chosen.solve("MRV");
    sudoku_random.solve("MRV");
    std::cout << sudoku_chosen << sudoku_random;
}
```

### From a given board

Here, 0's are treated as empty spaces. For example:

```cpp
#include <vector>

std::vector<std::vector<int>> board = {
    {5,3,0, 0,7,0, 0,0,0},
    {6,0,0, 1,9,5, 0,0,0},
    ...
}

Sudoku sudoku(board);
```

---

## Performance

Typical timings:
- Puzzle generation is around 10 ms.
- Solving (depending on difficulty):
  - Naive backtracking is around 500-1000 µs.
  - MRV backtracking is around 30-100 µs.
  
---
