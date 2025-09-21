#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include <array>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <string>
#include <random>
#include <optional>
#include "Timer.hpp"

using vector = std::vector<int>;
using matrix = std::vector<std::vector<int>>;

class Sudoku
{
    private:

        // Sudoku native variables:
        int _size = 9;
        matrix _board;
        vector _rows;
        vector _cols;
        vector _boxes;
        bool _solved = false;

        // RNG devices to generate a Sudoku given a difficulty (chosen by the user or random):
        inline static std::vector<std::string> difficulties{"easy", "medium", "hard", "expert"};
        inline static std::mt19937 rng{std::random_device{}()};
        inline static std::uniform_int_distribution<> difficulty_sampler{0,3};
        inline static constexpr std::array<std::pair<int,int>, 4> clue_ranges = {{{40, 45}, {32, 39}, {25, 31}, {17, 24}}};
        
        // Choose difficulty based on input:
        int chooseDifficulty(const std::optional<std::string>& difficulty)
        {

            // Check if a difficulty is provided:
            if (difficulty) 
            {
                auto it = std::find(difficulties.begin(), difficulties.end(), *difficulty);

                // If difficulty is found, get corresponding index in difficulties array. Otherwise, get a random diff:
                if (it != difficulties.end()) return std::distance(difficulties.begin(), it); 
                else return difficulty_sampler(rng);
            } 
            else return difficulty_sampler(rng); // If no difficulty is provided, it is random
        }

        // Get box [1..9] from row and column:
        int getBox(int row, int col) const
        {
            int sqrtN = std::sqrt(_size);
            return sqrtN * (row / sqrtN) + (col / sqrtN);
        }
        
        // Place a number using a bitmask:
        void placeNumber(int row, int col, int num)
        {
            int mask = 1 << (num - 1);
            _board[row][col] = num;
            _rows[row] |= mask;
            _cols[col] |= mask;
            _boxes[getBox(row, col)] |= mask;
        }

        // Remove a number using a bitmask:
        void removeNumber(int row, int col)
        {
            int num = _board[row][col];
            int mask = 1 << (num - 1);
            _board[row][col] = 0;
            _rows[row] &= ~mask;
            _cols[col] &= ~mask;
            _boxes[getBox(row, col)] &= ~mask;
        }
        
        // Get all possible candidates for a cell (bitmap):
        int getCandidates(const matrix& board, const vector& rows, const vector& cols, const vector& boxes, int row, int col) const
        {   
            // Get used values for that row, column and box
            int used = rows[row] | cols[col] | boxes[getBox(row, col)];

            // Calculate remaining values
            return (~used) & ((1 << _size) - 1);
        }

        // Return {row, col} with the minimum number of candidates:
        std::pair<int, int> findMRV(const matrix& board, const vector& rows, const vector& cols, const vector& boxes) const
        {
            int bestCount = 10;
            std::pair<int, int> bestChoice = {-1, -1};

            for (int i = 0; i < _size; i++)
            {
                for (int j = 0; j < _size; j++)
                {
                    if (board[i][j] != 0) continue;

                    // Count set bits in getCandidates(i, j)
                    int count = __builtin_popcount(getCandidates(board, rows, cols, boxes, i, j));

                    if (count == 1) return {i, j};
                    else if (count < bestCount)
                    {
                        bestCount = count;
                        bestChoice = {i, j};
                    }
                }
            }

            return bestChoice;
        }

        // Return {row, col} with the next empty cell:
        std::pair<int, int> findEmpty() const
        {
            for (int i = 0; i < _size; i++)
            {
                for (int j = 0; j < _size; j++)
                {
                    if (_board[i][j] == 0) return {i, j};
                }
            }

            return {-1, -1};
        }
        
        // Naive backtracking algorithm:
        void backtrack(int row, int col)
        {      
            if (row == _size) 
            {
                _solved = true;
                return;
            }

            if (_board[row][col] != 0)
            {
                if (col < _size - 1) backtrack(row, col + 1);
                else backtrack(row + 1, 0);
                return;
            }

            int box = getBox(row, col);

            for (int num = 1; num <= 9; num++)
            {
                int mask = 1 << (num - 1);
                if (!(_rows[row] & mask) && !(_cols[col] & mask) && !(_boxes[box] & mask)) 
                {
                    placeNumber(row, col, num);

                    if (col < _size - 1) backtrack(row, col + 1);
                    else backtrack(row + 1, 0);
                    if (_solved) return;

                    removeNumber(row, col);
                }
            }
        }

        // Backtracking with MRV:
        void backtrack_MRV()
        {      
            if (_solved) return;

            auto [row, col] = findMRV(_board, _rows, _cols, _boxes);
            
            if (row == -1) 
            {
                _solved = true;
                return;
            }

            int candidates = getCandidates(_board, _rows, _cols, _boxes, row, col);

            for (; candidates; candidates &= (candidates - 1))
            {
                // Get lowest set bit (-mask = ~mask + 1):
                int mask = candidates & -candidates; 
                
                // Get number by counting trailing zeros:
                int num = 1 + __builtin_ctz(mask); 

                placeNumber(row, col, num);
                
                backtrack_MRV();
                if (_solved) return;

                removeNumber(row, col);
            }
        }
        
        // Backtracking with MRV, count solutions:
        int countSolutions(matrix& board, vector& rows, vector& cols, vector& boxes, int& count)
        {
            if (count >= 2) return count;

            auto [row, col] = findMRV(board, rows, cols, boxes);
            
            if (row == -1) 
            {
                count++;
                return count;
            }

            int box = getBox(row, col);
            int candidates = getCandidates(board, rows, cols, boxes, row, col);

            for (; candidates; candidates &= (candidates - 1))
            {
                // Get lowest set bit (-mask = ~mask + 1):
                int mask = candidates & -candidates; 
                
                // Get number by counting trailing zeros:
                int num = 1 + __builtin_ctz(mask); 

                board[row][col] = num;
                rows[row] |= mask; cols[col] |= mask; boxes[box] |= mask;
                
                countSolutions(board, rows, cols, boxes, count);
                if (count >= 2) return count;

                rows[row] &= ~mask; cols[col] &= ~mask; boxes[box] &= ~mask;
                board[row][col] = 0;
            }

            return count;
        }

        // Check if a Sudodu has a unique solution for generation purposes:
        bool hasUniqueSolution()
        {
            auto savedBoard = _board;
            auto savedRows  = _rows;
            auto savedCols  = _cols;
            auto savedBoxes = _boxes;

            int solutionCount = 0;
            countSolutions(savedBoard, savedRows, savedCols, savedBoxes, solutionCount);

            return solutionCount == 1;
        }

        // Generate a board using backtracking and shuffling:
        bool fillBoard() 
        {
            auto [row, col] = findEmpty();
            
            if (row == -1) return true;

            vector nums = {1,2,3,4,5,6,7,8,9};
            std::shuffle(nums.begin(), nums.end(), rng);

            for (int num : nums) 
            {
                int mask = 1 << (num - 1);
                int box = getBox(row, col);

                if (!(_rows[row] & mask) && !(_cols[col] & mask) && !(_boxes[box] & mask)) 
                {
                    placeNumber(row, col, num);
                    
                    if (fillBoard()) return true;

                    removeNumber(row, col);
                }
            }
            
            return false;
        }
        
        // Remove random cells according to difficulty and ensure Sudoku is valid:
        void removeCells(int clue_num)
        {
            int cellsToRemove = (_size * _size) - clue_num;

            std::vector<std::pair<int,int>> positions;
            positions.reserve(_size * _size);
            for (int r = 0; r < _size; ++r)
                for (int c = 0; c < _size; ++c)
                    positions.emplace_back(r, c);

            int removed = 0;

            const int maxPasses = 10; 
            for (int pass = 0; pass < maxPasses && removed < cellsToRemove; ++pass)
            {
                std::shuffle(positions.begin(), positions.end(), rng);
                bool progress = false;

                for (auto [row, col] : positions)
                {
                    if (removed >= cellsToRemove) break;
                    if (_board[row][col] == 0) continue;

                    int prev = _board[row][col];
                    removeNumber(row, col);

                    if (hasUniqueSolution())
                    {
                        ++removed;
                        progress = true;
                    }
                    else placeNumber(row, col, prev);
                    
                }

                if (!progress) break; // no further removable cells found in this pass
            }
        }
        
    public:
        // Delete copy and move constructor and copy and move assignment:
        Sudoku(const Sudoku&) = delete;
        Sudoku& operator=(const Sudoku&) = delete;
        Sudoku(const Sudoku&&) = delete;
        Sudoku&& operator=(const Sudoku&&) = delete;

        // Generate Sudoku for either user-chosen or random difficulty:
        Sudoku(std::optional<std::string> difficulty = std::nullopt) : _board(_size, vector(_size)), _rows(_size), _cols(_size), _boxes(_size)
        {
            std::cout << "Generating Sudoku [difficulty: ";

            // Get idx of the difficulties array, then generate a random number of clues:
            int idx = chooseDifficulty(difficulty);
            auto [low, high] = clue_ranges[idx];
            std::uniform_int_distribution<int> clue_num(low, high);
            int clues = clue_num(rng);
            
            std::cout << difficulties[idx] << "]\nNumber of clues: " << clues << "\n";

            Timer t("Sudoku generation");

            // Generate board. First fill, then remove cells based on difficulty:
            fillBoard();
            removeCells(clues);
        }

        
        // Generate Sudoku from a vector<vector<int>>:
        Sudoku(const matrix& in_board) : _size(in_board.size()), _board(_size, vector(_size)), _rows(_size), _cols(_size), _boxes(_size)
        {
            for (int i = 0; i < _size; i++)
            {
                for (int j = 0; j < _size; j++)
                {
                    _board[i][j] = in_board[i][j];

                    if (_board[i][j] != 0)
                    {
                        int mask = 1 << (_board[i][j] - 1);
                        int box = getBox(i, j);

                        // Check validity of input board (no repeated numbers for a row, column or box):
                        if ((_rows[i] & mask) || (_cols[j] & mask) || (_boxes[box] & mask))
                        {
                            throw std::invalid_argument("Invalid Sudoku: repeated number at (" + std::to_string(i+1) + "," + std::to_string(j+1) + ")");
                        }

                        _rows[i] |= mask; _cols[j] |= mask; _boxes[box] |= mask; 
                    }
                }
            }
        }
        
        // Return state of the Sudoku:
        bool isSolved() const
        {
            return _solved;
        }

        // Solve the Sudoku:
        void solve(std::string solver="")
        {
            std::cout << "Solving Sudoku of size " << _size << "x" << _size << " using ";
            
            Timer t("Sudoku solution");

            // Start backtracking solution
            if (solver == "MRV") 
            {
                std::cout << "Minimum Remaining Values (MRV) heuristic backtracking. ";
                backtrack_MRV();
            }
            else 
            {
                std::cout << "naive backtracking. ";
                backtrack(0, 0);
            }
        }
        
        const matrix& getBoard() const
        {
            return _board;
        }

        // Pretty print of the Sudoku on the terminal:
        friend std::ostream& operator<< (std::ostream& stream, const Sudoku& sudoku) 
        {
            int cellWidth = 3;
            auto printBorder = [&stream,size=sudoku._size, cellWidth](bool thick=false) 
            {
                for (int c = 0; c < size; ++c) 
                {
                    stream << "+";
                    if (thick)
                    {
                        for (int i = 0; i < cellWidth; ++i) stream << "=";
                    }
                    else
                    {
                        for (int i = 0; i < cellWidth; ++i) stream << "-";
                    }          
                }
                
                stream << "+\n";
            };
            
            stream << "\n";
            printBorder(true);
            for (int i = 0; i < sudoku._size; i++)
            {
                for (int j = 0; j < sudoku._size; j++)
                {
                    if (j % 3 == 0)
                    {
                        if (sudoku._board[i][j] == 0) std::cout << "║   ";
                        else std::cout << "║ " << sudoku._board[i][j] << " "; 
                    }
                    else
                    {
                        if (sudoku._board[i][j] == 0) std::cout << "|   ";
                        else std::cout << "| " << sudoku._board[i][j] << " "; 
                    }                   
                } 

                std::cout << "║\n";

                if ((i + 1) % 3 == 0) printBorder(true);
                else printBorder();
            }
            stream << "\n";

            return stream;
        }
};

#endif