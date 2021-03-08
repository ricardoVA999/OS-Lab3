//Ricardo Antonio Valenzuela Avila	18762
//Sistemas Operativos Lab3

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
			if (sudoku[i][j] > 48 && sudoku[i][j] < 58) {
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

typedef struct colThread {
	int isCol;
	int value;
} colThread;

void *verifyColOrRow(void *arg) {
	omp_set_num_threads(MEASURE);
	omp_set_nested(1);

	colThread *data = (colThread*)arg;
	int isCol = data->isCol;
	int verifyCol[MEASURE] = { 1,1,1,1,1,1,1,1,1 };
	#pragma omp parallel for
	for (int i = 0; i < MEASURE; i++) {
		if (isCol == 1) {
			pid_t ttid;
			ttid = syscall(SYS_gettid);
			printf("Thread de las columanas %d\n", ttid);
		}
		int verifyNumbers[MEASURE] = { 0,0,0,0,0,0,0,0,0 };
		for (int j = 0; j < MEASURE; j++) {
			if (isCol == 1) {
				if (sudoku[j][i] > 48 && sudoku[j][i] < 58) {
					verifyNumbers[sudoku[j][i] - 49] = 1;
				}
				else {
					printf("Uno de los caracter no valido en columna");
					exit(1);
				}
			}
			else {
				if (sudoku[i][j] > 48 && sudoku[i][j] < 58) {
					verifyNumbers[sudoku[i][j] - 49] = 1;
				}
				else {
					printf("Uno de los caracter no valido en fila");
				}
			}
		}
		for (int j = 0; j < MEASURE; j++) {
			if (verifyNumbers[j] == 0) {
				verifyCol[i] = 0;
				break;
			}
		}
	}
	for (int i = 0; i < MEASURE; i++) {
		if (verifyCol[i] == 0) {
			data->value = 0;
			pthread_exit(0);
		}
	}
	data->value = 1;
	pthread_exit(0);
}

void manageFile() {
	omp_set_num_threads(SIZE - 1);
	omp_set_nested(1);
	FILE *arch = fopen("sudoku.txt", "r");
	fgets(buffer, SIZE, (FILE*)arch);

	#pragma omp parallel for collapse(2) schedule(dynamic)
	for (int i = 0; i < MEASURE; i++) {
		for (int j = 0; j < MEASURE; j++) {
			sudoku[i][j] = buffer[9 * i + j];
		}
	}
	fclose(arch);
}

int main(int argc, char const *argv[]) {
	omp_set_num_threads(MEASURE);
	omp_set_nested(1);
	manageFile();
	int res = 1;

	#pragma omp parallel for collapse(2) schedule(dynamic)
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (verifySubArray(j * 3, i * 3) == 0) {
				res = 0;
			}
		}
	}

	pid_t pid = getpid();
	pid_t pid2 = fork();

	char resp[10];

	if (pid2 == 0) {
		sprintf(resp, "%d", pid);
		execlp("ps", "ps", "-p", resp, "-Lf", NULL);
	}
	else {
		pthread_t tid;
		colThread data, data2;
		data.isCol = 1;
		pthread_create(&tid, NULL, verifyColOrRow, (void *)&data);
		pthread_join(tid, NULL);
		pid_t ttid;
		ttid = syscall(SYS_gettid);
		printf("Thread ID del main %d\n", ttid);
		wait(NULL);

		data2.isCol = 0;
		pthread_create(&tid, NULL, verifyColOrRow, (void *)&data2);
		pthread_join(tid, NULL);

		if (res == 1 && data.value == 1 && data2.value == 1) {
			printf("Sudoku Comprobado!\n");
		}
		else {
			printf("Sudoku MALO!\n");
		}

		pid_t gid = getpid();
		pid_t ggid= fork();
		char resp2[10];
		if (ggid == 0) {
			sprintf(resp2, "%d", gid);
			execlp("ps", "ps", "-p", resp2, "-lLf", NULL);
		}
		else {
			wait(NULL);
		}
	}
	return 0;
}