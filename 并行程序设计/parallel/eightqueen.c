#include <stdio.h>
#include <stdlib.h>

#define max 8

int queen[max], sum=0; /* max is the biggest queen*/
int success = 0;
void show() /* show the all solve */
{
    int i;
    for(i = 0; i < max; i++)
    {
         printf("(%d,%d) ", i, queen[i]);
    }
    printf("\n");
    sum++;
}

int check(int n) /* check the local queen is illegal or not */
{
    int i;
    for(i = 0; i < n; i++) /* check the col ,col45 and col135 is legal */
    {
        if(queen[i] == queen[n] || abs(queen[i] - queen[n]) == (n - i))
        {
            return 0;
        }
    }
    return 1;
}

void put(int n) /* traceback the queen, n means we are put the nth queen */
{
    int i;
    for(i = 0; i < max; i++)
    {
        queen[n] = i; /* put the queen in current i col */
        if(check(n)) /* we can put the queen in the i col*/
        {
            if(n == max - 1)
            {
                show(); /* we have put the all queen*/
                success = 1;
            }
            else
            {
                put(n + 1); /* put the next queen */
            }
        }
    }
}

void backtrace(int stepvegas){
    int i = stepvegas;
    int stacks[max+1];
    int j = 0;
    int m = 0;
    int step = 0;
    int val;
    while(i<max){
        for(m=j;m<max;m++){
                queen[i]=m;
            if(check(i)){
                break;
            }
        }
        if(m<max){
            queen[i] = m;
            stacks[step] = m;
            step++;
            i++;
            j=0;
        }else{
            if(step!=0){
                step--;
                val = stacks[step];
                i--;
                queen[i]=0;
                j = val+1;
            }else{
                success = 0;
                break;
            }

        }
    }
}

void queenlv(int stepvegas){
    int k = 0;
    int nb;
    int i;
    int randnum;
    int j = 0;
    srand(time(NULL));
    do{
        nb = 0;
        for(i = 0;i<max;i++){
            queen[k] = i;
            if(check(k)){
                nb = nb+1;
                randnum = rand()%max + 1;
                if(randnum == 1){
                    j = i;
                }
            }

        }
        if(nb > 0){
            queen[k]=j;
            k++;
        }
    }while(nb != 0&&k!=stepvegas);
    if(nb > 0){
        backtrace(stepvegas);
    }else{
        success = 0;
    }
}

void obstinate(int stepvegas){
    int i = 0;
    int time = 1;
    do{
        for(i;i<max;i++){
            queen[i] = 0;
        }
        queenlv(stepvegas);
        printf("%d\n",time);
        time++;
    }while(success == 0);
}

int main()
{
    obstinate(1);
    printf("%d", sum);
    return 0;
}
