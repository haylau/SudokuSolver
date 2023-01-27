rm -f bin/sudoku.out
gcc -Wall -Wextra -g3 src/sudoku.c -o bin/sudoku.out -lm -pthread
valgrind ./bin/sudoku.out tests/puzzle9-valid.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle2-valid.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle2-invalid.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle2-fill-valid.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle-hard.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle-hard2.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle-hard3.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle-hard4.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle-invalid-hard4.txt -s --leak-check=full
