/**
 * \file vector.c
 * \brief Vector operations
*/
#include "vector.h"
#include "malloc.h"

/**
 * \brief Allocate a vector with the specified size
 * \param size size of the vector to allocate
 * \return ptr to the allocated vector
*/
unsigned int* allocate_vector(unsigned int size) {
	unsigned int *vector;
	vector = (unsigned int*) malloc(sizeof(unsigned int)*size);

	return vector;
}


/**
 * \brief Free an allocated vector and set its pointer to NULL
 * \param vector ptr of ptr to the vector
*/
void free_vector(unsigned int **vector) {
	free(*vector);
	*vector = NULL;
}


/**
 * \brief Initialize a vector with one value
 * \param vector vector to initialize
 * \param size size of the vector
 * \param value value to set
*/
void init_vector(unsigned int *vector, 
			unsigned int size, 
			unsigned int value) {

	unsigned int i;

	for (i=0; i<size; i++) {
		vector[i] = value;
	}
}


/**
 * \brief Add two vectors and store result in a third one
 * \param dest vector where results are stored
 * \param src1 vector 1
 * \param src2 vector 2
 * \param size size of all vectors
*/
void add_vectors(unsigned int *dest,
			unsigned int *src1, 
			unsigned int *src2,
			unsigned int size) {
	unsigned int i;

	for (i=0; i<size; i++) {
		dest[i] = src1[i] + src2[i];
	} 
}
