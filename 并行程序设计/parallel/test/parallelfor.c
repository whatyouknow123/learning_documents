#include<stdio.h>
#include<omp.h>

int main(){
	int i;
	int n = 100;
	int a[100], b[100], c[100];
	for(i=0;i<n;i++){
		a[i] = b[i] = i;
	}
	#pragma omp parallel for private(i)
		
		for(i =0;i<n;i++){
			c[i] = a[i]+b[i];
			printf("the %d th is %d\n", i, c[i]);
		}
	
	
	return 0;
}
