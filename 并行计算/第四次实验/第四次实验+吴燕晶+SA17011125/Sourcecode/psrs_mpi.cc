#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>
#include<unistd.h>
#include<assert.h>
#include<sys/time.h>
#include<limits.h>

// define the compare function 
int cmp(const void *a, const void *b){
  if(*(int*)a < *(int*)b) return -1;
  if(*(int*)a > *(int*)b) return 1;
  else return 0;
}

// phase 1: sort the local array and excute the  regular sample
void phase1(int *array, int datasize, int startindex, int subarraysize, int *pivots, int size){
  // sort the subarray
  qsort(array+startindex, subarraysize, sizeof(array[0]), cmp);

  // regular sample
  for(int i=0; i<size; i++){
    pivots[i] = array[startindex+(i*(datasize/(size*size)))];
  }
  return;
}

// phase 2:the root 0 gather the message from other process
// and then sort the receive the data, and choose partiton value to
// brocast the values to other process
// each process calculate the partiton index according to the privots 
void phase2(int *array, int startindex, int subarraysize, int *pivots, int *partitionsize, int size, int myrank){
  int *collectedPivots = (int *)malloc(size*size*sizeof(pivots[0]));
  // the partition value which need to brocast
  int *phase2Pivots = (int *)malloc((size-1)*sizeof(pivots[0]));
  
  // the root process gather the message
  MPI_Gather(pivots, size, MPI_INT, collectedPivots, size, MPI_INT, 0, MPI_COMM_WORLD);

  if(myrank==0){
    // sort the regular samples
    qsort(collectedPivots, size*size, sizeof(pivots[0]), cmp);
    
    // choose the partition value
    for(int i=0; i<(size-1); i++){
      phase2Pivots[i] = collectedPivots[(((i+1)*size)+(size/2))-1];
    }
  }

  // brocast the partition value
  MPI_Bcast(phase2Pivots, size-1, MPI_INT, 0, MPI_COMM_WORLD);
  
  // partiton the local array according to the partiton value and 
  // calculate the partiton size
  int index = 0;
  for(int i=0; i<subarraysize; i++){
    if(array[startindex+i]>phase2Pivots[index]){
      index++;
    }
    if(index==size){
      partitionsize[size-1] = subarraysize - i+1;
      break;
    }
    partitionsize[index]++;
  }
  free(collectedPivots);
  free(phase2Pivots);
  return;
}

// phase 3:each process receive message from other process to confirm partition // size and the collect data from different processes
void phase3(int *array, int startindex, int *partitionsize, int **newpartitions, int *newpartitionsize, int size){
  int totalsize = 0;
  int *sendDisp = (int *)malloc(size*sizeof(int));
  int *recvDisp = (int *)malloc(size*sizeof(int));
  MPI_Barrier(MPI_COMM_WORLD);
  // all to all communicitio, each process can receive different data from other  // process
  MPI_Alltoall(partitionsize, 1, MPI_INT, newpartitionsize, 1, MPI_INT, MPI_COMM_WORLD);
  
  // calculathe the partiton size and malloc the new space
  for(int i=0; i<size; i++){
    totalsize += newpartitionsize[i];
  }
  
  *newpartitions = (int *)malloc(totalsize*sizeof(int));
  
  // calculate the relative position about sendbuf before sending partition
  // the position save the output data which need to send to process
  sendDisp[0] = 0;
  // calculate the relative position about the recvbuf, the position save
  // the data from different processes;
  recvDisp[0] = 0;
  for(int i=1; i<size; i++){
    sendDisp[i] = partitionsize[i-1] +sendDisp[i-1];
    recvDisp[i] = newpartitionsize[i-1] + recvDisp[i-1];
  }
  
  // send data, realize the point to point communication n times
  MPI_Alltoallv(&(array[startindex]), partitionsize, sendDisp, MPI_INT, *newpartitions, newpartitionsize, recvDisp, MPI_INT, MPI_COMM_WORLD);

  free(sendDisp);
  free(recvDisp);
  return;
}

//phase 4:the root process gather the sorted subarray
void phase4(int *partitions, int *partitionsize, int size, int myrank, int *array){
  int *sortedsublist;
  int *recvDisp, *indexes, *partitionends, *sublistsize, totallistsize;
 
  indexes = (int *)malloc(size*sizeof(int));
  partitionends = (int *)malloc(size*sizeof(int));
  indexes[0] = 0;
  totallistsize = partitionsize[0];
  for(int i=1; i<size; i++){
    totallistsize += partitionsize[i];
    indexes[i] = indexes[i-1] + partitionsize[i-1];
    partitionends[i-1] = indexes[i];
  }
  partitionends[size-1] = totallistsize;
  sortedsublist = (int *)malloc(totallistsize*sizeof(int));
  sublistsize = (int *)malloc(size*sizeof(int));
  recvDisp = (int *)malloc(size*sizeof(int));
  
  // merge sort
  for(int i=0; i<totallistsize; i++){
    int lowest = INT_MAX;
    int ind = -1;
    for(int j= 0; j<size; j++){
      if((indexes[j]<partitionends[j])&&(partitions[indexes[j]]<lowest)){
        lowest = partitions[indexes[j]];
        ind = j;
      }
    }
    sortedsublist[i] = lowest;
    indexes[ind] += 1;
  }
  
  // send size of all sublist to the roor process
  MPI_Gather(&totallistsize, 1, MPI_INT, sublistsize, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  // calculate the relative position of recvbuf in root process
  if(myrank==0){
    recvDisp[0] = 0;
    for(int i=1 ;i<size; i++){
      recvDisp[i] = sublistsize[i-1]+recvDisp[i-1];
    }
  }

  // send all sortd sublist to root process
  MPI_Gatherv(sortedsublist, totallistsize, MPI_INT, array, sublistsize, recvDisp, MPI_INT, 0, MPI_COMM_WORLD);
  
  // release the space
  free(partitionends);
  free(sortedsublist);
  free(indexes);
  free(sublistsize);
  free(recvDisp);
  return;
}
void psrs_mpi(int *array, int datasize){
  int myrank, size;
  int namelength;
  int startindex;
  char processorName[MPI_MAX_PROCESSOR_NAME];
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
   
  // record the excute time
  double starttime, endtime, diff;
  if(myrank==0){
    starttime = MPI_Wtime();
   // printf("the initial number is\n ");
    //for(int k=0; k<datasize; k++){
     // printf("%d ", array[k]);
    //}
   // printf("\n");
  }

  MPI_Bcast(&datasize, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(array, datasize, MPI_INT, 0, MPI_COMM_WORLD);
  //MPI_Get_processor_name(processorName, &namelength);
  
  // define the pivots
  int *pivots;
  pivots = (int *)malloc(size*sizeof(int));
  
  // define partition values
  int *partitionsize, *newpartitionsize, *newpartition;
  partitionsize = (int *)malloc(size*sizeof(int));
  newpartitionsize = (int *)malloc(size*sizeof(int));

  for(int k=0; k<size; k++){
    partitionsize[k] = 0;
  }
  
  // get the start position and sub array length
  startindex =  myrank*datasize/size;
  int endindex;
  if((myrank+1)==size){
    endindex = datasize;
  }else{
    endindex = (myrank+1)*datasize/size;
  }
  int subarraysize = endindex - startindex;
  MPI_Barrier(MPI_COMM_WORLD);  
  // the main process
  phase1(array, datasize, startindex, subarraysize, pivots, size);
  if(size>1){
    phase2(array, startindex, subarraysize, pivots, partitionsize, size, myrank);
    phase3(array, startindex, partitionsize, &newpartition, newpartitionsize, size);
    phase4(newpartition, newpartitionsize, size, myrank, array);  
  }

  // print the sorted data when datasize is equal to 64
  if(myrank==0&&datasize==64){
    printf("the result is \n");
    for(int k=0; k<datasize; k++){
      printf("%d ", array[k]);
    }
    printf("\n");
  }
  
  if(size>1){
 //   free(newpartition);
  }
  free(partitionsize);
  free(newpartitionsize);
  free(pivots);
  free(array);

  // calcute the excute time and print it out
  if(myrank==0){
    endtime = MPI_Wtime();
    diff = endtime - starttime;
    printf("the datasize is %d\n", datasize);
    printf("the excute time is %f\n", diff);
    if(datasize==1000000){
      printf("the ratio is %f\n", 0.182192/diff);
    }else if(datasize==5000000){
      printf("the ratio is %f\n", 1.143645/diff);
    }else{
      printf("the ratio is %f\n", 2.120692/diff);
    }
  }
}

int main(int argc, char* argv[]){
  // excute the psrs
  int *array;
  array = (int *)malloc(1000000*sizeof(int));  
  srand(47);
 // printf("the initial number is\n ");
  for(int k=0; k<1000000; k++){
    array[k] = rand()%10000;
  }
  MPI_Init(&argc, &argv);  
  psrs_mpi(array, 1000000);
  MPI_Finalize();
  return 0;
}
