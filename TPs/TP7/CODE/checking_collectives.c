
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

void checking_collectives(int collective_id, char * collective_name, int collective_line, char * list_forks)
{
	int size_before = -1;
	int size_after = -1;
	int rank;


//	printf("/*************************************************************/\n");
//	printf("/*************************************************************/\n");
//	printf("/******* PRINT CHECK ckecking_collectives function ***********/\n");
//	printf("/*******        - collective_id : %d           ****************/\n", collective_id);
//	printf("/*******        - collective_name : %s ************/\n", collective_name);
//	printf("/*******        _ list_forks : %s ****/\n", list_forks);
//	printf("/*************************************************************/\n");


	MPI_Comm_size(MPI_COMM_WORLD, &size_before);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	MPI_Comm new_comm;

	MPI_Comm_split(MPI_COMM_WORLD, collective_id, rank, &new_comm);

	MPI_Comm_size(new_comm, &size_after);

//	printf(" %s (l.%d) : size_before = %d, size_after = %d\n", collective_name, collective_line, size_before, size_after); fflush(stdout);


	if(size_before != size_after)
	{
		printf(" (%d) Error in collective communication pattern for %s (l.%d). \n", rank, collective_name, collective_line); fflush(stdout);
		printf(" (%d) ---- Error might come from %s.\n\n", rank, list_forks); fflush(stdout);
		MPI_Finalize();
		exit(0);
	}
}
