/**
 * \file main.c
 * \brief main TD1 - part1 GCC
*/

#include <stdlib.h>
#include <stdio.h>
#include "compute.h"

#define VECTOR_SIZE 32
#define REPEAT (unsigned int)3E7

/**
 * \brief Print header and info
*/
void print_info() {
	printf("******************************************************\n");
	printf("          CHPS - module COA - TD1: part1 GCC         \n");
	printf("   Add two vectors and store values in a third one    \n");
	printf("   Vectors size: %d\n", VECTOR_SIZE);
	printf("   Repeat %u times\n", REPEAT);
	printf("******************************************************\n");
}


int main (int argc, char **argv) {
	print_info();

	compute(VECTOR_SIZE, REPEAT);

	//float useless = 0;

}
