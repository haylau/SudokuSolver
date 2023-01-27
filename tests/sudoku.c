/**
 * @file sudoku.c
 * @author Hayden Lauritzen (haydenlauritzen@gmail.com)
 * @date 2023-1-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "sudoku.h"

int getGridIdx(int row, int col, int psize) {
    int gridSize = (int)floor(sqrt(psize));
    return (((row - 1) / gridSize) * gridSize) + ((col - 1) / gridSize) + 1;
}

smallestSolve_t* getSmallestSolve(missing_t* missingNums, int psize) {
    smallestSolve_t* ret = (smallestSolve_t*)malloc(sizeof(smallestSolve_t));
    int smallest = __INT32_MAX__;
    for (int i = 0; i < psize; ++i) {
        if (missingNums[i].rows > 1 && missingNums[i].rows < smallest) {
            ret->type = rows;
            ret->idx = i + 1;
            smallest = missingNums[i].rows;
        }
        if (missingNums[i].cols > 1 && missingNums[i].cols < smallest) {
            ret->type = cols;
            ret->idx = i + 1;
            smallest = missingNums[i].cols;
        }
        if (missingNums[i].grids > 1 && missingNums[i].grids < smallest) {
            ret->type = grids;
            ret->idx = i + 1;
            smallest = missingNums[i].grids;
        }
    }
    return ret;
}

bool isSolvable(missing_t* missingNums, int psize) {
    for (int i = 0; i < psize; ++i) {
        if (missingNums[i].rows == 1 || missingNums[i].cols == 1 || missingNums[i].grids == 1) {
            return true;
        }
    }
    return false;
}
bool isComplete(missing_t* missingNums, int psize) {
    for (int i = 0; i < psize; ++i) {
        if (missingNums[i].rows > 0 || missingNums[i].cols > 0 || missingNums[i].grids > 0) {
            return false;
        }
    }
    return true;
}

void makeMove(missing_t* missingNums, int row, int col, int grids, int** grid, int num) {
    grid[row][col] = num;
    --(missingNums[row - 1].rows);
    --(missingNums[col - 1].cols);
    --(missingNums[grids - 1].grids);
}

void undoMove(savedPos_t* saved, int** grid, missing_t* missingNums, int psize) {
    memcpy(missingNums, saved->missing, sizeof(missing_t) * (psize));
    for (int i = 1; i <= psize; ++i) {
        memcpy(grid[i], saved->grid[i], sizeof(int) * (psize + 1));
    }
    // free savedPos
    for (int i = 1; i <= psize; ++i) {
        free(saved->grid[i]);
    }
    free(saved->missing);
    free(saved);
}

savedPos_t* savePos(int** grid, missing_t* missingNums, int psize) {
    // alloc
    savedPos_t* saved = (savedPos_t*)malloc(sizeof(savedPos_t));
    saved->grid = (int**)calloc(psize + 1, sizeof(int*));
    for (int i = 1; i <= psize; ++i) {
        saved->grid[i] = (int*)calloc((psize + 1), sizeof(int));
    }
    // copy
    saved->missing = (missing_t*)calloc(psize, sizeof(missing_t));
    memcpy(saved->missing, missingNums, sizeof(missing_t) * (psize));
    for (int i = 1; i <= psize; ++i) {
        memcpy(saved->grid[i], grid[i], sizeof(int) * (psize + 1));
    }
    return saved;
}


void* solveRow(void* args) {
    solvepuzzle_t* params = (solvepuzzle_t*)args;
    bool* numCount = (bool*)calloc(params->psize + 1, sizeof(bool)); // keep track of used nums
    int numIdx, missingNum;
    int row = params->idx;
    for (int col = 1; col <= params->psize; ++col) {
        int num = params->grid[row][col];
        if (num == 0) {
            // location of missing number
            numIdx = col;
        }
        else {
            // first occurance of number
            numCount[num] = true;
        }
    }
    for (int i = 1; i <= params->psize; ++i) {
        if (!numCount[i]) missingNum = i;
    }
    // make move
    makeMove(params->missingNums, row, numIdx, getGridIdx(row, numIdx, params->psize), params->grid, missingNum);
    free(numCount);
    return NULL;
}

void* solveCol(void* args) {
    solvepuzzle_t* params = (solvepuzzle_t*)args;
    bool* numCount = (bool*)calloc(params->psize + 1, sizeof(bool)); // keep track of used nums
    int numIdx, missingNum;
    int col = params->idx;
    for (int row = 1; row <= params->psize; ++row) {
        int num = params->grid[row][col];
        if (num == 0) {
            // location of missing number
            numIdx = row;
        }
        else {
            // first occurance of number
            numCount[num] = true;
        }
    }
    for (int i = 1; i <= params->psize; ++i) {
        if (!numCount[i]) missingNum = i;
    }
    makeMove(params->missingNums, numIdx, col, getGridIdx(numIdx, col, params->psize), params->grid, missingNum);
    free(numCount);
    return NULL;
}
void* solveGrid(void* args) {
    solvepuzzle_t* params = (solvepuzzle_t*)args;
    bool* numCount = (bool*)calloc(params->psize + 1, sizeof(bool)); // keep track of used nums
    int rowIdx, colIdx, missingNum;
    int grid = params->idx;
    int gridSize = (int)floor(sqrt(params->psize));
    for (int i = 0; i < gridSize; ++i) {
        // converts grid idx to grid inital row idx
        int row = (((params->idx - 1) / gridSize) * gridSize + 1) + i;
        for (int j = 0; j < gridSize; ++j) {
            // converts grid idx to grid inital col idx
            int col = (((params->idx - 1) % gridSize) * gridSize + 1) + j;
            int num = params->grid[row][col];
            if (num == 0) {
                // puzzle is not complete, but might be valid
                rowIdx = row;
                colIdx = col;
            }
            else {
                // first occurance of number
                numCount[num] = true;
            }
        }
    }
    for (int i = 1; i <= params->psize; ++i) {
        if (!numCount[i]) missingNum = i;
    }
    makeMove(params->missingNums, rowIdx, colIdx, grid, params->grid, missingNum);
    free(numCount);
    return NULL;
}

cell_t* selectCell(smallestSolve_t* subset, int psize, int** grid) {
    cell_t* cell = (cell_t*)malloc(sizeof(cell_t));
    switch (subset->type) {
    case rows: {
        int row = subset->idx;
        for (int col = 1; col <= psize; ++col) {
            if (grid[row][col] == 0) {
                cell->row = row;
                cell->col = col;
            }
        }
        break;
    }
    case cols: {
        int col = subset->idx;
        for (int row = 1; row <= psize; ++row) {
            if (grid[row][col] == 0) {
                cell->row = row;
                cell->col = col;
            }
        }
        break;
    }
    case grids: {
        int grids = subset->idx;
        int gridSize = (int)floor(sqrt(psize));
        for (int i = 0; i < gridSize; ++i) {
            // converts grid idx to grid inital row idx
            int row = (((grids - 1) / gridSize) * gridSize + 1) + i;
            for (int j = 0; j < gridSize; ++j) {
                // converts grid idx to grid inital col idx
                int col = (((grids - 1) % gridSize) * gridSize + 1) + j;
                int num = grid[row][col];
                if (num == 0) {
                    cell->row = row;
                    cell->row = col;
                }
            }
        }
        break;
    }
    }
    return cell;
}

int* solveCell(int row_n, int col_n, int grid_n, int psize, int** grid) {
    bool* numCount = (bool*)calloc(psize + 1, sizeof(bool)); // keep track of used nums
    int* validNums = (int*)calloc(psize, sizeof(bool)); // keep track of used nums
    // check row and col
    for (int i = 1; i <= psize; ++i) {
        int rowNum = grid[row_n][i];
        if (!numCount[rowNum]) numCount[rowNum] = true;
        int colNum = grid[i][col_n];
        if (!numCount[colNum]) numCount[colNum] = true;
    }
    // check grid
    int gridSize = (int)floor(sqrt(psize));
    for (int i = 0; i < gridSize; ++i) {
        // converts grid idx to grid inital row idx
        int row = (((grid_n - 1) / gridSize) * gridSize + 1) + i;
        for (int j = 0; j < gridSize; ++j) {
            // converts grid idx to grid inital col idx
            int col = (((grid_n - 1) % gridSize) * gridSize + 1) + j;
            int gridNum = grid[row][col];
            if (!numCount[gridNum]) numCount[gridNum] = true;
        }
    }
    int j = 0;
    for (int i = 1; i <= psize; ++i) {
        if (!numCount[i]) {
            validNums[j] = i;
            ++j;
        }
    }
    validNums[j] = 0;
    free(numCount);
    return validNums;
}

void solvePuzzle(missing_t* missingNums, int psize, int** grid) {
    // make all possible easy solves
    while (isSolvable(missingNums, psize)) {
        bool* working = (bool*)calloc(psize, sizeof(bool));
        pthread_t solveThreads[psize];
        solvepuzzle_t args[psize];
        // spawn row threads
        for (int i = 0; i < psize; ++i) {
            if (missingNums[i].rows == 1) { // there is a easy solve here
                args[i].idx = i + 1;
                args[i].psize = psize;
                args[i].grid = grid;
                args[i].missingNums = missingNums;
                working[i] = true;
                pthread_create(&solveThreads[i], NULL, solveRow, &args[i]);
            }
        }

        // join row threads
        for (int i = 0; i < psize; ++i) {
            if (working[i]) {
                pthread_join(solveThreads[i], NULL);
                working[i] = false;
            }

        }
        // spawn col threads
        for (int i = 0; i < psize; ++i) {
            if (missingNums[i].cols == 1) { // there is a easy solve here
                args[i].idx = i + 1;
                args[i].psize = psize;
                args[i].grid = grid;
                args[i].missingNums = missingNums;
                working[i] = true;
                pthread_create(&solveThreads[i], NULL, solveCol, &args[i]);
            }
        }
        // join col threads
        for (int i = 0; i < psize; ++i) {
            if (working[i]) {
                pthread_join(solveThreads[i], NULL);
                working[i] = false;
            }
        }
        // spawn grid threads
        for (int i = 0; i < psize; ++i) {
            if (missingNums[i].grids == 1) { // there is a easy solve here
                args[i].idx = i + 1;
                args[i].psize = psize;
                args[i].grid = grid;
                args[i].missingNums = missingNums;
                working[i] = true;
                pthread_create(&solveThreads[i], NULL, solveGrid, &args[i]);
            }
        }
        // join grid threads
        for (int i = 0; i < psize; ++i) {
            if (working[i]) {
                pthread_join(solveThreads[i], NULL);
                working[i] = false;
            }
        }
        free(working);
    }

    // no more easy solves left, check if complete
    if (!isComplete(missingNums, psize)) {
        // puzzle is not complete, find smallest start location
        smallestSolve_t* subset = getSmallestSolve(missingNums, psize);
        // select a cell in the smallset subset to guess
        cell_t* cell = selectCell(subset, psize, grid);
        // get legal moves
        int gridIdx = getGridIdx(cell->row, cell->col, psize);
        int* moves = solveCell(cell->row, cell->col, gridIdx, psize, grid);
        int i = 0;
        while (moves[i] != 0) {
            // make move; this will decrease the smallest subset's missing numbers
            savedPos_t* savedPos = savePos(grid, missingNums, psize);
            makeMove(missingNums, cell->row, cell->col, gridIdx, grid, moves[i]);

            // increase depth, returns when no moves left; i.e. puzzle complete
            solvePuzzle(missingNums, psize, grid);

            // check if puzzle is complete and/or valid
            bool complete;
            bool valid;
            checkPuzzle(psize, grid, &complete, &valid);
            if (complete && valid) {
                for (int j = 0; j <= psize; ++j) {
                    free(savedPos->grid[j]);
                }
                free(savedPos->missing);
                free(savedPos);
                break; // puzzle is complete
            }

            // undo move if puzzle was illegal
            undoMove(savedPos, grid, missingNums, psize);
            ++i;
        }
        free(subset);
        free(cell);
        free(moves);
    }
}

void* checkRow(void* args) {
    checkpuzzle_t* params = (checkpuzzle_t*)args;
    // keep track of used nums
    bool* numCount = (bool*)calloc(params->psize + 1, sizeof(bool));
    for (int col = 1; col <= params->psize; ++col) {
        int num = params->grid[params->idx][col];
        if (num == 0) {
            // puzzle is not complete, but might be valid
            ++(params->numMissing);
            params->complete = false;
        }
        else if (numCount[num]) {
            // doubled number puzzle is not valid nor complete
            params->valid = false;
            params->complete = false;
            return NULL;
        }
        else {
            // first occurance of number
            numCount[num] = true;
        }
    }
    free(numCount);
    return NULL;
};
void* checkCol(void* args) {
    checkpuzzle_t* params = (checkpuzzle_t*)args;
    bool* numCount = (bool*)calloc(params->psize + 1, sizeof(bool)); // keep track of used nums
    for (int row = 1; row <= params->psize; ++row) {
        int num = params->grid[row][params->idx];
        if (num == 0) {
            // puzzle is not complete, but might be valid
            ++(params->numMissing);
            params->complete = false;
        }
        else if (numCount[num]) {
            // doubled number puzzle is not valid nor complete
            params->valid = false;
            params->complete = false;
            return NULL;
        }
        else {
            // first occurance of number
            numCount[num] = true;
        }
    }
    return NULL;
};
void* checkGrid(void* args) {
    checkpuzzle_t* params = (checkpuzzle_t*)args;
    bool* numCount = (bool*)calloc(params->psize + 1, sizeof(bool)); // keep track of used nums
    int gridSize = (int)floor(sqrt(params->psize));
    for (int i = 0; i < gridSize; ++i) {
        // converts grid idx to grid inital row idx
        int row = (((params->idx - 1) / gridSize) * gridSize + 1) + i;
        for (int j = 0; j < gridSize; ++j) {
            // converts grid idx to grid inital col idx
            int col = (((params->idx - 1) % gridSize) * gridSize + 1) + j;
            int num = params->grid[row][col];
            if (num == 0) {
                // puzzle is not complete, but might be valid
                ++(params->numMissing);
                params->complete = false;
            }
            else if (numCount[num]) {
                // doubled number puzzle is not valid nor complete
                params->valid = false;
                params->complete = false;
                return NULL;
            }
            else {
                // first occurance of number
                numCount[num] = true;
            }
        }
    }
    return NULL;
};

missing_t* checkPuzzle(int psize, int** grid, bool* complete, bool* valid) {

    missing_t* missing = (missing_t*)malloc(sizeof(missing_t) * psize);
    // arguments for thread function
    checkpuzzle_t rows[psize];
    checkpuzzle_t cols[psize];
    checkpuzzle_t grids[psize];

    // open threads for every subset of the puzzle
    pthread_t rowThreads[psize];
    pthread_t colThreads[psize];
    pthread_t gridThreads[psize];
    for (int i = 0; i < psize; ++i) {
        // open row thread
        rows[i].idx = i + 1;
        rows[i].psize = psize;
        rows[i].grid = grid;
        rows[i].valid = true;
        rows[i].complete = true;
        rows[i].numMissing = 0;
        pthread_create(&rowThreads[i], NULL, checkRow, &rows[i]);
        // open column thread
        cols[i].idx = i + 1;
        cols[i].psize = psize;
        cols[i].grid = grid;
        cols[i].valid = true;
        cols[i].complete = true;
        cols[i].numMissing = 0;
        pthread_create(&colThreads[i], NULL, checkCol, &cols[i]);
        // open grid thread
        grids[i].idx = i + 1;
        grids[i].psize = psize;
        grids[i].grid = grid;
        grids[i].valid = true;
        grids[i].complete = true;
        grids[i].numMissing = 0;
        pthread_create(&gridThreads[i], NULL, checkGrid, &grids[i]);
    }

    // join all threads
    for (int i = 0; i < psize; ++i) {
        pthread_join(rowThreads[i], NULL);
        pthread_join(colThreads[i], NULL);
        pthread_join(gridThreads[i], NULL);
    }

    // check valid and complete status
    *valid = true;    // if all rows/columns/boxes have numbers from 1 to psize
    *complete = true; // if it can be completed with no 0s in it
    for (int i = 0; i < psize; ++i) {
        if (!rows[i].valid || !cols[i].valid || !grids[i].valid) *valid = false;
        if (!rows[i].complete || !cols[i].complete || !grids[i].complete) *complete = false;
        missing[i].rows = rows[i].numMissing;
        missing[i].cols = cols[i].numMissing;
        missing[i].grids = grids[i].numMissing;
    }

    return missing;
}

int readSudokuPuzzle(char* filename, int*** grid) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Could not open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    int psize;
    fscanf(fp, "%d", &psize);
    int** agrid = (int**)malloc((psize + 1) * sizeof(int*));
    for (int row = 1; row <= psize; row++) {
        agrid[row] = (int*)malloc((psize + 1) * sizeof(int));
        for (int col = 1; col <= psize; col++) {
            fscanf(fp, "%d", &agrid[row][col]);
        }
    }
    fclose(fp);
    *grid = agrid;
    return psize;
}

void printSudokuPuzzle(int psize, int** grid) {
    printf("%d\n", psize);
    for (int row = 1; row <= psize; row++) {
        for (int col = 1; col <= psize; col++) {
            printf("%d ", grid[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}

void deleteSudokuPuzzle(int psize, int** grid) {
    for (int row = 1; row <= psize; row++) {
        free(grid[row]);
    }
    free(grid);
}

// expects file name of the puzzle as argument in command line
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: ./sudoku puzzle.txt\n");
        return EXIT_FAILURE;
    }
    // grid is a 2D array
    int** grid = NULL;
    // find grid size and fill grid
    int sudokuSize = readSudokuPuzzle(argv[1], &grid);
    bool valid = false;
    bool complete = false;
    missing_t* missing = checkPuzzle(sudokuSize, grid, &complete, &valid);
    printf("Complete puzzle? ");
    printf(complete ? "true\n" : "false\n");
    if (complete) {
        printf("Valid puzzle? ");
        printf(valid ? "true\n" : "false\n");
    }
    else if (valid) {
        printf("Attempting Solve of:\n");
        printSudokuPuzzle(sudokuSize, grid);
        solvePuzzle(missing, sudokuSize, grid);
    }
    printSudokuPuzzle(sudokuSize, grid);
    free(missing);
    deleteSudokuPuzzle(sudokuSize, grid);
    return EXIT_SUCCESS;
}