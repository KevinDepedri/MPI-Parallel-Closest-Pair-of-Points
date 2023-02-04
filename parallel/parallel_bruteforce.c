// mpi version of sequential_bruteforce.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define INT_MAX 2147483647
#include "utils/util.h"

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    clock_t start, end;
    double cpu_time_used;
    start = clock();

    int num_points;
    Point *points;
    if (rank == 0)
    {
        int num_dimensions;
        FILE *fp = fopen("/home/kevin.depedri/points/250M5d.txt", "r");
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
                fscanf(fp, "%d", &points[i].coordinates[j]);
            
        }
        fclose(fp);
    }
    MPI_Bcast(&num_points, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int local_num, starting_index;
    if (rank == 0)
    {
        for (int i = 1; i < size; i++)
            sendPointsPacked(points, num_points, i, 1, MPI_COMM_WORLD);
        
        local_num = num_points % (size - 1);
        starting_index = num_points - local_num;
    }
    else
    {
        local_num = num_points / (size - 1);
        points = (Point *)malloc(num_points * sizeof(Point));
        recvPointsPacked(points, num_points, 0, 1, MPI_COMM_WORLD);
        starting_index = (rank - 1) * local_num;
    }
    double min_distance = INT_MAX;
    int *min_points1 = (int *)malloc(num_points * sizeof(int));
    int *min_points2 = (int *)malloc(num_points * sizeof(int));
    int points1_count = 0;
    int points2_count = 0;
    int count = 0;
    if (local_num > 0)
    {
        for (int i = starting_index; i < starting_index + local_num; i++)
        {
            for (int j = i + 1; j < num_points; j++)
            {
                double dd = distance(points[i], points[j]);
                if (dd < min_distance)
                {
                    min_distance = dd;
                    min_points1[0] = i;
                    min_points2[0] = j;
                    count = 1;

                    points1_count = 1;
                    points2_count = 1;
                }
                else if (dd == min_distance)
                {
                    min_points1[count] = i;
                    min_points2[count] = j;
                    count++;
                    points1_count++;
                    points2_count++;
                }
            }
        }
    }
    double global_min;
    MPI_Allreduce(&min_distance, &global_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    if (min_distance == global_min)
    {
        printf("[R %d] Points that got the minimum distance:\n", rank);
        for (int i = 0; i < points1_count; i++)
        {
            printPoint(points[min_points1[i]]);
            printf(" - ");
            printPoint(points[min_points2[i]]);
            printf("\n");
        }
        printf("[R %d] Number of pairs with minimum distance: %d\n", rank, points1_count);
    }
    if (rank == 0)
    {
        printf("The minimum distance is %f\n", global_min);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Time elapsed: %f\n", cpu_time_used);
    }

    // Free the memory
    for (int i = 0; i < num_points; i++)
        free(points[i].coordinates);
    
    free(points);
    free(min_points1);
    free(min_points2);

    MPI_Finalize();
    return 0;
}