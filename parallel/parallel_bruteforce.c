// mpi version of sequential_bruteforce.c
#include "utils/util.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/*
IDEA:
- divide the points in # of cpus
- each cpu calculates the distance between the points in its part
- each cpu sends the minimum distance to the master
- the master calculates the minimum of the minimums
*/
int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int starting_index = 0;
    int num_points = 0;
    int local_num = 0;
    double min_distance;
    Point *points;
    if (rank == 0)
    {
        int num_points, num_dimensions;
        FILE *fp = fopen("../point_generator/10thousands.txt", "r");
        if (fp == NULL)
        {
            perror("Error opening file");
            return 1;
        }
        // Read the number of points and dimensions from the first line of the file
        fscanf(fp, "%d %d", &num_points, &num_dimensions);
        if (num_points < 2)
        {
            printf("Error: the number of points must be greater than 1");
            return 1;
        }
        // Allocate memory for the points
        points = (Point *)malloc(num_points * sizeof(Point));
        for (int i = 0; i < num_points; i++)
        {
            points[i].num_dimensions = num_dimensions;
            points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int j = 0; j < num_dimensions; j++)
            {
                fscanf(fp, "%d", &points[i].coordinates[j]);
            }
        }
        fclose(fp);
    }
    MPI_Bcast(&num_points, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0){
        for(int i = 1; i < size; i++){
            sendPointsPacked(points, num_points, i, 1, MPI_COMM_WORLD);
        }
        local_num = num_points % size(-1);
        starting_index = num_points - local_num;
    }
    else{
        local_num = num_points / size(-1);
        points = (Point *)malloc(num_points * sizeof(Point));
        receivePointsPacked(points, num_points, 0, 1, MPI_COMM_WORLD);
        starting_index = (rank - 1) * local_num;
    }

    // brute force algorithm
    for (int i = starting_index; i < starting_index + local_num; i++)
    {
        for (int j = i + 1; j < num_points; j++)
        {
            double distance = euclidean_distance(points[i], points[j]);
            if (distance < min_distance)
            {
                min_distance = distance;
            }
        }
    }








    //     // compute the number of points for each cpu
    //     int points_per_cpu = num_points / (size - 1);
    //     int points_left = num_points % (size - 1);
    //     local_n = points_per_cpu;
    //     // send the number of points to each cpu
        
    //     for (int process = 1; process < size; process++)
    //     {
    //         // send all points to the process
    //         MPI_Send(&num_points, 1, MPI_INT, process, 0, MPI_COMM_WORLD);
    //         sendPointsPacked(points, num_points, process, 1, MPI_COMM_WORLD);
    //         //send the starting index of the points
    //         MPI_Send(&starting_index, 1, MPI_INT, process, 2, MPI_COMM_WORLD);
    //         // send the number of points to the process
    //         MPI_Send(&points_per_cpu, 1, MPI_INT, process, 3, MPI_COMM_WORLD);
    //         starting_index += points_per_cpu;
    //     }       

    // }
    // else{
    //     MPI_Recv(&local_n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     Point *local_points = (Point *)malloc(local_n * sizeof(Point));
    //     receivePointsPacked(local_points, local_n, 0, 1, MPI_COMM_WORLD);
    //     MPI_Recv(&starting_index, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // }

    // compute the minimum distance between 

    MPI_Finalize();
    return 0;
}