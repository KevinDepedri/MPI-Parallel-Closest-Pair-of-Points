#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/util.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2147483647
#define VERBOSE 0
#define PRINT_PAIRS_OF_POINTS 1

/* COSE DA FARE
0. Importare file da argomento
1. Sistemare notazione funzioni e variabili
2. Verificare che ogni malloc abbia il proprio free (ALMOST DONE)
3. Definire come gestire print/VERBOSE
4. Valutare come migliorare la leggibilitá del codice
*/

int main(int argc, char *argv[])
{
    // Initialize variables to measure the time required
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    // Initialize MPI and get rank_process and comm_size
    MPI_Init(&argc, &argv);
    int rank_process, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_process);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    char path[] = "../point_generator/1hundred.txt";
    // char path[] = argv[1];

    // Get the total number of points and the number of dimensions
    int num_points, num_dimensions;
    if (rank_process == MASTER_PROCESS)
    {
        // Open input point file on master process
        FILE *point_file = fopen(path, "r"); 
        if (point_file == NULL)
        {
            perror("Error opening file on master process\n");
            return -1;
        }

        // Read the number of points and dimensions from the first line of the file
        fscanf(point_file, "%d %d", &num_points, &num_dimensions);

        // Give error if the points or processes are not enough
        if (num_points < 2)
        {
            perror("Error: the number of points must be greater than 1\n");
            return -1;
        }
        if (num_points < comm_size)
        {
            perror("Error: the number of points must be greater than the number of processes\n");
            return -1;
        }
        fclose(point_file);
    }

    // Get the points data for all the points and order them according to x coordinate
    Point *all_points = NULL;
    all_points = parallel_mergesort(all_points, path, rank_process, comm_size, VERBOSE);
    if (rank_process == MASTER_PROCESS)
    {
        if (all_points == NULL)
        {
            perror("Error in executing parallel_mergesort\n");
            return -1;
        }
    }

    // Points are divided equally on all processes exept master process which takes the remaing points
    int num_points_normal_processes, num_points_master_process;

    // Define a variable where the minimum distance will be stored and initialize it to MAX_INT
    double super_final_dmin = INT_MAX;

    // If the code is ran on two or less processes, then run the sequential version of the problem.
    // Indeed, in this implementation process 0 is supposed to be the MASTER_PROCESS, which just supervises the operations, manages the transfer 
    // of data and carries out computation over a reduced number of points (the reminder dividing the points on all the other processes). 
    // For this reason, when working with 2 processes MASTER_PROCESS will be idle. Therefore, all the computation will be done sequentially on process 1.
    if(comm_size <= 2)
    {
        if(rank_process == MASTER_PROCESS){
            printf("Launching sequential algorithm...\n");
            super_final_dmin = sequential_closestpair_recursive(all_points, num_points);
        }
    }
    // If the code is ran on more than two processes, then run the parallel version of the problem.
    else
    {
        // Send num_points to all the processes, it is used to compute the num_points variables
        MPI_Bcast(&num_points, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
        MPI_Bcast(&num_dimensions, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);

        // Points are divided equally on all processes exept master process which takes the remaing points
        num_points_normal_processes = num_points / (comm_size-1);
        num_points_master_process = num_points % (comm_size-1);

        super_final_dmin = parallel_closestpair(all_points, num_points, num_dimensions, rank_process, comm_size, 
                                                    num_points_normal_processes, num_points_master_process);
    }

    // Print the final minimum distance and free the memory
    if (rank_process == MASTER_PROCESS)
    {
        printf("SUPER FINAL GLOBAL DMIN: %f\n", super_final_dmin);

        for (int point = 0; point < num_points; point++)
            free(all_points[point].coordinates);
    }
    free(all_points);

    // Use a barrier to alling all the cores and then get the execution time
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
    {
        printf("Memory free\n");
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Time elapsed: %f\n", cpu_time_used);
    }

    // Finalize the MPI application
    MPI_Finalize();
    return 0;
}