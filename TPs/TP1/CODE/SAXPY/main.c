#include <stdio.h>
#include <stdlib.h>



void saxpy(int * res, int * x, int * y, int a, int size)
{
	int i;
	for(i=0; i<size; i++)
	{
		res[i] = a*x[i] + y[i];
	}
}


int main(int argc, char * argv[])
{
	int i;
	int size = 100;
	int * res, *x, *y;
	int a = 2;

	res = malloc(sizeof(int) * size);
	x = malloc(sizeof(int) * size);
	y = malloc(sizeof(int) * size);
	
	for(i=0; i<size; i++)
	{
		x[i] = 50 + i;
		y[i] = i;
	}

	saxpy(res, x, y, a, size);

	for(i=0; i<100; i++)
	{
		printf("res[%d] = %d ; ", i, res[i]);
		if((i+1)%10 == 0) printf("\n");
	}

	return 1;
}
