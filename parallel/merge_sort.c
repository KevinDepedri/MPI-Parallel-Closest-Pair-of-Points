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
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int num_points, num_dimensions;
    if (rank == 0)
    {
        Point *points;

        FILE *fp = fopen("../point_generator/1milion.txt", "r");
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
        int points_per_cpu = num_points / size;
        int points_left = num_points % size;
        // send the number of points to each cpu, with the index of the first point and all the points
        int first_point = 0;
        for (int i = 1; i < size - 1; i++)
        {
            MPI_Send(&points_per_cpu, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(points + first_point, points_per_cpu * sizeof(Point), MPI_BYTE, i, 0, MPI_COMM_WORLD);
            first_point += points_per_cpu;
        }
        MPI_Send(&points_left, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD);
        MPI_Send(points_left + first_point, points_per_cpu * sizeof(Point), MPI_BYTE, size - 1, 0, MPI_COMM_WORLD);

        // receive the sorted parts from the cpus
        // i think i should merge each part with the temporary total one, but i don't know how mpi receives the arrays
    }

    else
    {
        int points_per_cpu;
        MPI_Recv(&points_per_cpu, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // receive the points
        Point *points = (Point *)malloc(points_per_cpu * sizeof(Point));
        MPI_Recv(points, points_per_cpu * sizeof(Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // sort the points
        mergeSort(points, points_per_cpu, 0);
        // send the sorted points to the master
        MPI_Send(points, points_per_cpu * sizeof(Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}