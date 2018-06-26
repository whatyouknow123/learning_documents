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
/*AΪsize*size��ϵ������*/
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
 * ������:Environment_Finalize
 * ���ܣ��ͷŸ������̵�a,b,v
 */
void Environment_Finalize(float *a,float *b,float *v)
{
    free(a);
    free(b);
    free(v);
}

/*
 * ������: read_data()
 * ���ܣ���ȡ����
 */

void read_data()
{
    if(my_rank==0)
    {
        /*�������ļ�*/
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

        /*����ռ�*/
        A=(float *)malloc(floatsize*size*size);
        B=(float *)malloc(floatsize*size);
        V=(float *)malloc(floatsize*size);

        /*��ʼ������A������B*/
        for(i = 0; i < size; i++)
        {
            for(j = 0; j < size; j++)
                //fscanf(fdA,"%f", A+i*size+j);
                A(i, j) = 1.0;
            //fscanf(fdA,"%f", B+i);
            B(i) = 1.0;
        }

        /*��ʼ������V*/
        for(i = 0; i < size; i ++)
            //fscanf(fdA,"%f", V+i);
           V(i) = 1.0;
        /*�ر������ļ�*/
        fclose(fdA);

        /*��������ļ���Ϣ*/
        printf("Input of file \"dataIn.txt\"\n");
        printf("%d\t%d\n", size, N);

        /*�������A������B��ֵ*/
        for(i = 0; i < size; i ++)
        {
            for(j = 0; j < size; j ++)
                printf("%f\t",A(i,j));
            printf("%f\n",B(i));
        }

        printf("\n");

        /*���������V��ֵ*/
        for(i = 0; i < size; i ++)
            printf("%f\t", V(i));

        printf("\n\n");
        printf("\nOutput of result\n");
    }
}

/*
 * ������: broadcast_data()
 * ���ܣ��㲥����
 */
void broadcast_data()
{
    /*0�Ž��̽�size�㲥�����н���*/
    MPI_Bcast(&size,1,MPI_INT,0,MPI_COMM_WORLD);
    m=size/p;
    if (size%p!=0) m++;

    /*Ϊÿ�����̷�����Ҫ�Ŀռ�*/
    v=(float *)malloc(floatsize*size);
    a=(float *)malloc(floatsize*m*size);
    b=(float *)malloc(floatsize*m);
    sum=(float *)malloc(floatsize*m);

    /*�ж��Ƿ����ɹ�*/
    if (a==NULL||b==NULL||v==NULL)
    {
        printf("allocate space  fail!");
        exit(1);
    }

    /*�����0�Ž��̣��ͽ������̵Ľ�������ʼ��Ϊ����Ľ�����*/
    if (my_rank==0)
    {
        for(i=0;i<size;i++)
            v(i)=V(i);
    }

    /*��ʼ������v���㲥�����н���*/
    MPI_Bcast(v,size,MPI_FLOAT,0,MPI_COMM_WORLD);

    /*0�Ž��̲����п黮�ֽ�����A����Ϊ��СΪm*size��p���Ӿ���,
      ��B����Ϊ��СΪm��p�������������η��͸�1��p-1�Ž���*/
    if (my_rank==0)
    {
        /*��ʼ��0�Ž��̵�a��b*/
        for(i=0;i<m;i++)
            for(j=0;j<size;j++)
                a(i,j)=A(i,j);
        for(i=0;i<m;i++)
            b(i)=B(i);

        /*�����Ե�a��b���͵�1��p-1�Ž���*/
        for(i=1;i<p;i++)
        {
            MPI_Send(&(A(m*i,0)), m*size,MPI_FLOAT,i,i,MPI_COMM_WORLD);
            MPI_Send(&(B(m*i)),m, MPI_FLOAT,i,i,MPI_COMM_WORLD);
        }

        /*�ͷſռ�*/
        free(A); free(B); free(V);
    }
    else
    {
        /*�������0�Ž��̣��ͽ��մ�0�Ž��̴������Ӿ���a��������b*/
        MPI_Recv(a,m*size,MPI_FLOAT,0,my_rank,MPI_COMM_WORLD, &status);
        MPI_Recv(b,m,MPI_FLOAT,0,my_rank, MPI_COMM_WORLD,&status);
    }
}

/*
 * ������: cal_cross_data()
 * ���ܣ����Խ�Ԫ���ұߵ��������
 */
void cal_cross_data()
{
    /*���н��̲��еض����Խ�Ԫ���ұߵ��������*/
    for(i=0;i<m;i++)
    {
        sum[i]=0.0;
        for(j=0;j<size;j++)
            if (j>(my_rank*m+i))
                sum[i]=sum[i]+a(i,j)*v(j);
    }
}


/*
 * ������: die_dai_cal()
 * ���ܣ��������̲��е������
 */
void die_dai_cal()
{
    /*totalΪ�¾�ֵ֮��С��E��x�ķ����ĸ���*/
    while (total<size)
    {
        iteration=0;
        /*iterationΪ���������¾�ֵ֮��С��E��x�ķ�������*/
        total=0;
        for(j=0;j<size;j++)
        {
            r=j%m;
            q=j/m;
            /*���Ϊq�Ľ��̸��������������㲥�����н���*/
            if (my_rank==q)                       /*�������ڵĽ���*/
            {
                temp=v(my_rank*m+r);
                for(i=0;i<r;i++)
                    sum[r]=sum[r]+a(r,my_rank*m+i)*v(my_rank*m+i);

                /*������Ľ�����*/
                v(my_rank*m+r)=(b(r)-sum[r]) / a(r,my_rank*m +r);
                if (fabs(v(my_rank*m+r)-temp)<E)
                    iteration++;

                /*�㲥������*/
                MPI_Bcast(&v(my_rank*m+r), 1,MPI_FLOAT,my_rank,MPI_COMM_WORLD);
                sum[r]=0.0;
                for(i=0;i<r;i++)
                    sum[i]=sum[i]+a(i,my_rank*m+r)*v(my_rank*m+r);
            }
            else                                  /*�������̽��ܹ㲥����x[j]����ֵ*/
            /*�����̶Խ��������������������йس˻�����*/
            {
                MPI_Bcast(&v(q*m+r),1, MPI_FLOAT,q,MPI_COMM_WORLD);
                for(i=0;i<m;i++)
                    sum[i]=sum[i]+a(i,q*m +r)*v(q*m+r);
            }
        }

        /*ͨ����Լ��������������Ծ����Ƿ������һ�ε���*/
        /*allreduce����������н���iteration��total�����㲥�����н���*/
        MPI_Allreduce(&iteration,&total,1,MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);
        loop++;
        if (my_rank==0)
            printf("in the %d times total vaule = %d\n",loop,total);
    }
}

/*
 * ������: put_data
 * ���ܣ��������
 */
void put_data()
{
    if (my_rank==0)
    {
        for(i = 0; i < size; i ++)
            printf("x[%d] = %f\n",i,v(i));
        printf("\n");
        /*�����������*/
        printf("Iteration num = %d\n",loop);
    }
}

/*
 * ������: main
 * ���ܣ�������
 * ���룺argcΪ�����в���������
 *       argvΪÿ�������в�����ɵ��ַ������顣
 * ���������0�����������������
 */
int main(int argc, char **argv)
{
    double starttime, endtime, time;

    loop = total = 0;

    /*MPI�����ʼ��*/
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &group_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    p=group_size;
    if(my_rank == 0){
        starttime = MPI_Wtime();
    }
    /*0�Ž��̸����ȡ����*/
    read_data();
    /*�㲥����*/
    broadcast_data();
    /*���н��̲��еض����Խ�Ԫ���ұߵ��������*/
    cal_cross_data();
    /*�������*/
    die_dai_cal();
    /*0�Ž��̽�������*/
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
