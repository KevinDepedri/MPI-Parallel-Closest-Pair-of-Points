// parallel mergesort try
/*
read all the points
divide by the number of cpus
each cpu sorts its part
each cpu sends the sorted part to the master
the master merges the parts
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>  
#include "utils/util.h"

int main(int argc, char *argv[])
{
    int rank_process, comm_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_process);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    bool verbose = false;
    int num_points, num_dimensions;

    FILE *fp = fopen("../point_generator/points_channel_with_image/points_channel.txt", "r"); // ..\point_generator\points_channel_with_image\points_channel.txt  ../point_generator/10thousands.txt
    if (fp == NULL)
    {
        printf("Error opening file on core %d", rank_process);
        return 1;
    }

    // Read the number of points and dimensions from the first line of the file
    fscanf(fp, "%d %d", &num_points, &num_dimensions);

    // Give error if the point are not enough
    if (num_points < 2)
    {
        printf("Error: the number of points must be greater than 1\n");
        return 1;
    }
    if (num_points < comm_size)
    {
        printf("Error: the number of points must be greater than the number of cores\n");
        return 1;
    }

    int num_points_per_process, points_in_excess, index_first_point, index_last_point = 0;

    // Compute the number of points that will be read by each core
    num_points_per_process = num_points / comm_size;
    index_first_point = num_points_per_process*(rank_process);
    if (rank_process != comm_size-1){
        index_last_point = index_first_point + num_points_per_process;
        printf("Core %d/%d will read %d points from line %d to line %d\n", rank_process, comm_size, num_points_per_process, index_first_point, index_last_point);
    }else{
        points_in_excess = num_points % comm_size;
        num_points_per_process = num_points_per_process + points_in_excess;
        index_last_point = index_first_point + num_points_per_process;
        printf("Core %d/%d will read %d points from line %d to line %d\n", rank_process, comm_size, num_points_per_process, index_first_point, index_last_point);
    }

    // Compute the size of the header (number of points and number of dimensions)
    // long header_size = sizeof(num_points) + sizeof(num_dimensions);
    // printf("HEADER SIZE: %ld\n", header_size);

    // Jump to the point of the file which need to be read by this core
    long jump_size = 4 + 4*2*index_first_point; //The initial 4bytes is the size of the header (number of points and number of dimensions)
    if (fseek(fp, jump_size, SEEK_SET)!=0){
        printf("Error: could not use fseek to perform jump inside the input point file\n");
    }

    // Allocate memory for the points in each core
    Point *local_process_points;
    local_process_points = (Point *)malloc(num_points_per_process * sizeof(Point));

    // Read the points and store them in this core
    for (int point = 0; point < num_points_per_process; point++)
    {
        local_process_points[point].num_dimensions = num_dimensions;
        local_process_points[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        for (int dimension = 0; dimension < num_dimensions; dimension++)
        {
            fscanf(fp, "%d", &local_process_points[point].coordinates[dimension]);
        }
    }
    fclose(fp);

    // Print the points stored by each core (if verbose is True)
    if (verbose == true){
        for (int point=0; point< num_points_per_process; point++){
            printf("CORE-%d: Point %d: (", rank_process, point);
            for (int dimension=0; dimension < local_process_points[point].num_dimensions; dimension++){
                if (dimension != local_process_points[point].num_dimensions - 1){
                    printf("%d, ", local_process_points[point].coordinates[dimension]);
                }else{
                    printf("%d)\n", local_process_points[point].coordinates[dimension]);
                }
            }
        }
    }

    // Sort the points in each core
    mergeSort(local_process_points, num_points_per_process, 0);
    
    // Gather all the points in the main core
    if (rank_process == 0)
    {   
        // Core 0 receives the ordered points from all the other cores
        Point *all_sorted_points;
        all_sorted_points = (Point *)malloc(num_points * sizeof(Point));

        // transfer num points
        int *processes_num_points;
        processes_num_points = (int *)malloc(comm_size * sizeof(int));

        Point *processes_sorted_points;
        processes_sorted_points = (Point *)malloc(comm_size * sizeof(Point));
        for (int process = 1; process < comm_size; process++){
            processes_sorted_points[process] = (Point *)malloc(num_points * sizeof(Point));
        }

        for (int process = 1; process < comm_size; process++){
            MPI_Recv(points, num_points_per_process * sizeof(Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else
    {   
        // transfer num points for process

        // All the other cores send their ordered points to core 0
        MPI_Send(&num_points_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(points + first_point, num_points_per_process, MPI_BYTE, i, 0, MPI_COMM_WORLD);
        MPI_Send(points, num_points_per_process * sizeof(Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}