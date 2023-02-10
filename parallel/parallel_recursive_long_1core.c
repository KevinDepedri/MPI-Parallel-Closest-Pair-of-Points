#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/util.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2147483647

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

    // Parse the input arguments
    if (argc < 2)
    {
        if (rank_process == MASTER_PROCESS)
            perror("ERROR: no file name or path has been provided as first argument (points input file)\n");
        return -1;
    }

    // Get the total number of points and the number of dimensions
    int num_points = 0, num_dimensions = 0;

    // Send num_points to all the processes, it is used to compute the num_points variables
    MPI_Bcast(&num_points, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    MPI_Bcast(&num_dimensions, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    // Get the points data for all the points and order them according to x coordinate
    Point *all_points = NULL;

    // Points are divided equally on all processes exept master process which takes the remaing points
    // int num_points_normal_processes = 0, num_points_master_process = 0, num_points_local_process = 0;

    // Open input point file on master process
    // FILE *point_file = fopen(path, "r");
    FILE *point_file = fopen(argv[1], "r");
    if (point_file == NULL)
    {
        perror("ERROR opening file on master process\n");
        return -1;
    }

    // Read the number of points and dimensions from the first line of the file
    fscanf(point_file, "%d %d", &num_points, &num_dimensions);

    // Read the points and store them in the master process
    all_points = (Point *)malloc(num_points * sizeof(Point));
    for (int point = 0; point < num_points; point++)
    {
        all_points[point].num_dimensions = num_dimensions;
        all_points[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        for (int dimension = 0; dimension < num_dimensions; dimension++)
            fscanf(point_file, "%d", &all_points[point].coordinates[dimension]);
    }
    fclose(point_file);

    // Transfer total number of points and the correct slice of points to work on for every process

    // Sort the received points in each core
    mergeSort(all_points, num_points, AXIS);

    // POINT 2 - compute min distance for each process
    double local_dmin = INT_MAX;
    Pairs *pairs = (Pairs *)malloc(sizeof(Pairs));

    pairs->points1 = (Point *)malloc((num_points) * sizeof(Point));
    pairs->points2 = (Point *)malloc((num_points) * sizeof(Point));
    for (int i = 0; i < (num_points); i++)
    {
        pairs->points1[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        pairs->points2[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
    }

    pairs->num_pairs = 0;
    pairs->min_distance = INT_MAX;
    recSplit(all_points, num_points, pairs, rank_process);
    local_dmin = pairs->min_distance;
    printf("PROCESS:%d DMIN:%f\n", rank_process, local_dmin);
    printf("Memory free\n");
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time elapsed: %f\n", cpu_time_used);

    // Finalize the MPI application
    MPI_Finalize();
    return 0;
}