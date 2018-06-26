#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"
#include "math.h"
#include "string.h"
#define E 0.00001
#define min -1
#define intsize sizeof(int)
#define charsize sizeof(char)
#define floatsize sizeof(float)
#define A(x,y) A[x*N+y]
#define I(x,y) I[x*N+y]
#define a(x,y) a[x*N+y]
#define e(x,y) e[x*N+y]
#define b(x) b[x]
#define buffer(x,y) buffer[x*N+y]
#define buffee(x,y) buffee[x*N+y]
int M,N;
int *b;
int m,p;
int myid,group_size;
float *A,*I;
float *a,*e;
float max;
float sum;
double starttime,time1,time2,time3,endtime;
MPI_Status status;

float sgn(float v)
{
    float f;
    if (v>=0) f=1;
    else f=-1;
    return f;
}


void read_fileA()
{
    int i,j;

    FILE *fdA;
    time1=MPI_Wtime();

    fdA=fopen("dataIn.txt","r");
    //fscanf(fdA,"%d %d", &M, &N);
    printf("input the M:\n");
    scanf("%d", &M);
    printf("inut the N:\n");
    scanf("%d", &N);
    if(M != N)
    {
        puts("The input is error!");
        exit(0);
    }

    m=N/p; if (N%p!=0) m++;
    A=(float*)malloc(floatsize*N*m*p);
    I=(float*)malloc(floatsize*N*m*p);

    for(i = 0; i < M; i ++)
    {
        for(j = 0; j < M; j ++) 
          //fscanf(fdA, "%f", A+i*M+j);
          A(i, j) = i+j;
    }
    fclose(fdA);

    printf("Input of file \"dataIn.txt\"\n");
    printf("%d\t %d\n",M, N);
    for(i=0;i<M;i++)
    {
        for(j=0;j<N;j++) printf("%f\t",A(i,j));
        printf("\n");
    }

    for(i=0;i<N;i++)
    {
        for(j=0;j<N;j++)
        {
            if (i==j) I(i,j)=1;
            else I(i,j)=0;
        }
    }
}


void send_a()
{
    int i;
    for(i=1;i<p;i++)
    {
        MPI_Send(&(A(m*i,0)),m*N,MPI_FLOAT,i,i,MPI_COMM_WORLD);
        MPI_Send(&(I(m*i,0)),m*N,MPI_FLOAT,i,i,MPI_COMM_WORLD);
    }
    free(A);
    free(I);
}


void get_a()
{
    int i,j;
    if (myid==0)
    {
        for(i=0;i<m;i++)
            for(j=0;j<N;j++)
        {
            a(i,j)=A(i,j);
            e(i,j)=I(i,j);
        }
    }
    else
    {
        MPI_Recv(a,m*N,MPI_FLOAT,0,myid,MPI_COMM_WORLD,&status);
        MPI_Recv(e,m*N,MPI_FLOAT,0,myid,MPI_COMM_WORLD,&status);
    }
}


int main(int argc,char **argv)
{
    float *c;
    int k;
    int loop;
    int i,j,v,z,r,t,y;
    int i1,j1;
    float f,g,h;
    float sin1,sin2,cos1;
    float s1,c1;
    float *br,*bt,*bi,*bj,*zi,*zj;
    float *temp1,*temp2,*buffer,*buffee;
    int counter,current;
    int *buf;
    int mml,mpl;
    float bpp,bqq,bpq,bqp;
    float lmax;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&group_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    if (myid==0) starttime=MPI_Wtime();
    p=group_size;

    max=10.0;
    loop=0;

    if (myid==0) read_fileA();

    MPI_Bcast(&N,1,MPI_INT,0,MPI_COMM_WORLD);

    m=N/p;
    if (N%p!=0) m++;

    a=(float*)malloc(floatsize*m*N);
    e=(float*)malloc(floatsize*m*N);
    b=(int*)malloc(intsize*m);
    br=(float*)malloc(floatsize*N);
    bt=(float*)malloc(floatsize*N);
    bi=(float*)malloc(floatsize*m);
    bj=(float*)malloc(floatsize*m);
    zi=(float*)malloc(floatsize*m);
    zj=(float*)malloc(floatsize*m);
    temp1=(float*)malloc(floatsize*4);
    temp2=(float*)malloc(floatsize*4*p);

    if ((myid==p-1)&&(myid%2!=0))
    {
        buffer=(float*)malloc(floatsize*m/2*N);
        buffee=(float*)malloc(floatsize*m/2*N);
        buf=(int*)malloc(intsize*m/2);            /* store i,j */
    }

    if ((myid%2!=0)&&(myid!=p-1))
    {
        buffer=(float*)malloc(floatsize*m*N);
        buffee=(float*)malloc(floatsize*m*N);
        buf=(int*)malloc(intsize*m);
    }

    if (myid==0)
    {
        get_a();
        send_a();
    }
    else
    {
        get_a();
    }

    if (myid==0)                                  /* start computing now */
        time2=MPI_Wtime();

    for(i=0;i<m;i++)
        b(i)=myid*m+i;

    while (fabs(max)>E)
    {
        loop++;
        for(i=myid*m;i<(myid+1)*m-1;i++)
            for(j=i+1;j<=(myid+1)*m-1;j++)
        {
            r=i%m; t=j%m;                         /* to make a(r,j) zero use a(r,i) a(t,j) */
            if(a(r,j)!=0)
            {
                f=(-a(r,j));
                g=(a(t,j)-a(r,i))/2;
                h=sgn(g)*f/sqrt(f*f+g*g);
                sin2=h;
                sin1=h/sqrt(2*(1+sqrt(1-h*h)));
                cos1=sqrt(1-sin1*sin1);

                bpp=a(r,i)*cos1*cos1+a(t,j)*sin1*sin1+a(r,j)*sin2;
                bqq=a(r,i)*sin1*sin1+a(t,j)*cos1*cos1-a(r,j)*sin2;
                bpq=0; bqp=0;

                for(v=0;v<N;v++)                  /* compute row */
                    if ((v!=i)&&(v!=j))
                {
                    br[v]=a(r,v)*cos1+a(t,v)*sin1;
                    a(t,v)=(-a(r,v))*sin1+a(t,v)*cos1;
                }

                for(v=0;v<N;v++)                  /* row */
                    if ((v!=i)&&(v!=j))
                {
                    a(r,v)=br[v];
                }

                for(v=0;v<m;v++)
                    br[v]=e(v,i)*cos1+e(v,j)*sin1;

                for(v=0;v<m;v++)
                    e(v,j)=e(v,i)*(-sin1)+e(v,j)*cos1;

                for(v=0;v<m;v++)
                    e(v,i)=br[v];

                for(v=0;v<m;v++)                  /* compute col */
                    if ((v!=r)&&(v!=t))
                {
                    bi[v]=a(v,i)*cos1+a(v,j)*sin1;
                    a(v,j)=(-a(v,i))*sin1+a(v,j)*cos1;
                }

                for(v=0;v<m;v++)                  /* col */
                    if ((v!=r)&&(v!=t))
                        a(v,i)=bi[v];

                a(r,i)=bpp;
                a(t,j)=bqq;
                a(r,j)=bpq;
                a(t,i)=bqp;

                temp1[0]=sin1;
                temp1[1]=cos1;
                temp1[2]=(float)i;
                temp1[3]=(float)j;
            }
            else
            {
                temp1[0]=0.0;
                temp1[1]=0.0;
                temp1[2]=0.0;
                temp1[3]=0.0;
            }                                     /* end of if */

            MPI_Allgather(temp1,4,MPI_FLOAT,temp2,4,MPI_FLOAT,MPI_COMM_WORLD);
            current=0;

            for(v=1;v<=p;v++)
            {
                s1=temp2[(v-1)*4+0];
                c1=temp2[(v-1)*4+1];
                i1=(int)temp2[(v-1)*4+2];
                j1=(int)temp2[(v-1)*4+3];

                if ((s1!=0.0)||(c1!=0.0)||(i1!=0)||(j1!=0))
                {
                    if (myid!=current)
                    {

                        for(z=0;z<m;z++)
                        {
                            zi[z]=a(z,i1)*c1 + a(z,j1)*s1;
                            a(z,j1)=-a(z,i1)*s1 + a(z,j1)*c1;
                        }

                        for(z=0;z<m;z++)
                            a(z,i1)=zi[z];

                        for(z=0;z<m;z++)
                            zi[z]=e(z,i1)*c1+e(z,j1)*s1;

                        for(z=0;z<m;z++)
                            e(z,j1)=-e(z,i1)*s1+e(z,j1)*c1;

                        for(z=0;z<m;z++)
                            e(z,i1)=zi[z];

                    }                             /* if myid!=current */

                }                                 /* if */

                current=current+1;
            }                                     /* for v */
        }                                         /* for i,j */

        for(counter=1;counter<=2*p-2;counter++)
        {

            if (myid==0)
            {
                MPI_Send(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                MPI_Send(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                MPI_Send(&b(m/2),m/2,MPI_INT,myid+1,myid+1,MPI_COMM_WORLD);

                MPI_Recv(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&b(m/2),m/2,MPI_INT,myid+1,myid,MPI_COMM_WORLD,&status);
            }

            if ((myid==p-1)&&(myid%2!=0))
            {
                for(i=m/2;i<m;i++)
                    for(j=0;j<N;j++)
                        buffer((i-m/2),j)=a(i,j);

                for(i=m/2;i<m;i++)
                    for(j=0;j<N;j++)
                        buffee((i-m/2),j)=e(i,j);

                for(i=m/2;i<m;i++)
                    buf[i-m/2]=b(i);

                for(i=0;i<m/2;i++)
                    for(j=0;j<N;j++)
                        a((i+m/2),j)=a(i,j);

                for(i=0;i<m/2;i++)
                    for(j=0;j<N;j++)
                        e((i+m/2),j)=e(i,j);

                for(i=0;i<m/2;i++)
                    b(m/2+i)=b(i);

                MPI_Recv(&(a(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(e(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&b(0),m/2,MPI_INT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Send(buffer,m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                MPI_Send(buffee,m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                MPI_Send(buf,m/2,MPI_INT,myid-1,myid-1,MPI_COMM_WORLD);
            }

            if ((myid==p-1)&&(myid%2==0))
            {
                MPI_Send(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                MPI_Send(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                MPI_Send(&b(m/2),m/2,MPI_INT,myid-1,myid-1,MPI_COMM_WORLD);

                for(i=0;i<m/2;i++)
                    for(j=0;j<N;j++)
                        a((i+m/2),j)=a(i,j);

                for(i=0;i<m/2;i++)
                    for(j=0;j<N;j++)
                        e((i+m/2),j)=e(i,j);

                for(i=0;i<m/2;i++)
                    b(i+m/2)=b(i);

                MPI_Recv(&(a(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(e(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&b(0),m/2,MPI_INT,myid-1,myid,MPI_COMM_WORLD,&status);
            }

            if ((myid!=0)&&(myid!=p-1))
            {
                if(myid%2==0)
                {
                    MPI_Send(&(a(0,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                    MPI_Send(&(e(0,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                    MPI_Send(&b(0),m/2,MPI_INT,myid+1,myid+1,MPI_COMM_WORLD);
                    MPI_Send(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                    MPI_Send(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                    MPI_Send(&b(m/2),m/2,MPI_INT,myid-1,myid-1,MPI_COMM_WORLD);

                    MPI_Recv(&(a(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&(e(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&b(0),m/2,MPI_INT,myid-1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&b(m/2),m/2,MPI_INT,myid+1,myid,MPI_COMM_WORLD,&status);
                }

                if(myid%2!=0)
                {
                    for(i=0;i<m;i++)
                        for(j=0;j<N;j++)
                            buffer(i,j)=a(i,j);

                    for(i=0;i<m;i++)
                        for(j=0;j<N;j++)
                            buffee(i,j)=e(i,j);

                    for(i=0;i<m;i++)
                        buf[i]=b(i);

                    MPI_Recv(&(a(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&(e(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&b(0),m/2,MPI_INT,myid-1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                    MPI_Recv(&b(m/2),m/2,MPI_INT,myid+1,myid,MPI_COMM_WORLD,&status);

                    MPI_Send(&(buffer(0,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                    MPI_Send(&(buffee(0,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                    MPI_Send(&buf[0],m/2,MPI_INT,myid+1,myid+1,MPI_COMM_WORLD);
                    MPI_Send(&(buffer(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                    MPI_Send(&(buffee(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                    MPI_Send(&buf[m/2],m/2,MPI_INT,myid-1,myid-1,MPI_COMM_WORLD);
                }
            }

            for(i=0;i<m/2;i++)
                for(j=m/2;j<m;j++)
            {
                if (a(i,b(j))!=0)
                {
                    f=-a(i,b(j));
                    g=(a(j,b(j))-a(i,b(i)))/2;
                    h=sgn(g)*f/sqrt(f*f+g*g);
                    sin2=h;
                    sin1=h/sqrt(2*(1+sqrt(1-h*h)));
                    cos1=sqrt(1-sin1*sin1);

                    bpp=a(i,b(i))*cos1*cos1+a(j,b(j))*sin1*sin1+a(i,b(j))*sin2;
                    bqq=a(i,b(i))*sin1*sin1+a(j,b(j))*cos1*cos1-a(i,b(j))*sin2;
                    bpq=0; bqp=0;

                    for(v=0;v<N;v++)              /* compute row */
                        if ((v!=b(i))&&(v!=b(j)))
                    {
                        br[v]=a(i,v)*cos1+a(j,v)*sin1;
                        a(j,v)=-a(i,v)*sin1+a(j,v)*cos1;
                    }

                    for(v=0;v<N;v++)              /* row */
                        if ((v!=b(i))&&(v!=b(j)))
                            a(i,v)=br[v];

                    for(v=0;v<m;v++)
                        br[v]=e(v,b(i))*cos1+e(v,b(j))*sin1;

                    for(v=0;v<m;v++)
                        e(v,b(j))=e(v,b(i))*(-sin1)+e(v,b(j))*cos1;

                    for(v=0;v<m;v++)
                        e(v,b(i))=br[v];

                    for(v=0;v<m;v++)              /* compute col */
                        if ((v!=i)&&(v!=j))
                    {
                        bi[v]=a(v,b(i))*cos1+a(v,b(j))*sin1;
                        a(v,b(j))=-a(v,b(i))*sin1+a(v,b(j))*cos1;
                    }

                    for(v=0;v<m;v++)              /* col */
                        if ((v!=i)&&(v!=j))
                            a(v,b(i))=bi[v];

                    a(i,b(i))=bpp;
                    a(j,b(j))=bqq;
                    a(i,b(j))=bpq;                /* 0 */
                    a(j,b(i))=bqp;                /* 0 */

                    temp1[0]=sin1;
                    temp1[1]=cos1;
                    temp1[2]=(float)b(i);
                    temp1[3]=(float)b(j);
                }

                else
                {
                    temp1[0]=0.0;
                    temp1[1]=0.0;
                    temp1[2]=0.0;
                    temp1[3]=0.0;
                }

                MPI_Allgather(temp1,4,MPI_FLOAT,temp2,4,MPI_FLOAT,MPI_COMM_WORLD);
                current=0;

                for(v=1;v<=p;v++)
                {
                    s1=temp2[(v-1)*4+0];
                    c1=temp2[(v-1)*4+1];
                    i1=(int)temp2[(v-1)*4+2];
                    j1=(int)temp2[(v-1)*4+3];

                    if ((s1!=0.0)||(c1!=0.0)||(i1!=0)||(j1!=0))
                    {
                        if (myid!=current)
                        {
                            for(z=0;z<m;z++)
                            {
                                zi[z]=a(z,i1)*c1 + a(z,j1)*s1;
                                a(z,j1)=-a(z,i1)*s1 + a(z,j1)*c1;
                            }
                            for(z=0;z<m;z++)
                                a(z,i1)=zi[z];

                            for(z=0;z<m;z++)
                                zi[z]=e(z,i1)*c1+e(z,j1)*s1;

                            for(z=0;z<m;z++)
                                e(z,j1)=-e(z,i1)*s1+e(z,j1)*c1;

                            for(z=0;z<m;z++)
                                e(z,i1)=zi[z];

                        }                         /* if myid!=current */
                    }                             /* if */
                    current=current+1;
                }                                 /* for v */
            }                                     /* for i,j */

        }                                         /*  counter */

        if (myid==0)
        {
            MPI_Send(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);

            MPI_Send(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
            MPI_Send(&b(m/2),m/2,MPI_INT,myid+1,myid+1,MPI_COMM_WORLD);

            MPI_Recv(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
            MPI_Recv(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
            MPI_Recv(&b(m/2),m/2,MPI_INT,myid+1,myid,MPI_COMM_WORLD,&status);
        }

        if ((myid==p-1)&&(myid%2!=0))
        {
            for(i=m/2;i<m;i++)
                for(j=0;j<N;j++)
                    buffer((i-m/2),j)=a(i,j);

            for(i=m/2;i<m;i++)
                for(j=0;j<N;j++)
                    buffee((i-m/2),j)=e(i,j);

            for(i=m/2;i<m;i++)
                buf[i-m/2]=b(i);

            for(i=0;i<m/2;i++)
                for(j=0;j<N;j++)
                    a((i+m/2),j)=a(i,j);

            for(i=0;i<m/2;i++)
                for(j=0;j<N;j++)
                    e((i+m/2),j)=e(i,j);

            for(i=0;i<m/2;i++)
                b(m/2+i)=b(i);

            MPI_Recv(&(a(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
            MPI_Recv(&(e(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
            MPI_Recv(&b(0),m/2,MPI_INT,myid-1,myid,MPI_COMM_WORLD,&status);
            MPI_Send(buffer,m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
            MPI_Send(buffee,m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
            MPI_Send(buf,m/2,MPI_INT,myid-1,myid-1,MPI_COMM_WORLD);
        }

        if ((myid==p-1)&&(myid%2==0))
        {
            MPI_Send(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
            MPI_Send(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
            MPI_Send(&b(m/2),m/2,MPI_INT,myid-1,myid-1,MPI_COMM_WORLD);

            for(i=0;i<m/2;i++)
                for(j=0;j<N;j++)
                    a((i+m/2),j)=a(i,j);

            for(i=0;i<m/2;i++)
                for(j=0;j<N;j++)
                    e((i+m/2),j)=e(i,j);

            for(i=0;i<m/2;i++)
                b(i+m/2)=b(i);

            MPI_Recv(&(a(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
            MPI_Recv(&(e(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
            MPI_Recv(&b(0),m/2,MPI_INT,myid-1,myid,MPI_COMM_WORLD,&status);
        }

        if ((myid!=0)&&(myid!=p-1))
        {
            if(myid%2==0)
            {
                MPI_Send(&(a(0,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                MPI_Send(&(e(0,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                MPI_Send(&b(0),m/2,MPI_INT,myid+1,myid+1,MPI_COMM_WORLD);
                MPI_Send(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                MPI_Send(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                MPI_Send(&b(m/2),m/2,MPI_INT,myid-1,myid-1,MPI_COMM_WORLD);

                MPI_Recv(&(a(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(e(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&b(0),m/2,MPI_INT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&b(m/2),m/2,MPI_INT,myid+1,myid,MPI_COMM_WORLD,&status);
            }

            if(myid%2!=0)
            {
                for(i=0;i<m;i++)
                    for(j=0;j<N;j++)
                        buffer(i,j)=a(i,j);

                for(i=0;i<m;i++)
                    for(j=0;j<N;j++)
                        buffee(i,j)=e(i,j);

                for(i=0;i<m;i++)
                    buf[i]=b(i);

                MPI_Recv(&(a(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(e(0,0)),m/2*N,MPI_FLOAT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&b(0),m/2,MPI_INT,myid-1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(a(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&(e(m/2,0)),m/2*N,MPI_FLOAT,myid+1,myid,MPI_COMM_WORLD,&status);
                MPI_Recv(&b(m/2),m/2,MPI_INT,myid+1,myid,MPI_COMM_WORLD,&status);

                MPI_Send(&(buffer(0,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                MPI_Send(&(buffee(0,0)),m/2*N,MPI_FLOAT,myid+1,myid+1,MPI_COMM_WORLD);
                MPI_Send(&buf[0],m/2,MPI_INT,myid+1,myid+1,MPI_COMM_WORLD);
                MPI_Send(&(buffer(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                MPI_Send(&(buffee(m/2,0)),m/2*N,MPI_FLOAT,myid-1,myid-1,MPI_COMM_WORLD);
                MPI_Send(&buf[m/2],m/2,MPI_INT,myid-1,myid-1,MPI_COMM_WORLD);
            }
        }

        lmax=min;

        for(i=0;i<m;i++)
            for(j=0;j<N;j++)
                if ((m*myid+i)!=j)
                {
                    if (fabs(a(i,j))>lmax)
                        lmax=fabs(a(i,j));
                }

        MPI_Allreduce(&lmax,&max,1,MPI_FLOAT,MPI_MAX,MPI_COMM_WORLD);

    }                                             /* while */

    if (myid==0)
    {
        time3=MPI_Wtime();
        A=(float*)malloc(floatsize*N*m*p);
        I=(float*)malloc(floatsize*N*m*p);

        for(i=0;i<m;i++)
            for(j=0;j<N;j++)
        {
            A(i,j)=a(i,j);
            I(i,j)=e(i,j);
        }
    }

    if (myid!=0)
    {
        MPI_Send(a,m*N,MPI_FLOAT,0,myid,MPI_COMM_WORLD);
        MPI_Send(e,m*N,MPI_FLOAT,0,myid,MPI_COMM_WORLD);
    }
    else
        for(i=1;i<p;i++)
    {
        MPI_Recv(a,m*N,MPI_FLOAT,i,i,MPI_COMM_WORLD,&status);
        MPI_Recv(e,m*N,MPI_FLOAT,i,i,MPI_COMM_WORLD,&status);

        for(j=0;j<m;j++)
            for(k=0;k<N;k++)
                A((i*m+j),k)=a(j,k);

        for(j=0;j<m;j++)
            for(k=0;k<N;k++)
                I((i*m+j),k)=e(j,k);
    }

    if (myid==0)
    {
        for(i=0;i<N;i++)
            printf("the %dst envalue:%f\n",i,A(i,i));

        endtime=MPI_Wtime();

        printf("\n");
        printf("Iteration num = %d\n",loop);
        printf("Whole running time    = %f seconds\n",endtime-starttime);
        printf("Distribute data time  = %f seconds\n",time2-time1);
        printf("Parallel compute time = %f seconds\n",time3-time2);

    }

    MPI_Finalize();
    free(a);
    free(b);
    free(c);
    free(br);
    free(bt);
    free(bi);
    free(bj);
    free(zi);
    free(zj);
    free(buffer);
    free(buf);
    free(buffee);
    free(A);
    free(I);
    free(temp1);
    free(temp2);
    return(0);
}
