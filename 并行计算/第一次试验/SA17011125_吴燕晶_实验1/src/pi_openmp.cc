/*
 * this code is design for appropriate pi
 * by using the openmp
 * it also record the cost time
 */

#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<sys/time.h>

void values(int n){
  double local, pi, w;
  long i;
  double time_used, time_used1;
  struct timeval tv_start, tv_end;
  printf("the n is %d\n", n);
  gettimeofday(&tv_start, NULL);
  w = 1.0/n;
  pi = 0.0;
  #pragma omp parallel for reduction(+:pi) private(local)
  for(i=0; i<n; i++){
    local = (i+0.5)*w;
    pi += 4.0/(1.0+local*local);
  }

  gettimeofday(&tv_end, NULL);
  time_used = (tv_end.tv_sec - tv_start.tv_sec)*1000000+(tv_end.tv_usec - tv_start.tv_usec);
  printf("parallel: the pi is %lf\n", pi*w);
  printf("parallel: time_used = %lf sec\n", time_used/1000000);
  time_used1 = time_used;
  
  gettimeofday(&tv_start, NULL);
  
  w = 1.0/n;
  pi = 0.0;
  for(i=0; i<n; i++){
    local = (i+0.5)*w;
    pi = pi+4.0/(1.0+local*local);
  }

  gettimeofday(&tv_end, NULL);
  time_used = (tv_end.tv_sec - tv_start.tv_sec)*1000000+(tv_end.tv_usec - tv_start.tv_usec);
  printf("serial: the pi is  %lf\n", pi*w);
  printf("serial: time_used = %lf sec\n", time_used/1000000);
  printf("the rate = %lf\n",time_used/time_used1);
}

int main(int argc, char* argv[]){
 // int num_threads;
 // printf("the num_threads is:\n");
 // scanf("%d", &num_threads);
 // omp_set_num_threads(num_threads);
  values(1000);
  values(10000);
  values(50000);
  values(100000);
  return 0;
}
