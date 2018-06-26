/*
 * define a function to count the number of prime which value <= n
 * by using the mpi
 */
#include<mpi.h>
#include<stdio.h>
#include<sys/time.h>
#include<stdlib.h>
#include<math.h>

//judge if n is prime or not
//return 1 means i is prime else means i is not prime
int isPrime(int n){
  int flag = 1; 
  int middle = sqrt(n*1.0);
  for(int i=2; i<=middle; i++){
    if(n%i == 0){
      flag = 0;
      break;
    }
  }
  return flag;
}

int values(int n){
  int i, number, sum, mynumber;
  int myrank, nprocs;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  double starttime, endtime, time1, time2;
  if(myrank==0){
    printf("the n is %d\n", n);
    starttime = MPI_Wtime();
  }
  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  sum = 0;
  for(i=myrank*2+1; i<=n; i+=nprocs*2){
    sum += isPrime(i);
  }
  mynumber = sum;
  MPI_Reduce(&mynumber, &number, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if(myrank==0){
    printf("the result is %d\n", number);
    endtime = MPI_Wtime();
    time1 = endtime - starttime;
    printf("the parallel time = %lf\n", time1);
  }

  if(myrank==0){
    sum = 0;
    starttime = MPI_Wtime();
    for(i=1; i<=n; i+=2){
       sum += isPrime(i);
    }
    endtime = MPI_Wtime();
    time2 = endtime - starttime;
    printf("the serial time = %lf\n", time2);
    printf("the rate = %lf\n", time2/time1);
  }
}

int main(int argc, char* argv[]){
  MPI_Init(&argc, &argv);
  values(1000);
  values(10000);
  values(100000);
  values(500000);
  MPI_Finalize();
  return 0;
}
