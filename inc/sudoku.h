/**
 * @file sudoku.h
 * @author Hayden Lauritzen (haydenlauritzen@gmail.com)
 * @date 2022-12-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <math.h>    // sqrt
#include <stdbool.h> // bool
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // threads

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

// helper functions
inline int getGridIdx(int row, int col, int psize);
inline bool isSolvable(missing_t* missingNums, int psize);

typedef struct solvepuzzle_t {
  int idx;
  int psize;
  int** grid;
  missing_t* missingNums;
} solvepuzzle_t;


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

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
int readSudokuPuzzle(char* filename, int*** grid);

// takes puzzle size and grid[][]
// prints the puzzle
void printSudokuPuzzle(int psize, int** grid);

// takes puzzle size and grid[][]
// frees the memory allocated
void deleteSudokuPuzzle(int psize, int** grid);
