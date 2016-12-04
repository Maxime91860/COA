#ifndef __VECTOR__H
#define __VECTOR__H

unsigned int* allocate_vector(unsigned int size);

void init_vector(unsigned int *vector, 
			unsigned int size, 
			unsigned int value);

void free_vector(unsigned int **vector);

void add_vectors(unsigned int *dest,
			unsigned int *src1, 
			unsigned int *src2,
			unsigned int size);
#endif
