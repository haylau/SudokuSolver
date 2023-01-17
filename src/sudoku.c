/**
 * @file sudoku.c
 * @author Hayden Lauritzen (haydenlauritzen@gmail.com)
 * @date 2022-12-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "../inc/sudoku.h"

void* checkRow(void* arg) {
    t_checkpuzzle* status = (t_checkpuzzle*)arg;
    bool numCount[10] = { 0 }; // keep track of used nums
    int numMissing = 0;
    for (int col = 1; col <= status->psize; ++col) {
        int num = status->grid[status->idx][col];
        if (num == 0) {
            // puzzle is not complete, but might be valid
            ++numMissing;
            status->complete = false;
        }
        else if (numCount[num]) {
            // doubled number puzzle is not valid nor complete
            status->valid = false;
            status->complete = false;
            return NULL;
        }
        else {
            // first occurance of number
            numCount[num] = true;
        }
    }
    return NULL;
};
void* checkCol(void* arg) {
    t_checkpuzzle* status = (t_checkpuzzle*)arg;
    bool numCount[10] = { 0 }; // keep track of used nums
    int numMissing = 0;
    for (int row = 1; row <= status->psize; ++row) {
        int num = status->grid[row][status->idx];
        if (num == 0) {
            // puzzle is not complete, but might be valid
            ++numMissing;
            status->complete = false;
        }
        else if (numCount[num]) {
            // doubled number puzzle is not valid nor complete
            status->valid = false;
            status->complete = false;
            return NULL;
        }
        else {
            // first occurance of number
            numCount[num] = true;
        }
    }
    return NULL;
};
void* checkGrid(void* arg) {
    t_checkpuzzle* status = (t_checkpuzzle*)arg;
    bool numCount[10] = { 0 }; // keep track of used nums
    int numMissing = 0;
    int gridSize = (int)floor(sqrt(status->psize));
    for (int i = 0; i < gridSize; ++i) {
        // converts grid idx to grid inital row idx
        int row = ((status->idx - 1) / 3 * 3 + 1) + i;
        for (int j = 0; j < gridSize; ++j) {
            // converts grid idx to grid inital col idx
            int col = ((status->idx % 3 == 1) ? 1 : ((status->idx % 3 == 2) ? 4 : 7)) + j;
            int num = status->grid[row][col];
            if (num == 0) {
                // puzzle is not complete, but might be valid
                ++numMissing;
                status->complete = false;
            }
            else if (numCount[num]) {
                // doubled number puzzle is not valid nor complete
                status->valid = false;
                status->complete = false;
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

void checkPuzzle(int psize, int** grid, bool* complete, bool* valid) {

    // arguments for thread function
    t_checkpuzzle rows[9];
    t_checkpuzzle cols[9];
    t_checkpuzzle grids[9];

    // open threads for every subset of the puzzle
    pthread_t rowThreads[9];
    pthread_t colThreads[9];
    pthread_t gridThreads[9];
    for (int i = 0; i < psize; ++i) {
        // open row thread
        rows[i].idx = i + 1;
        rows[i].psize = psize;
        rows[i].grid = grid;
        rows[i].valid = true;
        rows[i].complete = true;
        pthread_create(&rowThreads[i], NULL, checkRow, &rows[i]);
        // open column thread
        cols[i].idx = i + 1;
        cols[i].psize = psize;
        cols[i].grid = grid;
        cols[i].valid = true;
        cols[i].complete = true;
        pthread_create(&colThreads[i], NULL, checkRow, &cols[i]);
        // open grid thread
        grids[i].idx = i + 1;
        grids[i].psize = psize;
        grids[i].grid = grid;
        grids[i].valid = true;
        grids[i].complete = true;
        pthread_create(&gridThreads[i], NULL, checkRow, &grids[i]);
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
        if (!rows[i].valid && !cols[i].valid && !grids[i].valid) *valid = false;
        if (!rows[i].complete && !cols[i].complete && !grids[i].complete) *complete = false;
    }
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
    checkPuzzle(sudokuSize, grid, &complete, &valid);
    printf("Complete puzzle? ");
    printf(complete ? "true\n" : "false\n");
    if (complete) {
        printf("Valid puzzle? ");
        printf(valid ? "true\n" : "false\n");
    }
    printSudokuPuzzle(sudokuSize, grid);
    deleteSudokuPuzzle(sudokuSize, grid);
    return EXIT_SUCCESS;
}