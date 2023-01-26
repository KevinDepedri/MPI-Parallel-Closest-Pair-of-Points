#include<mpi.h>
#include<stdio.h>

int main(int argc,char**argv){
 // Initialize the MPI environment. The two arguments to MPI Init are not
 // currently used by MPI implementations, but are there in case future
 // implementations might need the arguments.
 MPI_Init(NULL,NULL);

 // Get the number of processes
 int world_size;
 MPI_Comm_size(MPI_COMM_WORLD,&world_size);

 // Get the rank of processes
 int world_rank;
 MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

 // Execution of the program
 int x=15;
 if (world_rank==0){
    printf("Rank 0 sends: %d \n",x);
    MPI_Send(&x,1,MPI_INT,1,0,MPI_COMM_WORLD);
    MPI_Recv(&x,1,MPI_INT,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("Rank 0 recieves: %d \n",x);
 }
 else if (world_rank==1){
    MPI_Recv(&x,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("Rank 1 recieves: %d \n",x);
    x = 10;
    printf("Rank 1 sends: %d \n",x);
    MPI_Send(&x,1,MPI_INT,0,0,MPI_COMM_WORLD);
 }
 
 // Finalize the MPI environment. No more MPI calls can be made after this
 MPI_Finalize();
 return 0;
}