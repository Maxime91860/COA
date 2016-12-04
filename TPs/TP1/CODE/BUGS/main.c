/**
 * \file main.c
 * \brief main TD1 - part1 GCC
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct fibo_s {
	unsigned int result;
	unsigned int n_minus_1;
	unsigned int n_minus_2;
	unsigned int max; 
} Fibo;

/**
 * \brief Print header and info
*/
void print_info() {
	printf("******************************************************\n");
	printf("          CHPS - module COA - TD1: part3 GDB         \n");
	printf("          mean floored - factorial - fibonacci        \n");
	printf("******************************************************\n");
}


unsigned int factorial(int val) {
	if (val < 1) {
		return 1;
	} else {
		return factorial(val-1)*val;
	}
}


void fibonacci(Fibo *fibo_values, unsigned int n) {
	switch (n) {
		case 0: 
			fibo_values->result = fibo_values->n_minus_1;
			break;
		case 1: 
			fibonacci(fibo_values, n-1);
			fibo_values->result = fibo_values->n_minus_2;
			break;
		default:
			fibonacci(fibo_values, n-1);
			fibo_values->result = fibo_values->n_minus_1 
					+ fibo_values->n_minus_2;
			fibo_values->n_minus_2 = fibo_values->n_minus_1;
			fibo_values->n_minus_1 = fibo_values->result;
			break;
	} 
}


void launch_fibonacci(Fibo *fibo_values, unsigned int max) {
	fibo_values->max = max;
	fibo_values->n_minus_1 = 1;
	fibo_values->n_minus_2 = 0;
	fibonacci(fibo_values, max);
}

unsigned int floor_mean(unsigned int *list, unsigned int nb) {
	unsigned int result = 0;
	unsigned int i;

	for (i=0; i<nb; i++) {
		result += list[i];
	}

	// memset(&nb,0,sizeof(unsigned int));
	result /=nb;

	return result;
}

int main (int argc, char **argv) {
	unsigned int i, value;
	print_info();


	// Exercice 1: mean (florred) of 100 values
	unsigned int *list = malloc(sizeof(unsigned int)*100);
	for (i=0; i<100; i++) {
		list[i] = 3*i+1;
	}
	value = floor_mean(list, 100);
	free(list);
	printf("1) mean value = %d\n", value);


	// Exercice 2: facorial 
	value = factorial(4);
	printf("2) factorial value = %d\n", value);


	// Exercice 3: another factorial
	value = factorial(-1);
	printf("3) Another factorial value = %d\n", value);


	// Exercice 4 & 5: fibonacci
	Fibo *fibo_values;
	// fibo_values = (Fibo *) malloc (sizeof(Fibo));
	launch_fibonacci(fibo_values, 6);
	printf("4) fibonacci value F%d = %d\n", 6, fibo_values->result);

	return 0;
}
