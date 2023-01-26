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
 int x=1;
 if (world_rank==0){
    printf("The number of processes is %d\n", world_size);

    // Send the first message to process 1 and start the ring
    printf("Rank 0 sends %d to rank %d \n",x,1);
    MPI_Send(&x,1,MPI_INT,1,0,MPI_COMM_WORLD);

    // Receive the last message from process 'world_size-1' and close the ring
    MPI_Recv(&x,1,MPI_INT,world_size-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("Rank 0 recieves %d to rank %d \n",x,world_size-1);
 }else{
    // Receive a message from the previous process 'world_rank-1'
    MPI_Recv(&x,1,MPI_INT,world_rank-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("Rank %d recieves %d from the rank %d \n",world_rank, x, world_rank-1);

    // Increment x. WHY THE PRINT ORDER IS NOT CORRECT?
    x++;
    if(world_rank + 1 != world_size){
        // Send a message to the next process to gradually pass through all the ring
        printf("Rank %d sends %d to rank %d \n",world_rank,x,world_rank+1);
        MPI_Send(&x,1,MPI_INT,world_rank+1,0,MPI_COMM_WORLD);
    }else{
        // Send a message to the first process to close the ring
        printf("Rank %d sends %d to rank %d \n",world_rank,x,0);
        MPI_Send(&x,1,MPI_INT,0,0,MPI_COMM_WORLD);
    }
 }

 // Finalize the MPI environment. No more MPI calls can be made after this
 MPI_Finalize();
 return 0;
}