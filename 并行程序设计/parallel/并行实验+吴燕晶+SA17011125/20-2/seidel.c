#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"
#include "math.h"

#include<sys/time.h>
#include<unistd.h>

#define E 0.0001
#define a(x,y) a[x*size+y]
#define b(x) b[x]
#define v(x) v[x]
/*A为size*size的系数矩阵*/
#define A(x,y) A[x*size+y]
#define B(x) B[x]
#define V(x) V[x]
#define intsize sizeof(int)
#define floatsize sizeof(float)
#define charsize sizeof(char)

int size,N;
int m;
float *B;
float *A;
float *V;
int my_rank;
int p;
MPI_Status status;
FILE *fdA,*fdB,*fdB1;
int i,j,my_rank,group_size;
int k;
float *sum;
float *b;
float *v;
float *a;
float *differ;
float temp;
int iteration,total,loop;
int r,q;

/*
 * 函数名:Environment_Finalize
 * 功能：释放各个进程的a,b,v
 */
void Environment_Finalize(float *a,float *b,float *v)
{
    free(a);
    free(b);
    free(v);
}

/*
 * 函数名: read_data()
 * 功能：读取数据
 */

void read_data()
{
    if(my_rank==0)
    {
        /*打开数据文件*/
        fdA=fopen("dataIn.txt","r");
        //fscanf(fdA,"%d %d", &size, &N);
        
        printf("the size:\n");
        scanf("%d", &size);
        printf("the size is %d", size);
        printf("the N is:\n");
        scanf("%d", &N);
        printf("the N is %d\n", N);
       
        if (size != N-1)
        {
            printf("the input is wrong\n");
            exit(1);
        }

        /*分配空间*/
        A=(float *)malloc(floatsize*size*size);
        B=(float *)malloc(floatsize*size);
        V=(float *)malloc(floatsize*size);

        /*初始化矩阵A和向量B*/
        for(i = 0; i < size; i++)
        {
            for(j = 0; j < size; j++)
                //fscanf(fdA,"%f", A+i*size+j);
                A(i, j) = 1.0;
            //fscanf(fdA,"%f", B+i);
            B(i) = 1.0;
        }

        /*初始化向量V*/
        for(i = 0; i < size; i ++)
            //fscanf(fdA,"%f", V+i);
           V(i) = 1.0;
        /*关闭数据文件*/
        fclose(fdA);

        /*输出数据文件信息*/
        printf("Input of file \"dataIn.txt\"\n");
        printf("%d\t%d\n", size, N);

        /*输出矩阵A和向量B的值*/
        for(i = 0; i < size; i ++)
        {
            for(j = 0; j < size; j ++)
                printf("%f\t",A(i,j));
            printf("%f\n",B(i));
        }

        printf("\n");

        /*输出解向量V的值*/
        for(i = 0; i < size; i ++)
            printf("%f\t", V(i));

        printf("\n\n");
        printf("\nOutput of result\n");
    }
}

/*
 * 函数名: broadcast_data()
 * 功能：广播数据
 */
void broadcast_data()
{
    /*0号进程将size广播给所有进程*/
    MPI_Bcast(&size,1,MPI_INT,0,MPI_COMM_WORLD);
    m=size/p;
    if (size%p!=0) m++;

    /*为每个进程分配需要的空间*/
    v=(float *)malloc(floatsize*size);
    a=(float *)malloc(floatsize*m*size);
    b=(float *)malloc(floatsize*m);
    sum=(float *)malloc(floatsize*m);

    /*判断是否分配成功*/
    if (a==NULL||b==NULL||v==NULL)
    {
        printf("allocate space  fail!");
        exit(1);
    }

    /*如果是0号进程，就将本进程的解向量初始化为问题的解向量*/
    if (my_rank==0)
    {
        for(i=0;i<size;i++)
            v(i)=V(i);
    }

    /*初始解向量v被广播给所有进程*/
    MPI_Bcast(v,size,MPI_FLOAT,0,MPI_COMM_WORLD);

    /*0号进程采用行块划分将矩阵A划分为大小为m*size的p块子矩阵,
      将B划分为大小为m的p块子向量，依次发送给1至p-1号进程*/
    if (my_rank==0)
    {
        /*初始化0号进程的a和b*/
        for(i=0;i<m;i++)
            for(j=0;j<size;j++)
                a(i,j)=A(i,j);
        for(i=0;i<m;i++)
            b(i)=B(i);

        /*将各自的a和b发送到1至p-1号进程*/
        for(i=1;i<p;i++)
        {
            MPI_Send(&(A(m*i,0)), m*size,MPI_FLOAT,i,i,MPI_COMM_WORLD);
            MPI_Send(&(B(m*i)),m, MPI_FLOAT,i,i,MPI_COMM_WORLD);
        }

        /*释放空间*/
        free(A); free(B); free(V);
    }
    else
    {
        /*如果不是0号进程，就接收从0号进程传来的子矩阵a和子向量b*/
        MPI_Recv(a,m*size,MPI_FLOAT,0,my_rank,MPI_COMM_WORLD, &status);
        MPI_Recv(b,m,MPI_FLOAT,0,my_rank, MPI_COMM_WORLD,&status);
    }
}

/*
 * 函数名: cal_cross_data()
 * 功能：主对角元素右边的数据求和
 */
void cal_cross_data()
{
    /*所有进程并行地对主对角元素右边的数据求和*/
    for(i=0;i<m;i++)
    {
        sum[i]=0.0;
        for(j=0;j<size;j++)
            if (j>(my_rank*m+i))
                sum[i]=sum[i]+a(i,j)*v(j);
    }
}


/*
 * 函数名: die_dai_cal()
 * 功能：各个进程并行迭代求解
 */
void die_dai_cal()
{
    /*total为新旧值之差小于E的x的分量的个数*/
    while (total<size)
    {
        iteration=0;
        /*iteration为本进程中新旧值之差小于E的x的分量个数*/
        total=0;
        for(j=0;j<size;j++)
        {
            r=j%m;
            q=j/m;
            /*编号为q的进程负责计算解向量并广播给所有进程*/
            if (my_rank==q)                       /*主行所在的进程*/
            {
                temp=v(my_rank*m+r);
                for(i=0;i<r;i++)
                    sum[r]=sum[r]+a(r,my_rank*m+i)*v(my_rank*m+i);

                /*计算出的解向量*/
                v(my_rank*m+r)=(b(r)-sum[r]) / a(r,my_rank*m +r);
                if (fabs(v(my_rank*m+r)-temp)<E)
                    iteration++;

                /*广播解向量*/
                MPI_Bcast(&v(my_rank*m+r), 1,MPI_FLOAT,my_rank,MPI_COMM_WORLD);
                sum[r]=0.0;
                for(i=0;i<r;i++)
                    sum[i]=sum[i]+a(i,my_rank*m+r)*v(my_rank*m+r);
            }
            else                                  /*其他进程接受广播来的x[j]的新值*/
            /*各进程对解向量的其它分量计算有关乘积项并求和*/
            {
                MPI_Bcast(&v(q*m+r),1, MPI_FLOAT,q,MPI_COMM_WORLD);
                for(i=0;i<m;i++)
                    sum[i]=sum[i]+a(i,q*m +r)*v(q*m+r);
            }
        }

        /*通过归约操作的求和运算以决定是否进行下一次迭代*/
        /*allreduce过程求出所有进程iteration和total，并广播到所有进程*/
        MPI_Allreduce(&iteration,&total,1,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);
        loop++;
        if (my_rank==0)
            printf("in the %d times total vaule = %d\n",loop,total);
    }
}

/*
 * 函数名: put_data
 * 功能：输出数据
 */
void put_data()
{
    if (my_rank==0)
    {
        for(i = 0; i < size; i ++)
            printf("x[%d] = %f\n",i,v(i));
        printf("\n");
        /*输出迭代次数*/
        printf("Iteration num = %d\n",loop);
    }
}

/*
 * 函数名: main
 * 功能：主程序
 * 输入：argc为命令行参数个数；
 *       argv为每个命令行参数组成的字符串数组。
 * 输出：返回0代表程序正常结束；
 */
int main(int argc, char **argv)
{
    double starttime, endtime, time;

    loop = total = 0;

    /*MPI程序初始化*/
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &group_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    p=group_size;
    if(my_rank == 0){
        starttime = MPI_Wtime();
    }
    /*0号进程负责读取数据*/
    read_data();
    /*广播数据*/
    broadcast_data();
    /*所有进程并行地对主对角元素右边的数据求和*/
    cal_cross_data();
    /*迭代求解*/
    die_dai_cal();
    /*0号进程将结果输出*/
    put_data();

    MPI_Barrier(MPI_COMM_WORLD);
    if(my_rank == 0){
        endtime = MPI_Wtime();
        printf("the excute time is %f\n", endtime-starttime);
    }
    MPI_Finalize();
    Environment_Finalize(a,b,v);
    return (0);
}
