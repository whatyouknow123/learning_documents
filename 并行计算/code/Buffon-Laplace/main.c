#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
int out_of_grid(int x, int y, int r, int l, int a, int b){
    int flag = 0;
    double sin_value = sin(r)*l/2;
    double cos_value = cos(r)*l/2;
    if((x+cos_value)>=a||(x+cos_value)<=0||(x-cos_value)>=a||(x-cos_value)<=0)
        flag = 1;
    if((y+sin_value)>=b||(y+sin_value)<=0||(y-sin_value)>=b||(y-sin_value)<=0)
        flag = 1;
    return flag;
}
int main()
{
    int l = 2;
    double a = 10;
    double b = 10;
    int n = 1000000;
    int touch_line = 0;
    double x, y, r;
    double pi = acos(-1.0);
    clock_t start_time = clock();
    srand(time(NULL));
    int i;
    for(i = 0; i<n; i++){
        x = a*rand()/RAND_MAX;
        y = b*rand()/RAND_MAX;
        r = pi*rand()/RAND_MAX;
        if(out_of_grid(x, y, r, l, a, b)==1){
            touch_line++;
        }
    }
    double p = touch_line/n;
    printf("p is %f", p);
    double mypi = (2*l*(a+b)-l*l)/(p*a*b);
    clock_t end_time = clock();
    printf("my pi is %f\n", mypi);
    printf("the process time is %f", (double)(end_time-start_time)/CLOCKS_PER_SEC);

    return 0;
}
