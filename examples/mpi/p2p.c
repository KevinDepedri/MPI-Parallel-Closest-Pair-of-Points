#include<mpi.h>
#include<stdlib.h>
#include<stdio.h>

#define REPETITIONS 10
// To compile in console
// mpicc -std=c99 -g -Wall -o mpi_p2p p2p.c -lm

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
 
 // Define size variables
 int initial_size = 1;
 int final_size = 1048576;

 // Define time variables
 double initial_time, final_time, avg_transfer_time_rt, avg_transfer_time_1w, bandwidth = 0; // Variable 'not used' means: 'not printed'
 double transfer_time_rt_array[REPETITIONS];

 // For each package size execute this code
 for (int i=initial_size; i<=final_size; i*=2){ //Need to write i=.. if not i++
    if ((i==1)&&(world_rank==0)){
        printf("Size(bytes) \t\t TimeRoundTurn(ms) \t\t TimeOneWay(ms) \t\t Bandwidth(MB/s) \n");
        }
    
    char *packet;
    packet = (char *)malloc(i*sizeof(char));
    avg_transfer_time_rt = avg_transfer_time_1w = 0;

    if (packet == NULL){
        printf("Error. Could not allocate dynamic memory of size: %d\n", i);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    for (int rep=0; rep<REPETITIONS; rep++){
        if (world_rank==0){

            initial_time = MPI_Wtime();
            MPI_Sendrecv(packet,i,MPI_CHAR,1,0,
                         packet,i,MPI_CHAR,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            final_time = MPI_Wtime();

            transfer_time_rt_array[rep] = (final_time - initial_time)*1000;
            if(rep==REPETITIONS-1){
                for (int arr_pos=0; arr_pos<REPETITIONS; arr_pos++){
                    avg_transfer_time_rt = avg_transfer_time_rt + transfer_time_rt_array[arr_pos];
                }
                avg_transfer_time_rt = avg_transfer_time_rt/REPETITIONS;
                avg_transfer_time_1w = avg_transfer_time_rt/2;
                bandwidth =  (double)i/1048576/(avg_transfer_time_1w/1000); // MB/sec
                printf("%d\t\t\t%f\t\t\t%f\t\t\t%f\n",i, avg_transfer_time_rt, avg_transfer_time_1w, bandwidth);
            }
        }

        else if (world_rank==1){
            MPI_Sendrecv(packet,i,MPI_CHAR,0,0,
                         packet,i,MPI_CHAR,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
    }

    free(packet);
}

 // Finalize the MPI environment. No more MPI calls can be made after this
 MPI_Finalize();
 return 0;
}