#include <gtest/gtest.h>
#include "Sudoku.hpp"
#include <vector>

using matrix = std::vector<std::vector<int>>;

int count_clues(const matrix& board)
{
    int clues = 0;
    for (const auto& row : board)
    {
        for (auto val : row)
        {
            if (val != 0) clues++;
        }
    }

    return clues;
}

class SudokuTest : public testing::Test
{
    protected: 
        const matrix board = {{5,3,0,0,7,0,0,0,0},
        {6,0,0,1,9,5,0,0,0},
        {0,9,8,0,0,0,0,6,0},
        {8,0,0,0,6,0,0,0,3},
        {4,0,0,8,0,3,0,0,1},
        {7,0,0,0,2,0,0,0,6},
        {0,6,0,0,0,0,2,8,0},
        {0,0,0,4,1,9,0,0,5},
        {0,0,0,0,8,0,0,7,9}
        };
        const matrix solution = {{5,3,4,6,7,8,9,1,2},
        {6,7,2,1,9,5,3,4,8},
        {1,9,8,3,4,2,5,6,7},
        {8,5,9,7,6,1,4,2,3},
        {4,2,6,8,5,3,7,9,1},
        {7,1,3,9,2,4,8,5,6},
        {9,6,1,5,3,7,2,8,4},
        {2,8,7,4,1,9,6,3,5},
        {3,4,5,2,8,6,1,7,9}
    };
};

TEST(Sudoku, ConstructsEasy) 
{
    Sudoku s("easy");
    EXPECT_FALSE(s.isSolved());

    matrix board = s.getBoard();
    int clues = count_clues(board);
    
    EXPECT_GE(clues, 40);
    EXPECT_LE(clues, 45);
}

TEST(Sudoku, ConstructsMedium) 
{
    Sudoku s("medium");
    EXPECT_FALSE(s.isSolved());

    matrix board = s.getBoard();
    int clues = count_clues(board);
    
    EXPECT_GE(clues, 32);
    EXPECT_LE(clues, 39);
}

TEST(Sudoku, ConstructsHard) 
{
    Sudoku s("hard");
    EXPECT_FALSE(s.isSolved());

    matrix board = s.getBoard();
    int clues = count_clues(board);
    
    EXPECT_GE(clues, 25);
    EXPECT_LE(clues, 31);
}

TEST(Sudoku, ConstructsExpert) 
{
    for (int i = 0; i < 100; i++)
    {
        Sudoku s("expert");
        EXPECT_FALSE(s.isSolved());

        matrix board = s.getBoard();
        int clues = count_clues(board);

        EXPECT_GE(clues, 17);
        EXPECT_LE(clues, 24);
    }
}

TEST_F(SudokuTest, ConstructsFromBoard)
{
    Sudoku sudoku(board);
    matrix SudokuBoard = sudoku.getBoard();

    EXPECT_EQ(SudokuBoard, board);
}

TEST(Sudoku, InvalidBoardThrows) {
    matrix bad = {
        {1,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    };
    EXPECT_THROW(Sudoku s(bad), std::invalid_argument);
}

TEST_F(SudokuTest, NaiveSolutionTest)
{
    Sudoku sudoku(board);
    sudoku.solve();

    EXPECT_EQ(sudoku.getBoard(), solution);
    EXPECT_TRUE(sudoku.isSolved());
}

TEST_F(SudokuTest, MRVSolutionTest)
{
    Sudoku sudoku(board);
    sudoku.solve("MRV");

    EXPECT_EQ(sudoku.getBoard(), solution);
    EXPECT_TRUE(sudoku.isSolved());
}

TEST(Sudoku, SolvesWithDifficulty) {
    Sudoku s1("easy"), s2("medium"), s3("hard"), s4("expert");

    s1.solve("MRV");
    s2.solve("MRV");
    s3.solve("MRV");
    s4.solve("MRV");

    EXPECT_TRUE(s1.isSolved());
    EXPECT_TRUE(s2.isSolved());
    EXPECT_TRUE(s3.isSolved());
    EXPECT_TRUE(s4.isSolved());
}

TEST(Sudoku, EmptyBoardisSolvableMRV)
{
    matrix board = {
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
    };

    Sudoku sudoku(board);
    sudoku.solve("MRV");

    EXPECT_TRUE(sudoku.isSolved());
}

/*int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}*/