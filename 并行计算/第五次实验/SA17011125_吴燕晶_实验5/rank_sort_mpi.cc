#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<climits>
#include<string.h>

// get the datasize
int GetDataSize(){
  int i;
  while(1){
    printf("please input the data length:\n");
    scanf("%d", &i);
    if((i>0)&&(i<INT_MAX)){
      break;
    }
    printf("you input the wrong data size");
  }
  return i;
}

void CountRank(int *data_in, int datasize, int mylength, int *rank, int sumid, int myid){
  int i, j;
  int start, end;
  start = datasize/sumid*myid;
  end = start + mylength;
  for(j=start; j<end; j++){
    rank[j-start]=0;
    for(i=0; i<datasize; i++){
      if((data_in[j]>data_in[i])||((data_in[j]==data_in[i])&&(j>i))){
        rank[j-start]++;
      }
    }
  }
}

int main(int argc, char *argv[]){
  int datasize,mylength;
  int *data_in, *data_out;
  int *rank;
  int myid, sumid;
  int i, j;
  double starttime, endtime, diff;
  MPI_Status status;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &sumid);
  
  // get the datasize and brocast
  if(myid == 0){
    starttime = MPI_Wtime();
    datasize = GetDataSize();
  }
  MPI_Bcast(&datasize, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // every process initialize the parameters
  mylength = datasize/sumid;
  if(myid==sumid-1){
    mylength = datasize - mylength*(sumid-1);
  }
  data_in = (int*)malloc(datasize*sizeof(int));
  if(data_in==0){
    printf("malloc memory error");
  }
  if(myid == 0){
    data_out = (int*)malloc(datasize*sizeof(int));
    if(data_out == 0){
      printf("malloc memort error");
    }
    rank = (int *)malloc(datasize*sizeof(int));
    if(rank==0){
      printf("malloc memory error");
    }
  }else{
    rank = (int *)malloc(mylength*sizeof(int));
    if(rank==0){
      printf("malloc memory error");
    }
  }

  // the root process initialize the array data_in
  if(myid==0){
    srand(47);
    for(i=0; i<datasize; i++){
      data_in[i] = ((int)rand())%10000;
    }
  }
  
  //brocast the data_in array to every process
  MPI_Bcast(data_in, datasize, MPI_INT, 0, MPI_COMM_WORLD);
  
  //every process calcute the rank of the elements which they have
  CountRank(data_in, datasize, mylength, rank, sumid, myid);
  
  // gather all sorted data from other process
  if(myid==0){
    for(i=1;i<sumid;i++){
      if(i==sumid-1){
        MPI_Recv(rank+mylength*i, datasize-mylength*(sumid-1), MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      }else{
        MPI_Recv(rank+mylength*i, mylength, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      }
    }
  }else{
    MPI_Send(rank, mylength, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  // reallocate the position of all elements
  if(myid==0){
    for(i=0; i<datasize; i++){
      data_out[rank[i]] = data_in[i];
    }
  }

  // calculate the time and the ratio
  if(myid==0){
    endtime = MPI_Wtime();
    diff = endtime-starttime;
    printf("the diff time is %f\n", diff);
  }
  free(data_in);
  free(rank);
  if(myid==0){
    free(data_out);
  }
  MPI_Finalize();
  return 1;
}
