#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/util.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2147483647
#define MERGESORT_VERBOSE 0
#define ENUMERATE_PAIRS_OF_POINTS 1
#define PRINT_PAIRS_OF_POINTS 1


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
    char path[] = "../point_generator/1H2d.txt"; //"/home/kevin.depedri/points/250M5d.txt";
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
        if (comm_size < 2){
            perror("Error: cannot run parallel application over just 1 process\n");
            return -1;
        }
        fclose(point_file);
    }

    // Get the points data for all the points and order them according to x coordinate
    Point *all_points = NULL;
    all_points = parallelMergeSort(all_points, path, rank_process, comm_size, MERGESORT_VERBOSE);
    if (rank_process == MASTER_PROCESS)
    {
        if (all_points == NULL)
        {
            perror("Error in executing parallel_mergesort\n");
            return -1;
        }
    }
    // Send num_points to all the processes, it is used to compute the num_points variables
    MPI_Bcast(&num_points, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    MPI_Bcast(&num_dimensions, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);

    // Points are divided equally on all processes exept master process which takes the remaing points
    int num_points_normal_processes, num_points_master_process;

    // If the code is ran on two processes, then it run the sequential version of the problem.
    // Indeed, in this parallel implementation process 0 is supposed to be the MASTER_PROCESS, which just supervises the operations, manages the transfer 
    // of data and carries out computation over a reduced number of points (the reminder dividing the points on all the other processes). 
    // For this reason, when working with 2 processes MASTER_PROCESS will be idle. Therefore, all the computation will be done sequentially on process 1.
    // For the same reason working on just one process is not feasible since the MASTER_PROCESS is not designed to carry out all the computations alone.
    if(comm_size == 2)
    {
        if(rank_process == MASTER_PROCESS){
        // Pairs will store the pairs of points with minimum distance and their distance data
        Pairs *pairs = NULL;
        pairs = (Pairs *)malloc(sizeof(Pairs));

        pairs->points1 = (Point *)malloc((num_points) * sizeof(Point));
        pairs->points2 = (Point *)malloc((num_points) * sizeof(Point));
        for (int point = 0; point < (num_points); point++){
            pairs->points1[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            pairs->points2[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        }
        pairs->num_pairs = 0;
        pairs->min_distance = INT_MAX;

            printf("Launching sequential algorithm...\n");
            recSplit(all_points, num_points, pairs);
            double global_dmin = pairs->min_distance;
            printf("---GLOBAL DMIN: %f\n", global_dmin);

            getUniquePairs(pairs, global_dmin, rank_process, comm_size, ENUMERATE_PAIRS_OF_POINTS, PRINT_PAIRS_OF_POINTS);
        
        free(pairs);
        }
    }
    // If the code is ran on more than two processes, then run the parallel version of the problem.
    else
    {
        // Points are divided equally on all processes exept master process which takes the remaing points
        num_points_normal_processes = num_points / (comm_size-1);
        num_points_master_process = num_points % (comm_size-1);

        if(rank_process == MASTER_PROCESS)
            printf("Launching parallel algorithm...\n");

        parallelClosestPair(all_points, num_points, num_dimensions, rank_process, comm_size,num_points_normal_processes, 
                              num_points_master_process, ENUMERATE_PAIRS_OF_POINTS, PRINT_PAIRS_OF_POINTS);
    }

    // Print the final minimum distance and free the memory
    if (rank_process == MASTER_PROCESS)
    {
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