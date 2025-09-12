#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <string>
#include <random>
#include <chrono>

using vector = std::vector<int>;
using board = std::vector<std::vector<int>>;

class Sudoku
{
    private:
        const int _size = 9;
        board _board;
        vector _rows;
        vector _cols;
        vector _boxes;
        bool _solved = false;
        inline static std::vector<std::string> difficulties{"easy", "medium", "hard", "expert"};
        inline const static std::vector<vector> clues{{40,45}, {32,39}, {25, 31}, {17, 24}};
        inline static std::mt19937 rng{std::random_device{}()};
        inline static std::uniform_int_distribution<> difficulty_sampler{0,3};

        int getBox(int row, int col)
        {
            int sqrtN = std::sqrt(_size);
            return sqrtN * (row / sqrtN) + (col / sqrtN);
        }

        int getCandidates(int row, int col)
        {
            int box = getBox(row, col);
            
            // Get used values for that row, column and box
            int used = _rows[row] | _cols[col] | _boxes[box];

            // Calculate remaining values
            return (~used) & ((1 << _size) - 1);
        }

        std::pair<int, int> findMRV()
        {
            int bestCount = 10;
            std::pair<int, int> bestChoice = {-1, -1};

            for (int i = 0; i < _size; i++)
            {
                for (int j = 0; j < _size; j++)
                {
                    if (_board[i][j] != 0) continue;

                    // Count set bits in getCandidates(i, j)
                    int count = __builtin_popcount(getCandidates(i, j));

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
                    _board[row][col] = num;
                    _rows[row] |= mask; _cols[col] |= mask; _boxes[box] |= mask;

                    if (col < _size - 1) backtrack(row, col + 1);
                    else backtrack(row + 1, 0);
                    if (_solved) return;

                    _rows[row] &= ~mask; _cols[col] &= ~mask; _boxes[box] &= ~mask;
                    _board[row][col] = 0;
                }
            }
        }

        void backtrack_withMRV()
        {      
            if (_solved) return;

            auto [row, col] = findMRV();
            
            if (row == -1) 
            {
                _solved = true;
                return;
            }

            int box = getBox(row, col);
            int candidates = getCandidates(row, col);

            for (; candidates; candidates &= (candidates - 1))
            {
                // Get lowest set bit (-mask = ~mask + 1):
                int mask = candidates & -candidates; 
                
                // Get number by counting trailing zeros:
                int num = 1 + __builtin_ctz(mask); 

                _board[row][col] = num;
                _rows[row] |= mask; _cols[col] |= mask; _boxes[box] |= mask;
                
                backtrack_withMRV();
                if (_solved) return;

                _rows[row] &= ~mask; _cols[col] &= ~mask; _boxes[box] &= ~mask;
                _board[row][col] = 0;
            }
        }
        
        bool fillBoard(int row, int col) 
        {
            if (row == _size) return true;

            vector nums = {1,2,3,4,5,6,7,8,9};
            std::shuffle(nums.begin(), nums.end(), rng);

            for (int num : nums) 
            {
                int mask = 1 << (num - 1);
                int box = 3 * (row / 3) + col / 3;
                if (!(_rows[row] & mask) && !(_cols[col] & mask) && !(_boxes[box] & mask)) 
                {
                    _board[row][col] = num;
                    _rows[row] |= mask; _cols[col] |= mask; _boxes[box] |= mask;
                    
                    if (col < _size - 1) 
                    {
                        if (fillBoard(row, col + 1)) return true;
                    }
                    else 
                    {
                        if (fillBoard(row + 1, 0)) return true;
                    }

                    _board[row][col] = 0;
                    _rows[row] &= ~mask; _cols[col] &= ~mask; _boxes[box] &= ~mask;
                }
            }
            
            return false;
        }
        
        void removeCells(int clue_num)
        {

        }
        
    public:
        Sudoku(const Sudoku&) = delete;
        Sudoku& operator=(const Sudoku&) = delete;
        Sudoku(const std::string& difficulty) : _board(_size, vector(_size)), _rows(_size), _cols(_size), _boxes(_size)
        {
            while (std::find(difficulties.begin(), difficulties.end(), difficulty) == difficulties.end())
            {
                std::cout << "Available difficulties are: \n \t - easy\n \t - medium\n \t - hard\n \t - expert\n\n Do you want a randomly selected difficulty? (y/n): ";
                std::string reply;

                std::cin >> reply;
                if (reply == "y")
                {
                    //difficulty = difficulties[difficulty_sampler(rng)];
                    std::cout << "\nThe randomly selected difficulty is: " << difficulty;
                } 
                else 
                {
                    std::cout << "\nSelect a difficulty: ";
                    //std::cin >> difficulty;
                }                
            }

            int idx = std::find(difficulties.begin(), difficulties.end(), difficulty) - difficulties.begin();
            std::uniform_int_distribution<int> clue_num(clues[idx][0],clues[idx][1]);

            fillBoard(0, 0);
            removeCells(clue_num(rng));
        }

        Sudoku() : Sudoku(difficulties[difficulty_sampler(rng)])
        {}
        
        Sudoku(const board& in_board) : _size(in_board.size()), _board(_size, vector(_size)), _rows(_size), _cols(_size), _boxes(_size)
        {
            for (int i = 0; i < _size; i++)
            {
                for (int j = 0; j < _size; j++)
                {
                    _board[i][j] = in_board[i][j];
                    if (_board[i][j] != 0)
                    {
                        int mask = 1 << (_board[i][j] - 1);
                        _rows[i] ^= mask; _cols[j] ^= mask;   

                        int box = getBox(i, j);
                        _boxes[box] ^= mask; 
                    }
                }
            }
        }

        bool isSolved()
        {
            return _solved;
        }

        void solve()
        {
            std::cout << "Solving SUDOKU of size " << _size << "x" << _size << ". Elapsed time: ";
            
            // Start timer
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

            // Start backtracking solution
            //backtrack(0, 0);
            backtrack_withMRV();
            // End timer
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " [ms]\n";
        }

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
            
            return stream;
        }
};

#endif