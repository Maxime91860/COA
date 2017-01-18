#include <stdio.h>
#include <stdlib.h>

#include <mpi.h>

extern void checking_collectives(int collective_id, char * collective_name, int collective_line, char * list_forks);

int main(int argc, char * argv[])
{
	MPI_Init(&argc, &argv);

	int i;
	int a = 2;
	int b = 3;
	int c=0;

	int rank = -1;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for(i=0; i<10; i++)
	{

		MPI_Barrier(MPI_COMM_WORLD);

		if(c<10)
		{
//			printf("je suis dans le if (c=%d)\n", c);
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
			if(c <5)
			{
				a = a*a +1 + rank;
			}
			else
			{
				MPI_Barrier(MPI_COMM_WORLD);
				c = c*3*rank;
				if(c <20)
				{
//					return c;
				}
			}

		}
		else
		{
//			printf("je suis dans le else (c=%d)\n", c);
			MPI_Barrier(MPI_COMM_WORLD);
//			return 1;
		}
		c+= (a+b+rank);	
//		printf("fin du for\n");
	}

//	printf("c=%d\n", c);

	MPI_Finalize();
	return 1;
}
