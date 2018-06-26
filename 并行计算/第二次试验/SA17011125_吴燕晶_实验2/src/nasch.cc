/*
 * traffic simulation using the nasch model and parallel the program
 * by mpi
 * author: wuyanjing date:2018/5/12
 */
#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<time.h>
#include<string.h>

// the vmax is the max speed of a vertical;
#define vmax 5

// the L is the length of the road
#define L 5000000

// the propability of sudden stop
#define p 0.3

#define keyvalue -99
// define the nasch parallel model
// the N is the number of verticals
// the Te is the effective number of time steps

void naschparallel(long N, int Te){
  double start, end, diff;
  int myid, myprocs;

  // Allocation and Initial
  // initialize the speed of verticals and positions
  int *tempv;
  int *temppos;
  
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_size(MPI_COMM_WORLD, &myprocs);
  
  int len = N/myprocs;
  
  tempv = (int*)malloc(len*sizeof(int));
  temppos = (int*)malloc(len*sizeof(int));
  if(temppos!=NULL&&tempv!=NULL){
    memset(tempv, 0, sizeof(int)*len);
    memset(temppos, 0, sizeof(int)*len);      
  }else{
   printf("malloc failure");
   return;
  }

  if(myid==0){
    start = MPI_Wtime();
    //initial position
    for(int i= 0; i<len; i++){
      temppos[i] = i + myid*len;
    }
  }else{
    for(int i=0; i<len; i++){
      temppos[i] = i+myid*len;
    }
    MPI_Send(&temppos[0], 1, MPI_INT, myid-1, 0, MPI_COMM_WORLD);
  }

  int k = 1;
  // dynamic change the speed and the position
  while(k<=Te){
    
    // change the speed and position of all verticals 
    for(int i = 0; i<len; i++){
      
      // suppose v = v+1
      if(tempv[i]<vmax){
        tempv[i]++;
      }
      
      // define v = d - 1
      int d;
      if(i==(len-1)&&myid!=(myprocs-1)){
        MPI_Recv(&d, 1, MPI_INT, myid+1, k-1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        d = d - temp[i]; 
     }else if(i==(len-1)&&myid==(myprocs-1)){
        d = L - temppos[i];
      }else{
        d = temppos[i+1] - temppos[i];
      }
      d--;
      if(tempv[i]>d){
        tempv[i] = d; 
      }
      // random sudden deceleration
      int t = rand()%100;
      if(t<30){
        if((tempv[i]-1)>=0){
          tempv[i]--;
        }
      }
     
      // new postition
      temppos[i]+=tempv[i];

      // send new position of temppos[0]
      if(i==0&&myid!=0){
        MPI_Send(&temppos[0], 1, MPI_INT, myid-1, k, MPI_COMM_WORLD);
      }
    }
    k++;
  }
  
  if(myid == 0){  
    end = MPI_Wtime();
    diff = end - start;
    printf("the N is %ld\n", N);
    printf("the Te is %d\n", Te);
    printf("the diff time is %lf\n", diff);
    if(N==100000){
      printf("the rate is %lf\n", 4.509401/diff);
    }else if(N==500000){
      printf("the rate is %lf\n", 5.679087/diff);
    }else{
      printf("the rate is %lf\n", 6.769731/diff);
    }
    printf("----------------------------\n");
  }

  free(temppos);
  free(tempv);
}

int main(int argc, char* argv[]){
  srand((unsigned)(time(NULL)));
  MPI_Init(&argc, &argv);
  naschparallel(100000, 2000);
  naschparallel(500000, 500);
  naschparallel(1000000, 300);
  MPI_Finalize();
  return 0;
}
