#include "mpi.h"
#include<stdio.h>
#include<stdlib.h>
int main (int argc, char** argv){
	MPI_Comm myworld, splitworld;
	char node_name[10];
	int resultlen;
	int my_rank, group_size, color, key;
	MPI_Init(&argc, &argv);
	MPI_Get_processor_name(node_name, &resultlen);
	printf("the name is %s\n", node_name);
	printf("the lenthg is %d\n", resultlen);
	MPI_Comm_dup(MPI_COMM_WORLD, &myworld);
	MPI_Comm_rank(myworld, &my_rank);
	MPI_Comm_size(myworld, &group_size);
	color = my_rank%3;
	key = my_rank/3;
	MPI_Comm_split(myworld, color, key, &splitworld);
	int row_rank, row_size;
	MPI_Comm_rank(splitworld, &row_rank);
	MPI_Comm_size(splitworld, &row_size);
	printf("the row rank is %d, the row size is %d \n", row_rank, row_size);
	printf("the color is %d\n", color);
	MPI_Finalize();
	return(0);
}

