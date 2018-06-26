#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<mpi.h>
#define true 1
#define false 0
const int root_id = 0;
const int max_size = 16;

int main(int argc,char**argv)
{
    double start_time, end_time;
    int rank,size;
    MPI_Status status;
    MPI_Request reqSend, reqRecv;
    MPI_Init(&argc,&argv);
    start_time = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int i,j;
    int N=4;
    const int size_sqrt =(int)sqrt(size);
    const int n = N / size_sqrt;
    const int s = n*n;

    if (size<4 || size> max_size){
        printf("The fox algorithm requires at least 4 processors and at most %d processors.",max_size);
        MPI_Finalize();
        return 0;
    }
    if (size_sqrt*size_sqrt != size){
        printf("The number of process must be a square. ");
        MPI_Finalize();
        return 0;
    }
    if (N % size_sqrt !=0){
        printf("N % size_sqrt !=0  ");
        MPI_Finalize();
        return 0;
    }

    int * A = (int *)malloc(s*sizeof(int));
    int * B = (int *)malloc(s*sizeof(int));
    int * C = (int *)malloc(s*sizeof(int));
    int * T = (int *)malloc(s*sizeof(int));

    for (i=0; i<n; ++i)
        for (j=0; j<n; ++j){
            A[i*n+j] = (i+j)*rank;
            B[i*n+j] = (i-j)*rank;
            C[i*n+j] = 0;
        }

    printf("A on procs %d :  \n", rank);    
for (i=0; i<n; ++i){
        for (j=0; j<n; ++j){
            printf("%5d",A[i*n+j]);
        }
        printf("\n");
  }
    printf("B on procs# %d : \n ", rank);
    for (i=0; i<n; ++i){
        for (j=0; j<n; ++j){
            printf("%5d",B[i*n+j]);
        }
        printf("\n");
    }

    MPI_Comm cart_all, cart_row, cart_col;
    int dims[2], periods[2];
    int procs_cart_rank, procs_coords[2];
    dims[0] = dims[1] = size_sqrt;
    periods[0] = periods[1] = true;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, false, &cart_all);
    MPI_Comm_rank(cart_all, &procs_cart_rank);
    MPI_Cart_coords(cart_all, procs_cart_rank,  2, procs_coords);
    MPI_Comm_split(cart_all, procs_coords[0], procs_coords[1], &cart_row);
    MPI_Comm_split(cart_all, procs_coords[1], procs_coords[0], &cart_col);
    int rank_cart_row, rank_cart_col;
    MPI_Comm_rank(cart_row, & rank_cart_row);
    MPI_Comm_rank(cart_col, & rank_cart_col);
	int round;
    for (round = 0; round < size_sqrt; ++ round){

        MPI_Isend(B, s, MPI_INT, (procs_coords[0] - 1 + size_sqrt) % size_sqrt, 
                  1, cart_col, &reqSend);

        int broader = (round + procs_coords[0]) % size_sqrt;
        if (broader == procs_coords[1]) for(i=0;i<s;i++)T[i]=A[i];

        MPI_Bcast(T, s, MPI_INT, broader , cart_row);
	int row,col,k;
        for (row=0; row<n; ++row)
            for (col=0; col<n; ++col)
                for (k=0; k<n; ++k){
                    C[row*n+col] += T[row*n+k]*B[k*n+col];
                }
        
        MPI_Wait(&reqSend, &status);
        MPI_Recv(T, s,  MPI_INT, (procs_coords[0] + 1) % size_sqrt
                          , 1, cart_col, &status);
       for(i=0;i<s;i++)B[i]=T[i];

    }

    printf("C on procs %d :  ", rank);
    for (i=0; i<n; ++i){
        for (j=0; j<n; ++j){
            printf("%5d",C[i*n+j]);
        }
        printf(" ");
    }
    
    MPI_Comm_free(&cart_col);
    MPI_Comm_free(&cart_row);
    MPI_Comm_free(&cart_all);
    free(A);
    free(B);
    free(C);
    free(T);
    end_time = MPI_Wtime();
    MPI_Finalize();
    printf("proc #%d consumed %lf seconds ", rank, end_time-start_time);

    return 0;
}
