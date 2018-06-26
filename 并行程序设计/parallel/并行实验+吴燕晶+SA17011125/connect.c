/**���㷨�д�������Ŀ��С��ͼ�Ķ�����**/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

/**ʹ�ö�̬������ڴ�洢�ڽӾ�������Ϊ�궨��**/
#define A(i,j) A[i*N+j]

/**����Ϊ N:������  n:������������Ķ�����  p:��������**/
int N;
int n;
int p;

int *D,*C;
int *A;
int temp;
int myid;
MPI_Status status;

/**�����Ҫ��Ϣ**/
void print(int *P)
{
    int i;
    if(myid==0)
    {
        for(i=0;i<N;i++)
            printf("%d ",P[i]);
        printf("\n");
    }
}


/**�����ڽӾ���**/
void readA()
{
    char *filename;
    int i,j;
    printf("\n");
    printf("Input the vertex num:\n");
    scanf("%d",&N);
    n=N/p;
    if(N%p!=0) n++;
    A=(int*)malloc(sizeof(int)*(n*p)*N);
    if(A==NULL)
    {
        printf("Error when allocating memory\n");
        exit(0);
    }
    for(i=0;i<N;i++)
       for(j =0;j<N;j++)
          A(i, j) = 1;
    /*
    printf("Input the adjacent matrix:\n");
    for(i=0;i<N;i++)
        for(j=0;j<N;j++)
            scanf("%d",&A(i,j));
    */
    for(i=N;i<n*p;i++)
        for(j=0;j<N;j++)
            A(i,j)=0;
}


/**������0�㲥�ض�����**/
void bcast(int *P)
{
    MPI_Bcast(P,N,MPI_INT,0,MPI_COMM_WORLD);
}


/**������ȡ��С����ѧ����**/
int min(int a,int b)
{
    return(a<b?a:b);
}


/**Ϊ����������С���ڽӳ����㣬��Ӧ�㷨����(2.1)**/
void D_to_C()
{
    int i,j;

    // change the code into omp code
    int nthreads = omp_get_num_threads();
    #pragma omp parallel private(i)
    {
        int tid = omp_get_thread_num();     
    	for(i=tid;i<n;i+=nthreads)
    	{
        	C[n*myid+i]=N+1;
        	for(j=0;j<N;j++)
            	if((A(i,j)==1)&&(D[j]!=D[n*myid+i])&&(D[j]<C[n*myid+i]))
        	{
            	C[n*myid+i]=D[j];
        	}
        	if(C[n*myid+i]==N+1)
   	         C[n*myid+i]=D[n*myid+i];
   	 }
    }
}


/**Ϊ������������С�ڽӳ����㣬��Ӧ�㷨����(2.2)**/
void C_to_C()
{
    int i,j;

    int nthreads = omp_get_num_threads();
    #pragma omp parallel private(i)
    {
	int tid = omp_get_thread_num();
    	for(i=tid;i<n;i+=nthreads)
    	{
        	temp=N+1;
        	for(j=0;j<N;j++)
            	if((D[j]==n*myid+i)&&(C[j]!=n*myid+i)&&(C[j]<temp))
        	{
            	temp=C[j];
        	}
        	if(temp==N+1) temp=D[n*myid+i];
        	C[myid*n+i]=temp;
    	}
    }
}


/**�����������ʶ**/
void CC_to_C()
{
    int i;
    #pragma omp for
    for(i=0;i<n;i++)
        C[myid*n+i]=C[C[myid*n+i]];

}


/**�����µĳ����㣬��Ӧ�㷨����(2.5)**/
void CD_to_D()
{
    int i;
    #pragma omp for
    for(i=0;i<n;i++)
        D[myid*n+i]=min(C[myid*n+i],D[C[myid*n+i]]);

}


/**�ͷŶ�̬�ڴ�**/
void freeall()
{
    #pragma omp sections
    {
	#pragma omp section
    	free(A);
	#pragma omp section
    	free(D);
	#pragma omp section
    	free(C);
    }
}


/**������**/
void main(int argc,char **argv)
{
    int i,j,k;
    double l;
    int group_size;

	/**���±���������¼����ʱ��**/
    double starttime,endtime;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&group_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    p=group_size;
    MPI_Barrier(MPI_COMM_WORLD);
    if(myid==0)
        starttime=MPI_Wtime();

	/**������0���ڽӾ���**/
    if(myid==0)
        readA();
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);
    if(myid!=0)
    {
        n=N/p;
        if(N%p!=0) n++;
    }

    D=(int*)malloc(sizeof(int)*(n*p));
    C=(int*)malloc(sizeof(int)*(n*p));
    if(myid!=0)
        A=(int*)malloc(sizeof(int)*n*N);

	/**��ʼ������D������(1)**/
    for(i=0;i<n;i++)
        D[myid*n+i]=myid*n+i;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(&D[myid*n],n,MPI_INT,D,n,MPI_INT,0,MPI_COMM_WORLD);
    bcast(D);

    MPI_Barrier(MPI_COMM_WORLD);

	/**������0���������������ͱ�Ҫ����**/
    if(myid==0)
        for(i=1;i<p;i++)
            MPI_Send(&A(i*n,0),n*N,MPI_INT,i,i,MPI_COMM_WORLD);
    else
        MPI_Recv(A,n*N,MPI_INT,0,myid,MPI_COMM_WORLD,&status);
    MPI_Barrier(MPI_COMM_WORLD);

    l=log(N)/log(2);

	/**��ѭ���壺�㷨����(2)**/
    for(i=0;i<l;i++)
    {
        if(myid==0) printf("Stage %d:\n",i+1);

		/**�㷨����(2.1)**/
        D_to_C();
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Gather(&C[n*myid],n,MPI_INT,C,n,MPI_INT,0,MPI_COMM_WORLD);
        print(C);
        bcast(C);
        MPI_Barrier(MPI_COMM_WORLD);

		/**�㷨����(2.2)**/
        C_to_C();
        print(C);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Gather(&C[n*myid],n,MPI_INT,C,n,MPI_INT,0,MPI_COMM_WORLD);
        MPI_Gather(&C[n*myid],n,MPI_INT,D,n,MPI_INT,0,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

		/**�㷨����(2.3)**/
        if(myid==0)
            for(j=0;j<n;j++)
                D[j]=C[j];

		/**�㷨����(2.4)**/
        for(k=0;k<l;k++)
        {
            bcast(C);
            CC_to_C();
            MPI_Gather(&C[n*myid],n,MPI_INT,C,n,MPI_INT,0,MPI_COMM_WORLD);
        }
        bcast(C);
        bcast(D);

		/**�㷨����(2.5)**/
        CD_to_D();
        MPI_Gather(&D[n*myid],n,MPI_INT,D,n,MPI_INT,0,MPI_COMM_WORLD);
        print(D);
        bcast(D);
    }

    if(myid==0) printf("Result: \n");
    print(D);
    if(myid==0)
    {
        endtime=MPI_Wtime();
        printf("The running time is %f\n",endtime-starttime);
    }

    freeall();
    MPI_Finalize();

}
