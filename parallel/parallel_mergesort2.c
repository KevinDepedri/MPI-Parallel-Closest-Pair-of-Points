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
#include "utils/util.h"

int main(int argc, char *argv[])
{
    int core_rank, comm_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &core_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    unsigned int num_points, num_dimensions;
    int points_per_cpu, remainder, first_point_index, last_point_index = 0;

    // temporarly working just with 20 points
    FILE *fp = fopen("../point_generator/points_channel_with_image/points_channel.txt", "r"); // ..\point_generator\points_channel_with_image\points_channel.txt  ../point_generator/10thousands.txt
    if (fp == NULL)
    {
        printf("Error opening file on core %d", core_rank);
        return 1;
    }

    // Read the number of points and dimensions from the first line of the file
    fscanf(fp, "%d %d", &num_points, &num_dimensions);

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

    // Compute the number of points that will be read by this core
    points_per_cpu = num_points / comm_size;
    first_point_index = points_per_cpu*(core_rank);
    if (core_rank != comm_size-1){
        last_point_index = first_point_index + points_per_cpu;
        printf("Core %d/%d will read %d points from line %d to line %d\n", core_rank, comm_size, points_per_cpu, first_point_index, last_point_index);

    }else{
        remainder = num_points % comm_size;
        points_per_cpu = points_per_cpu + remainder;
        last_point_index = first_point_index + points_per_cpu;
        printf("Core %d/%d will read %d points from line %d to line %d\n", core_rank, comm_size, points_per_cpu, first_point_index, last_point_index);
    }

    // Allocate memory for the points
    Point *local_core_points;
    local_core_points = (Point *)malloc(points_per_cpu * sizeof(Point));

    long header_size = sizeof(num_points) + sizeof(num_dimensions);
    printf("HEADER SIZE: %ld\n", header_size);

    long jump_size = 4 + 4*2*first_point_index; //The initial 4 is the first line (number of points and number of dimensions)
    if (fseek(fp, jump_size, SEEK_SET)!=0){
        printf("Error: could not perform jump inside point file using fseek\n");
    }

    for (int i = 0; i < points_per_cpu; i++)
    {
        local_core_points[i].num_dimensions = num_dimensions;
        local_core_points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        for (int j = 0; j < num_dimensions; j++)
        {
            fscanf(fp, "%d", &local_core_points[i].coordinates[j]);
        }
    }
    fclose(fp);

    for (int i=0; i< points_per_cpu; i++){
        printf("CORE-%d: Point %d: (", core_rank, i);
        for (int j=0; j < local_core_points[i].num_dimensions; j++){
            if (j != local_core_points[i].num_dimensions - 1){
                printf("%d, ", local_core_points[i].coordinates[j]);
            }else{
                printf("%d)\n", local_core_points[i].coordinates[j]);
            }
        }
    }

    // if (core_rank == 0)
    // {
    //     Point *all_points;

    //     FILE *fp = fopen("../point_generator/10thousands.txt", "r");
    //     if (fp == NULL)
    //     {
    //         perror("Error opening file");
    //         return 1;
    //     }
    //     // Read the number of points and dimensions from the first line of the file
    //     fscanf(fp, "%d %d", &num_points, &num_dimensions);
    //     if (num_points < 2)
    //     {
    //         printf("Error: the number of points must be greater than 1");
    //         return 1;
    //     }
    //     // Allocate memory for the points
    //     points = (Point *)malloc(num_points * sizeof(Point));
    //     for (int i = 0; i < num_points; i++)
    //     {
    //         points[i].num_dimensions = num_dimensions;
    //         points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
    //         for (int j = 0; j < num_dimensions; j++)
    //         {
    //             fscanf(fp, "%d", &points[i].coordinates[j]);
    //         }
    //     }
    //     fclose(fp);

        // // compute the number of points for each cpu
        // int points_per_cpu = num_points / size;
        // int points_left = num_points % size;
        // // send the number of points to each cpu, with the index of the first point and all the points
        // int first_point = 0;
        // for (int i = 1; i < size - 1; i++)
        // {
        //     MPI_Send(&points_per_cpu, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        //     MPI_Send(points + first_point, points_per_cpu * sizeof(Point), MPI_BYTE, i, 0, MPI_COMM_WORLD);
        //     first_point += points_per_cpu;
        // }
        // MPI_Send(&points_left, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
        // MPI_Send(points_left + first_point, points_per_cpu * sizeof(Point), MPI_BYTE, size - 1, 0, MPI_COMM_WORLD);

        // // receive the sorted parts from the cpus
        // // i think i should merge each part with the temporary total one, but i don't know how mpi receives the arrays
    // }

    // else
    // {
    //     int points_per_cpu;
    //     MPI_Recv(&points_per_cpu, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     // receive the points
    //     Point *points = (Point *)malloc(points_per_cpu * sizeof(Point));
    //     MPI_Recv(points, points_per_cpu * sizeof(Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     // sort the points
    //     mergeSort(points, points_per_cpu, 0);
    //     // send the sorted points to the master
    //     MPI_Send(points, points_per_cpu * sizeof(Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    // }

    MPI_Finalize();
    return 0;
}