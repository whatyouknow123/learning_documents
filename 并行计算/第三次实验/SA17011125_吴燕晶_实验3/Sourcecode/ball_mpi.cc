#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<math.h>
#include<string.h>
#include<mpi.h>

//define the weight of each ball
#define m 10000

//define the value of G
#define g 6.67*pow(10, -11)

//defime the delta time 
#define delta_t 0.00001


//define the structure of ball
typedef struct myball{
  double vx; // the speed of x
  double vy; // the speed of y
  double px; // the position of x
  double py; // the position of y 
  double ax; // the accelation of x
  double ay; // the accelation of y
}ball;

// compute the force
// return a array which the length is 4 and it record the acc between to point
void compute_force(ball *b1, ball *b2, double acc[]){
  double diff_x, diff_y;
  diff_x = b1->px - b2->px;
  diff_y = b1->py - b2->py; 
  double acc_x, acc_y;
  double distance;
  distance = sqrt(diff_x*diff_x + diff_y*diff_y);
  double temp_force = g*m/pow(distance, 3);
  acc_x = diff_x * temp_force;
  acc_y = diff_y * temp_force;
  acc[0] = acc_x * delta_t;
  acc[1] = acc_y * delta_t;
  acc[2] = (-acc_x) * delta_t;
  acc[3] = (-acc_y) * delta_t;
}

// compute the accelerate
void compute_acce(ball *b1, double acc_x, double acc_y){
  b1->ax = acc_x;
  b1->ay = acc_y;
}
// compute the speed
void compute_velocities(ball *b1){
  b1->vx += b1->ax;
  b1->vy += b1->ay;
}

// compute the position
void compute_positions(ball *b1){
  double vx = b1->vx;
  double vy = b1->vy;
  b1->px += vx*delta_t;
  b1->py += vy*delta_t;
}

void compute_allforce(ball balls[], ball force[], int n){
  double acc[4];
  for(int i=0; i<n; i++)
    for(int j=i+1; j<n; j++){
      compute_force(&balls[i], &balls[j], acc);
      force[i].ax += acc[0];
      force[i].ay += acc[1];
      force[j].ax += acc[2];
      force[j].ay += acc[3];
    }
  
}

void balls(int n){
  double start, end, diff;
  int size, myrank;
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
 // printf("myrank is %d\n", myrank);
 // printf("the size is %d\n", size);
  int count;
  count = n/size;
  if(myrank == 0){
    start = MPI_Wtime();
  }

  int row = (int)sqrt(n);
  int col= row/size;
  ball balls[count+1];
  ball force[count+1];


  for(int i=0; i<row; i++)
    for(int j = 0; j<col; j++){
      balls[i*col+j].vx = 0;
      balls[i*col+j].vy = 0;
      balls[i*col+j].px = i;
      balls[i*col+j].py = j+myrank*count;
      balls[i*col+j].ax = 0;
      balls[i*col+j].ay = 0;
    }
  
  for(int i=0; i<count; i++){
    force[i].vx = 0;
    force[i].vy = 0;
    force[i].px = balls[i].px;
    force[i].py = balls[i].py;
    force[i].ax = 0;
    force[i].ay = 0;
  }

  // define the send data type 
  MPI_Datatype mpiball;
  MPI_Datatype oldtype[6] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
  int blockcount[6]= {1,1,1,1,1,1};
  MPI_Aint offset[5];
  MPI_Status stat;
  //initialize the address
  MPI_Address(&(force[0].vx), &offset[0]);
  MPI_Address(&(force[0].vy), &offset[1]);
  MPI_Address(&(force[0].px), &offset[2]);
  MPI_Address(&(force[0].py), &offset[3]);
  MPI_Address(&(force[0].ax), &offset[4]);
  MPI_Address(&(force[0].ay), &offset[5]);
  for(int i=5; i>=0; i--){
    offset[i] -= offset[0];
  }
  MPI_Type_struct(6, blockcount, offset, oldtype, &mpiball);
  MPI_Type_commit(&mpiball);
  
 // printf("finish the initail\n");
  int k = 1;
  while(k<10000){
    
    // calculate current block all force and send the force to next
    compute_allforce(balls, force, count);
   // printf("%d finish calculate the force\n", myrank);
    MPI_Send(force, count, mpiball, (myrank+1)%size, myrank+k*size, MPI_COMM_WORLD);
   // printf("%d finish the first send\n", myrank);
    for(int i=0; i<size; i++){
      if(i!=myrank){
        MPI_Recv(force, count, mpiball, (myrank-1+size)%size, i+k*size, MPI_COMM_WORLD, &stat);
     //   printf("%d finish the second recv\n", myrank);
	compute_allforce(balls, force, count);
      //  printf("%d finish calculate the force 2\n", myrank);
        MPI_Send(force, count, mpiball, (myrank+1)%size, i+k*size, MPI_COMM_WORLD);
       // printf("%d finish the second send\n", myrank);
      }
    }
   // printf("finish the circling");
    MPI_Recv(force, count, mpiball, (myrank-1+size)%size, myrank+k*size, MPI_COMM_WORLD, &stat);
   // printf("finish the first receive");
    for(int i=0; i<count; i++){
      compute_acce(&balls[i], force[i].ax, force[i].ay);
      compute_velocities(&balls[i]);
      compute_positions(&balls[i]);
    }
 //   printf("%d finish the %d time", myrank, k);
    for(int i=0; i<count; i++){
      force[i].vx = 0;
      force[i].vy = 0;
      force[i].px = balls[i].px;
      force[i].py = balls[i].py;
      force[i].ax = 0;
      force[i].ay = 0;
    }
    k++;
  }
  if(myrank==0){
    end = MPI_Wtime();
    diff = end - start;
    printf("the N is %d\n", n);
    printf("the exetuce time is %fs \n", diff);
    if(n==64){
      printf("the ratio is %f\n", 2.075067/diff);
    }else{
      printf("the ratio is %f\n", 34.314328/diff);
    }
  }
}

void serial_time(int n){
  double start, end, diff;
  start = MPI_Wtime();
  ball balls[n];
  int row = (int)sqrt(n);
  for(int i=0; i<row; i++){
    for(int j =0; j<row; j++){
      balls[i+j*row].vx = 0;
      balls[i+j*row].vy = 0;
      balls[i+j*row].px = i;
      balls[i+j*row].py = j;
      balls[i+j*row].ax = 0;
      balls[i+j*row].ay = 0;
    }
  }
  printf("finish the initial");
  int k = 1;
  while(k<10000){
    compute_allforce(balls, balls, n);
    for(int i=0; i<n; i++){
      compute_velocities(&balls[i]);
      compute_positions(&balls[i]);
    }
    k++;
  }
  end = MPI_Wtime();
  diff = end - start;
  printf("the serial time is %f\n", diff);
}

int main(int argc, char* argv[]){
  MPI_Init(&argc, &argv);
  //serial_time(64);
  //serial_time(256);
  balls(64);
  balls(256);
  MPI_Finalize();
  return 0;
}
