/**
  This is a program for serial rank sort
 */
#include<stdlib.h>
#include<stdio.h>

int main(int argc, char *argv[]){
  int datasize;
  int *a, *b;
  printf("please input the number of datasize:\n");
  scanf("%d", &datasize);
  printf("\n");
  a = (int *)malloc(datasize*sizeof(int));
  b = (int *)malloc(datasize*sizeof(int));
  
  srand(47);
  // initial the Array a
  printf("input the array:\n");
  for(int i=0; i<datasize; i++){
    a[i] = ((int)rand())%10000;
    printf("%d ", a[i]);
  }
  printf("\n");
  // sort the array
  for(int i=0; i<datasize; i++){
    int k=1;
    for(int j=0; j<datasize; j++){
      if(a[i]>a[j]||(a[i]==a[j]&&i>j)){
        k++;
      }
    }
    b[k-1] = a[i];
  }
  printf("output the array:\n");
  for(int i=0; i<datasize; i++){
    printf("%d ", b[i]);
  }
  printf("\n");
  return 0;
}
