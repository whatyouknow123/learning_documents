#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
int main()
{
    double l = 4;
    double a = 8;
    double b = 10;
    int n = 1000000;
    int touch_line = 0;
    double x, y, r;
    double pi = acos(-1.0);
    clock_t start_time = clock();
    srand(time(NULL));
    int i;
    for(i = 0; i<n; i++){
        x = a*rand()/(double)RAND_MAX;
        y = b*rand()/(double)RAND_MAX;
        r = pi*rand()/(double)RAND_MAX;
		int flag = 0;
		double sin_value = sin(r)*l/2;
		double cos_value = cos(r)*l/2;
/*
		printf("the x is %f\t", x);
		printf("the y is %f\t", y);
		printf("the cos_value %f\t", cos_value);
		printf("the sin_value %f\n", sin_value);
*/
		if((x+cos_value)>=a||(x+cos_value)<=0||(x-cos_value)>=a||(x-cos_value)<=0)
			flag = 1;
		if((y+sin_value)>=b||(y+sin_value)<=0||(y-sin_value)>=b||(y-sin_value)<=0)
			flag = 1;
 //       printf("the flag is %d\n", flag);
        if(flag==1){
            touch_line++;
        }
    }
    double p = (double)touch_line/(double)n;
    printf("p is %f\n", p);
    if(p!=0){
        double mypi = (2*l*(a+b)-l*l)/(p*a*b);
        printf("my pi is %f\n", mypi);
    }
    clock_t end_time = clock();

    printf("the process time is %f", (double)(end_time-start_time)/CLOCKS_PER_SEC);

    return 0;
}
