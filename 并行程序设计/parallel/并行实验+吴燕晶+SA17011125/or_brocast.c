#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "mpi.h"

int main(int argc, char **argv){
	int rank, group_size, i;
	MPI_Comm mycomm;
	int *arr = NULL;
	double starttime, endtime,diff; 
	long len=10000;
	// initical the mpi
	MPI_Init(&argc, &argv);
	MPI_Comm_dup(MPI_COMM_WORLD, &mycomm);
	MPI_Comm_rank(mycomm, &rank);
	MPI_Comm_size(mycomm, &group_size);
	
	// define the arr

	arr = (int*)malloc(len*sizeof(int));
	memset(arr, 0, len);

	//brocast the value
	if(rank ==0){
		printf("the length is :\n");
		//scanf("%ld", &len);
		//arr = (int*)malloc(len*sizeof(int));
		//memset(arr, 0, len);
		// get the excute time 
		starttime = MPI_Wtime();

		for(i = 0; i<len;i++){
			arr[i] = 1;
		}
		printf("the root process has send the data\n");
	}

	MPI_Bcast(arr, len, MPI_INT, 0, mycomm);
	MPI_Barrier(mycomm);

	if(rank != 0){
		printf("the process %d  has receive the data\n", rank);
	}

	free(arr);
	MPI_Finalize();
	
	if(rank == 0){
		endtime = MPI_Wtime();
		diff = endtime - starttime;
		printf("the excute time is  %f\n", diff);
	}
	return 0;
}

