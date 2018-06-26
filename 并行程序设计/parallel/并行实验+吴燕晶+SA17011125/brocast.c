#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"
#include<string.h>

int main(int argc, char** argv){
	int rank, group_size, i;
	int *arr = NULL;
	char *allhost;
	char host[128];
	int color, key;
	int resultlen;
	MPI_Comm mycomm,  nodecomm;
	MPI_Comm head;
	long len = 10000;
	double starttime, endtime, diff;

	//init and get the rank and size
	MPI_Init(&argc, &argv);
	MPI_Comm_dup(MPI_COMM_WORLD, &mycomm);
	MPI_Comm_rank(mycomm, &rank);
	MPI_Comm_size(mycomm, &group_size);
		

	//define the arr
	arr = (int*)malloc(len*sizeof(int));
	memset(arr, 0, len);
	if(rank == 0){
		starttime = MPI_Wtime();
		for(i = 0; i<len; i++){
			arr[i] = 1;
		}
	}
	// get the processor name 
	allhost = (char*)malloc(128*group_size);	
	MPI_Get_processor_name(host, &resultlen);
	//printf("the node name is %s\n", host);
	MPI_Allgather(host, 128, MPI_BYTE, allhost, 128, MPI_BYTE, mycomm);
	
	//get the color and key
	color = 0;
	key = 0;
	for(i = 0; i<rank; i++){
		if(strcmp(allhost+i*128, host)) continue;
		else key++;
	}
	for(i = 0; i<=rank; i++){
		if(!strcmp(allhost+i*128, host)) break;
		
	}	
	color = i;
	MPI_Comm_split(mycomm, color, key, &nodecomm);
	
	//test the value
	int temp_rank, temp_size;
	MPI_Comm_rank(nodecomm, &temp_rank);
	MPI_Comm_size(nodecomm, &temp_size);
	/*
	printf("the total rank is %d\n", rank);
	printf("the nodecomm size is %d, the nodecomm rank is %d\n", temp_size, temp_rank);
	*/

	// define the arr
	/*if(rank ==0){
		starttime = MPI_Wtime();
		arr = (int*)malloc(len*sizeof(int));
		memset(arr, 0, len*sizeof(int));
		for(i =0; i<len; i++){
			arr[i] = 1;
		}	
	}*/
	// create the head
	if(temp_rank == 0){
		MPI_Comm_split(mycomm, 0, rank, &head);
		int head_size, head_rank;
		MPI_Comm_rank(head, &head_rank);
		MPI_Comm_size(head, &head_size);
		//printf("the head size is %d, the head rank is %d\n", head_size, head_rank);
		
		// brocast in the head
		if(head_rank == 0){
			printf("the root 0  process begin brocast\n");
			for(i = 0;i< head_size; i++){
				if(i != head_rank){
					MPI_Send(arr, len, MPI_INT, i, 0, head);
					//printf("the root send data to head rank %d\n", i);
				}
			}
		}else{
			MPI_Recv(arr, len, MPI_INT, 0, 0, head, MPI_STATUS_IGNORE);
			//printf("head process receive data from root\n");
			printf("process %d receive data\n", rank);
		}
		
		//brocast int the node
		for(i = 0; i<temp_size; i++){
			if(i != temp_rank){
				MPI_Send(arr, len, MPI_INT, i, 0, nodecomm);
				//printf("the node root send data to node rank %d\n", i);
			}
		}
	}else{
		MPI_Comm_split(mycomm, MPI_UNDEFINED, rank, &head);
		MPI_Recv(arr, len, MPI_INT, 0, 0, nodecomm, MPI_STATUS_IGNORE);
		//printf("receive data from node root\n");
		printf("process %d receive data\n", rank);
	}
	
	if(rank == 0){
		endtime = MPI_Wtime();
		diff = endtime - starttime;
		printf("the excutetime is %f\n", diff);
	}
	
	//finialize the mpi
	free(arr);
	MPI_Finalize();
	return 0;
}


