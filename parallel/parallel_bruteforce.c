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

    if (rank == 0)
    {
        Point *points;
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
        // compute the number of points for each cpu
        int points_per_cpu = num_points / (size - 1);
        int points_left = num_points % (size - 1);
        // send the number of points to each cpu, with the index of the first point and all the points
        int first_point = 0;
        for (int i = 1; i < size - 1; i++)
        {
            MPI_Send(&points_per_cpu, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&first_point, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(points, num_points * sizeof(Point), MPI_BYTE, i, 0, MPI_COMM_WORLD);
            first_point += points_per_cpu;
        }
        int last_cpu_points = points_per_cpu + points_left;
        MPI_Send(&last_cpu_points, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
        MPI_Send(&first_point, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
        MPI_Send(points, num_points * sizeof(Point), MPI_BYTE, size - 1, 0, MPI_COMM_WORLD);

        // receive the minimum distance from each cpu
        double min_distance = distance(points[0], points[1]);
        for (int i = 1; i < size; i++)
        {
            double cpu_min_distance;
            MPI_Recv(&cpu_min_distance, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (cpu_min_distance < min_distance)
            {
                min_distance = cpu_min_distance;
            }
        }
        printf("The minimum distance is %f", min_distance);

        // free the memory
        for (int i = 0; i < num_points; i++)
        {
            free(points[i].coordinates);
        }
        free(points);
    }
    else
    {
        // receive the number of points
        int num_points, first_point;
        MPI_Recv(&num_points, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&first_point, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // allocate memory for the points
        Point *points = (Point *)malloc(num_points * sizeof(Point));
        // receive the points
        MPI_Recv(points, num_points * sizeof(Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // compute the minimum distance
        double min_distance = distance(points[first_point], points[0]);

        // Find the minimum distance between any two points
        for (int i = first_point; i < first_point + num_points; i++)
        {
            for (int j = 0; j < num_points; j++)
            {
                if (i != j)
                {
                    double distance_ij = distance(points[i], points[j]);
                    if (distance_ij < min_distance)
                    {
                        min_distance = distance_ij;
                    }
                }
            }
        }
        // send the minimum distance to the master
        MPI_Send(&min_distance, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        // free the memory
        for (int i = 0; i < num_points; i++)
        {
            free(points[i].coordinates);
        }
        free(points);
    }
    MPI_Finalize();
    return 0;
}