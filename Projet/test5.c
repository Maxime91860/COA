
#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>


int main(int argc, char * argv[])
{
	MPI_Init(&argc, &argv);
	int rank; MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank%2 == 0){
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Allreduce (NULL, NULL, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	}


	MPI_Finalize();

}