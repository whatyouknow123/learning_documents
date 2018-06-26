/*
 * this code is design for calculte the number of prime which is <= n
 * by using the openmp
 * it also record the cost time
 */

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>
#include<math.h>
#include<sys/time.h>
// judge if n is prime or not
int isPrime(int n){
  int flag = 1;
  int middle = sqrt((double)n);
  int i;
  for(i=2; i<=middle; i++){
    if(n%i==0){
      flag = 0;
      break;
    }
  }
  return flag;
}

void values(int n){
  int i;
  int number=0;
  double time_used, time_used1;
  struct timeval tv_start, tv_end;
  printf("the n is %d\n", n);

  // below is parallel code
  gettimeofday(&tv_start, NULL);
  #pragma omp parallel for schedule(dynamic, 64) reduction(+:number)
  for(i=1; i<=n; i+=2){
    number += isPrime(i);
  }
  
  gettimeofday(&tv_end, NULL);
  time_used = (tv_end.tv_sec - tv_start.tv_sec)*1000000+(tv_end.tv_usec - tv_start.tv_usec);
  printf("parallel: the number of prime is %d\n", number);
  printf("parallel: time_used = %lf sec\n", time_used/1000000);
  // use the time_used1 to record parallel time
  time_used1 = time_used;
  
  // below is serial code
  number = 0;
  gettimeofday(&tv_start, NULL);
  int j;
  for(j=1; j<=n; j+=2){
    number += isPrime(j);
  }
  gettimeofday(&tv_end, NULL);
  time_used = (tv_end.tv_sec - tv_start.tv_sec)*1000000+(tv_end.tv_usec - tv_start.tv_usec);
  printf("serial: the number of prime is %d\n", number);
  printf("serial: time_used = %lf sec\n", time_used/1000000);
  printf("the rate = %lf\n",time_used/time_used1);
}

int main(int argc, char* argv[]){
  int n = 1000;
  if(n<2){
    printf("no prime");
  }else if(n==2){
    printf("there is only one prime 2");
  }else{
    values(1000);
    values(10000);
    values(100000);
    values(500000);
    values(10000000);
    return 0;
  }
}
