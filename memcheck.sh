rm -f bin/sudoku.out
gcc -Wall -Wextra src/sudoku.c -o bin/sudoku.out -lm -g3 
valgrind ./bin/sudoku.out tests/puzzle9-valid.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle2-valid.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle2-invalid.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle2-fill-valid.txt -s --leak-check=full
valgrind ./bin/sudoku.out tests/puzzle-hard.txt -s --leak-check=full
