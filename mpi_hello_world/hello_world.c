#include<mpi.h>
#include<stdio.h>

// MPI COMPILE (can be ran on every linux user)
// To compile in console
// mpicc -std=c99 -g -Wall -o mpi_hello_world hello_world.c -lm

// MPI EXECUTE (better to not run it on ROOT)
// To run it in linux (relying just on the available cores, in this case 6)
// mpiexec -n 6 mpi_hello_world

// To run it in linux (resorting to threads to increase the number of nodes, in this case 12)
// mpiexec -use-hwthread-cpus -n 12 mpi_hello_world

// To run it in linux (resorting to oversubscribe to allocate more nodes than the available core/threads, in this case more than 12)
// mpiexec -oversubscribe -n 20 mpi_hello_world

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

 // Print off a hello world message
 printf("Hello world from process rank %d out of %d processors\n", world_rank, world_size);

 // Finalize the MPI environment. No more MPI calls can be made after this
 MPI_Finalize();
 return 0;
}