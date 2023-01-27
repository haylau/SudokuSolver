/**
 * @file sudoku.h
 * @author Hayden Lauritzen (haydenlauritzen@gmail.com)
 * @date 2023-1-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <math.h>    // sqrt; gcc requires the '-lm' option when compiling 
#include <stdbool.h> // bool
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // threads; gcc requires the '-pthread' option when compiling
#include <string.h> // memcpy()

 /**
 *  Board Structure
 * grid[row][col]
 *   1 2 3 4 5 6 7 8 9 col
 * 1 - - - - - - - - -
 * 2 - - - - - - - - -
 * 3 - - - - - - - - -
 * 4 - - - - - - - - -
 * 5 - - - - - - - - -
 * 6 - - - - - - - - -
 * 7 - - - - - - - - -
 * 8 - - - - - - - - -
 * 9 - - - - - - - - -
 * row
 *
 *   1 2 3 4
 * 1 - - - -
 * 2 - - - -
 * 3 - - - -
 * 4 - - - -
 *
 * 1 2
 * 3 4
 *
 * 1 2 3
 * 4 5 6
 * 7 8 9
 *
 *
 *
 * 1 - sqrt(psize)
 * - - -
 * - - psize
 * grid
 *
*/

// threaded functions for validation
void* solveRow(void* args);
void* solveCol(void* args);
void* solveGrid(void* args);

// threaded functions for solving
void* checkRow(void* args);
void* checkCol(void* args);
void* checkGrid(void* args);

// missing number information 
typedef struct missing_t {
  int rows;
  int cols;
  int grids;
} missing_t;

// storage for pthreads
typedef struct checkpuzzle_t {
  int idx;
  int psize;
  int** grid;
  bool valid;
  bool complete;
  int numMissing;
} checkpuzzle_t;

typedef struct solvepuzzle_t {
  int idx;
  int psize;
  int** grid;
  missing_t* missingNums;
} solvepuzzle_t;

enum subset { rows, cols, grids };

typedef struct cell_t {
  int row;
  int col;
} cell_t;

typedef struct smallestSolve_t {
  enum subset type;
  int idx;
} smallestSolve_t;

typedef struct savedPos_t {
  int** grid;
  missing_t* missing;
} savedPos_t;

// helper functions
inline int getGridIdx(int row, int col, int psize);
inline bool isSolvable(missing_t* missingNums, int psize);
inline bool isComplete(missing_t* missingNums, int psize);
void makeMove(missing_t* missingNums, int row, int col, int grids, int** grid, int num);
void undoMove(savedPos_t* savedPos, int** grid, missing_t* missingNums, int psize); // free's savedPos_t
savedPos_t* savePos(int** grid, missing_t* missingNums, int psize); // returns malloc!

// takes puzzle size and grid[][] representing sudoku puzzle
// and tow booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9][9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity
// returns int[3][10] of num missing in each subsection
missing_t* checkPuzzle(int psize, int** grid, bool* complete, bool* valid);

void solvePuzzle(missing_t* missingNums, int psize, int** grid);
int* solveCell(int row_n, int col_n, int grid_n, int psize, int** grid); // returns malloc!
smallestSolve_t* getSmallestSolve(missing_t* missingNums, int psize); // returns malloc
cell_t* selectCell(smallestSolve_t* subset, int psize, int** grid); // returns malloc!

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
int readSudokuPuzzle(char* filename, int*** grid);

// takes puzzle size and grid[][]
// prints the puzzle
void printSudokuPuzzle(int psize, int** grid);

// takes puzzle size and grid[][]
// frees the memory allocated
void deleteSudokuPuzzle(int psize, int** grid);
