#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

int main(){
	int i, n, chunk;
	float a[100], b[100], result;
	n=4;
	chunk = 10;
	result = 0.0;
	for(i=0; i<n;i++){
		a[i] = i*1.0;
		b[i] = i*2.0;
	}
	#pragma omp parallel for default(shared) private(i) schedule(static, chunk) reduction(+:result)
	for(i = 0; i<n;i++){
		result = result + (a[i]*b[i]);
	}
	printf("Final result = %f\n", result);
}
