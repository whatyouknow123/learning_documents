/*
 * define a function to calcute pi
 * by using the mpi
 * it also record the used_time
 */
#include<mpi.h>
#include<stdio.h>
#include<sys/time.h>
#include<stdlib.h>

int values(int n, double w){
  double pi=0.0, local=0.0;
  double temp;
  long i;
  int myrank, nprocs;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  double starttime, endtime, time1, time2;
  if(myrank==0){
    printf("the n is %d\n", n);
    starttime = MPI_Wtime();
  }
  
  for(i = myrank; i<n; i+=nprocs){
    temp = (i+0.5)*w;
    local += 4.0/(1.0+temp*temp);
  }
  MPI_Reduce(&local, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if(myrank==0){
    printf("the parallel pi is %lf\n", pi*w);
    endtime = MPI_Wtime();
    time1 = endtime - starttime;
    printf("the parallel time = %lf\n", time1);
  }

  starttime = MPI_Wtime();
  pi = 0.0;
  if(myrank==0){
    for(i=0; i<n; i++){
       local = (i+0.5)*w;
       pi = pi+4.0/(1.0+local*local);
    }
    endtime = MPI_Wtime();
    time2 = endtime - starttime;
    printf("the serial pi = %lf\n", pi*w);
    printf("the serial time = %lf\n", time2);
    printf("the rate = %lf\n", time2/time1);
  }
}

int main(int argc, char* argv[]){
  MPI_Init(&argc, &argv);
  values(1000, 1.0/1000);
  values(10000, 1.0/10000);
  values(50000, 1.0/50000);
  values(100000, 1.0/100000);
  MPI_Finalize();
  return 0;
}
