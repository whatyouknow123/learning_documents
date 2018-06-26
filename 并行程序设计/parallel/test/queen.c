#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"
#include<sys/time.h>
#include<unistd.h>
#define QUEENS 8
#define MAX_SOLUTIONS 92

const int true = 1;
const int false = 0;
typedef int bool;

enum msg_content{
	READY, 
	ACCOMPLISHED,
	NEW_TASK,
	TERMINATE
};

enum msg_tag{
	REQUEST_TAG,
	SEED_TAG,
	REPLY_TAG,
	NUM_SOLUTIONS_TAG,
	SOLUTIONS_TAG
};

int solutions[MAX_SOLUTIONS][QUEENS];
int solution_count = 0;

// judge the place is legal or not
bool collides(int row1, int col1, int row2, int col2){
	return(col1==col2)||(row1-row2 == col1-col2)||(row1+col1==row2+col2);
}

int generate_seed(){
	static int seed = 0;
	do{
		seed++;
	}while(seed<=QUEENS*QUEENS-1 && collides(0, seed/QUEENS, 1, seed%QUEENS));
	
	if(seed>QUEENS*QUEENS-1){
		return 0;
	}else{
		return seed;
	}
}

void print_solutions(int count, int solutions[][QUEENS]){
	int i, j, k;
	for(i = 0; i< count; i++){
		printf("solution %d:\n", i+1);
		for(j=0;j<QUEENS;j++){
			printf("%d", solutions[i][j]);
			for(k=0;k<solutions[i][j]; k++){
				printf("-");
			}
			printf("*");
			for(k = QUEENS-1; k>solutions[i][j]; k--){
				printf("-");
			}
			printf("\n");
		}
		printf("\n");
	}
}

bool is_safe(int chessboard[], int row, int col){
	int i;
	for(i = 0;i<row;i++){
		if(collides(i, chessboard[i], row, col)){
			return false;
		}
	}
	return true;
}

void place_queens(int chessboard[], int row){
	int i, col;
	if(row>=QUEENS){
		for(i = 0;i<QUEENS;i++){
			solutions[solution_count][i] = chessboard[i];
		}
		solution_count++;
	}else{
		for(col = 0; col<QUEENS; col++){
			if(is_safe(chessboard, row, col)){
				chessboard[row] = col;
				place_queens(chessboard, row+1);
			}
		}
	}
}

void squential_eight_queens(){
	int chessboard[QUEENS];
	solution_count = 0;
	place_queens(chessboard, 0);
	print_solutions(solution_count, solutions);
}

void eight_queens_master(int nodes){
	MPI_Status status;
	int active_slaves = nodes - 1;
	int new_task = NEW_TASK;
	int terminate = TERMINATE;
	int reply;
	int child;
	int num_solutions;
	int seed;
	
	while(active_slaves){
		MPI_Recv(&reply, 1, MPI_INT, MPI_ANY_SOURCE, REPLY_TAG, MPI_COMM_WORLD, &status);
		child = status.MPI_SOURCE;	
		if(reply == ACCOMPLISHED){
			MPI_Recv(&num_solutions, 1, MPI_INT, child, NUM_SOLUTIONS_TAG, MPI_COMM_WORLD, &status);
			if(num_solutions >0){
				MPI_Recv(solutions[solution_count], QUEENS*num_solutions, MPI_INT, child, SOLUTIONS_TAG, MPI_COMM_WORLD, &status);
				solution_count += num_solutions;
			}
		}

		seed = generate_seed();
		if(seed){
			MPI_Send(&new_task, 1, MPI_INT, child, REQUEST_TAG, MPI_COMM_WORLD);
			MPI_Send(&seed, 1, MPI_INT, child, SEED_TAG, MPI_COMM_WORLD);
		}else{
			MPI_Send(&terminate, 1, MPI_INT, child, REQUEST_TAG, MPI_COMM_WORLD);
			active_slaves--;
		}
	}
	print_solutions(solution_count, solutions);
}

void eight_queens_slave(int my_rank){
	MPI_Status status;
	int ready = READY;
	int accomplished = ACCOMPLISHED;
	bool finished = false;
	int request;
	int seed;
	int num_solutions = 0;
	int chessboard[QUEENS];
	
	MPI_Send(&ready, 1, MPI_INT, 0, REPLY_TAG, MPI_COMM_WORLD);
	
	while(!finished){
		MPI_Recv(&request, 1, MPI_INT, 0, REQUEST_TAG, MPI_COMM_WORLD, &status);
		if(request == NEW_TASK){
			MPI_Recv(&seed, 1, MPI_INT, 0, SEED_TAG, MPI_COMM_WORLD, &status);
			chessboard[0] = seed/QUEENS;
			chessboard[1] = seed%QUEENS;
			solution_count = 0;
			place_queens(chessboard, 2);
			MPI_Send(&accomplished, 1, MPI_INT, 0, REPLY_TAG, MPI_COMM_WORLD);
			MPI_Send(&solution_count, 1, MPI_INT, 0, NUM_SOLUTIONS_TAG, MPI_COMM_WORLD);
			if(solution_count>0){
				MPI_Send(*solutions, QUEENS*solution_count, MPI_INT, 0, SOLUTIONS_TAG, MPI_COMM_WORLD);
			}
		}else{
			finished = true;
		}
	}
}

int main(int argc, char* argv[]){
	struct timeval start;
	struct timeval end;
	unsigned long diff;
	int nodes, my_rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nodes);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	if (nodes==1){
		gettimeofday(&start, NULL);
		squential_eight_queens();
		gettimeofday(&end, NULL);
		diff = 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
		printf("the excute time is %ld\n", diff);
	}
	if(!my_rank){
		gettimeofday(&start, NULL);
		eight_queens_master(nodes);
	}else{
		eight_queens_slave(my_rank);
	}
	if(!my_rank){
		gettimeofday(&end, NULL);
		diff = 1000000*(end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
		printf("the excute time is %ld \n", diff);
	}
	MPI_Finalize();
	return 0;
}

