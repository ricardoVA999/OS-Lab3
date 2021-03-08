#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <omp.h>
#include <pthread.h>

#define MEASURE 9
#define SIZE 82

char buffer[SIZE];
char sudoku[MEASURE][MEASURE];

int verifySubArray(int col, int row) {
	omp_set_num_threads(SIZE - 1);
	omp_set_nested(1);
	int verifyNumbers[MEASURE] = { 0,0,0,0,0,0,0,0,0 };

	#pragma omp parallel for collapse(2) schedule (dynamic)
	for (int i = col; i < col + 3; i++) {
		for (int j = row; j < row + 3; j++) {
			if (sudoku[i][j] > 48 && grid[i][j] < 58) {
				verifyNumbers[sudoku[i][j] - 49] = 1;
			}
			else {
				printf("Uno de los catactares no es un numero del 1 al 9");
				exit(1);
			}
		}
	}
	for (int i = 0; i < MEASURE; i++) {
		if (verifyNumbers[i] == 0) {
			return 0;
		}
	}
	return 1;
}

int main() {
	printf("Hola mundo");
}