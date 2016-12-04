/**
 * \file compute.c
 * \brief Allocate, add vectors and print results
*/
#include <stdlib.h>
#include <stdio.h>
#include "compute.h"
#include "vector.h"
#include "timer.h"


/**
 * \brief Print vectors to check if V1 + V2 = V3
 * \param v1 source vector V1
 * \param v2 source vector V2
 * \param v3 result vector V3
 * \param size size of all vectors
*/
void print_results(unsigned int *v1,
			unsigned int *v2,
			unsigned int *v3,
			unsigned int size) {

	unsigned int i;

	printf("\tV1\t+\tV2\t=\tV3\n");
	printf("\t----------------------------------\n");

	for (i=0; i<size; i++) {
		printf("\t%d\t \t%d\t \t%g\n", 
				v1[i], v2[i], v3[i]);
	}
}


/**
 * \brief Allocate, compute V3 = V1 + V2, print results and time spent
 * \param vector_size vector size of all vectors
 * \param repeat repeat operation "repeat" times to get meaningful duration 
*/
void compute (unsigned int vector_size, unsigned int repeat) {
	unsigned int *V1, *V2, *V3;
	unsigned int i;
	TIMER_INIT
 
	V1 = allocate_vector(vector_size);
	V2 = allocate_vector(vector_size);
	V3 = allocate_vector(vector_size);

	// Initialize each vecto
	init_vector(V1, vector_size, 1);
	init_vector(V2, vector_size, 2);
	init_vector(V3, vector_size, 0);

	// V3 = V1 + V2
	TIMER_START
	for (i=0; i<repeat; i++) {
		add_vectors(V3, V1, V2, vector_size);
	}
	TIMER_END

	print_results(V1, V2, V3, vector_size);
	TIMER_PRINT	

	free_vector(&V1);
	free_vector(&V2);
	free_vector(&V3);
}
