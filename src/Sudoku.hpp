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
        inline static std::uniform_int_distribution<> diff_sampler{0,3};

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

            int sqrtN = std::sqrt(_size);
            int box = sqrtN * (row / sqrtN) + (col / sqrtN);

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

                    _rows[row] ^= mask; _cols[col] ^= mask; _boxes[box] ^= mask;
                    _board[row][col] = 0;
                }
            }
        }

        void generateBoard(int clue_num)
        {

        }

    public:
        Sudoku(const Sudoku&) = delete;
        Sudoku(std::string& difficulty)
        {
            while (std::find(difficulties.begin(), difficulties.end(), difficulty) == difficulties.end())
            {
                std::cout << "Available difficulties are: \n \t - easy\n \t - medium\n \t - hard\n \t - expert\n\n Do you want a randomly selected difficulty? (y/n): ";
                std::string reply;

                std::cin >> reply;
                if (reply == "y")
                {
                    difficulty = difficulties[diff_sampler(rng)];
                    std::cout << "\nThe randomly selected difficulty is: " << difficulty;
                } 
                else 
                {
                    std::cout << "\nSelect a difficulty: ";
                    std::cin >> difficulty;
                }                
            }

            int idx = std::find(difficulties.begin(), difficulties.end(), difficulty) - difficulties.begin();
            std::uniform_int_distribution<int> gen(clues[idx][0],clues[idx][1]);

            generateBoard(gen(rng));
        }

        Sudoku() : Sudoku(difficulties[diff_sampler(rng)])
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

                        int sqrtN = std::sqrt(_size);
                        int box = sqrtN * (i / sqrtN) + (j / sqrtN);
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
            backtrack(0, 0);

            // End timer
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << " [ms]\n";
        }

        friend std::ostream& operator<< (std::ostream& stream, const Sudoku& sudoku) 
        {
            int cellWidth = 3;
            auto printBorder = [&stream,size=sudoku._size, cellWidth](bool thick=false) {
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