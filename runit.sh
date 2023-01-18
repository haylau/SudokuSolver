#!/bin/bash

# Script to compile and run sudoku program
rm -f bin/sudoku.out
gcc -Wall -Wextra src/sudoku.c -o bin/sudoku.out -lm
./sudoku.out tests/puzzle9-valid.txt
./sudoku.out tests/puzzle2-valid.txt
./sudoku.out tests/puzzle2-invalid.txt
./sudoku.out tests/puzzle2-fill-valid.txt
./sudoku.out tests/puzzle-hard.txt

# to check for memory leaks, use
# valgrind ./sudoku puzzle9-good.txt

# to fix formating use
# clang-format -i main.c

# if clang-format does not work 
# use 'source scl_source enable llvm-toolset-7.0' and try again

# if using GitHub, you can run the program on GitHub servers and see
# the result. Repository > Actions > Run Workflow


