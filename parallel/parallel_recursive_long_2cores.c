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
    if (comm_size == 1)
    {
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

    // Get the total number of points and the number of dimensions
    int num_points = 0, num_dimensions = 0;

    if (rank_process == MASTER_PROCESS)
    {
        // Open input point file on master process
        FILE *point_file = fopen(argv[1], "r");
        if (point_file == NULL)
        {
            perror("ERROR opening file on master process\n");
            return -1;
        }

        // Read the number of points and dimensions from the first line of the file
        fscanf(point_file, "%d %d", &num_points, &num_dimensions);
        fclose(point_file);
    }

    // Send num_points to all the processes, it is used to compute the num_points variables
    MPI_Bcast(&num_points, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    MPI_Bcast(&num_dimensions, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    // Get the points data for all the points and order them according to x coordinate
    Point *all_points = NULL;

    // Points are divided equally on all processes exept master process which takes the remaing points
    // int num_points_normal_processes = 0, num_points_master_process = 0, num_points_local_process = 0;
    int num_points_normal_processes = 0, num_points_master_process = 0, num_points_local_process = 0;

    Point *local_points = NULL;

    if (rank_process == MASTER_PROCESS)
    {
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

        // Points are divided equally on all processes exept master process which takes the remaing points
        num_points_normal_processes = num_points / (comm_size - 1);
        num_points_master_process = num_points % (comm_size - 1);

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
        Point *points_to_send = NULL;
        points_to_send = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        for (int point = 0; point < num_points_normal_processes; point++)
            points_to_send[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));

        for (int process = 1; process < comm_size; process++)
        {
            for (int point = 0; point < num_points_normal_processes; point++)
            {
                points_to_send[point].num_dimensions = num_dimensions;

                for (int dimension = 0; dimension < num_dimensions; dimension++)
                    points_to_send[point].coordinates[dimension] = all_points[point + num_points_normal_processes * (process - 1)].coordinates[dimension];
            }
            MPI_Send(&num_points_normal_processes, 1, MPI_INT, process, 0, MPI_COMM_WORLD);
            // sendPointsPacked(points_to_send, num_points_normal_processes, process, 1, MPI_COMM_WORLD);
            int first_half = num_points_normal_processes / 4;
            sendPointsPacked(points_to_send, first_half, process, 1, MPI_COMM_WORLD);
            // send second quarter of the points
            int second_half = first_half * 2;
            sendPointsPacked(points_to_send + first_half, first_half, process, 1, MPI_COMM_WORLD);
            // send third quarter of the points
            sendPointsPacked(points_to_send + second_half, first_half, process, 1, MPI_COMM_WORLD);
            // send fourth quarter of the points
            sendPointsPacked(points_to_send + second_half + first_half, num_points - second_half - first_half, process, 1, MPI_COMM_WORLD);
        }

        // Free points to send once the send for all the processes is done
        for (int point = 0; point < num_points_normal_processes; point++)
            free(points_to_send[point].coordinates);
        free(points_to_send);

        // Store the points of the master process
        num_points_local_process = num_points_master_process;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        int starting_from = num_points_normal_processes * (comm_size - 1);
        for (int point = 0; point + starting_from < num_points; point++)
        {
            local_points[point].num_dimensions = num_dimensions;
            local_points[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                local_points[point].coordinates[dimension] = all_points[point + starting_from].coordinates[dimension];
        }
    }
    else
    {
        // Receive total number of points and the points that will be processed by this process
        MPI_Recv(&num_points_normal_processes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        num_points_local_process = num_points_normal_processes;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        // recvPointsPacked(local_points, num_points_local_process, 0, 1, MPI_COMM_WORLD);
        int first_half = num_points_local_process / 4;
        recvPointsPacked(local_points, first_half, 0, 1, MPI_COMM_WORLD);
        // receive the second quarter of the points
        int second_half = first_half * 2;
        recvPointsPacked(local_points + first_half, first_half, 0, 1, MPI_COMM_WORLD);
        // receive the third quarter of the points
        recvPointsPacked(local_points + second_half, first_half, 0, 1, MPI_COMM_WORLD);
        // receive the fourth quarter of the points
        recvPointsPacked(local_points + second_half + first_half, num_points - second_half - first_half, 0, 1, MPI_COMM_WORLD);
    }

    // Sort the received points in each core
    if (num_points_local_process > 0)
        mergeSort(local_points, num_points_local_process, AXIS);

    if (rank_process != MASTER_PROCESS)
    {
        // All the processes send their ordered points to the master process
        // fisrt quarter of the points
        int first_half = num_points_local_process / 4;
        sendPointsPacked(local_points, first_half, 0, 1, MPI_COMM_WORLD);
        // second quarter of the points
        int second_half = first_half * 2;
        sendPointsPacked(local_points + first_half, first_half, 0, 1, MPI_COMM_WORLD);
        // third quarter of the points
        sendPointsPacked(local_points + second_half, first_half, 0, 1, MPI_COMM_WORLD);
        // fourth quarter of the points
        sendPointsPacked(local_points + second_half + first_half, num_points - second_half - first_half, 0, 1, MPI_COMM_WORLD);
    }
    else
    {
        // The master process gather all the points from the other processes
        Point **processes_sorted_points;
        processes_sorted_points = (Point **)malloc(comm_size * sizeof(Point *));

        processes_sorted_points[0] = (Point *)malloc((num_points_local_process) * sizeof(Point));
        for (int process = 1; process < comm_size; process++)
            processes_sorted_points[process] = (Point *)malloc(num_points_normal_processes * sizeof(Point));

        // Save first the points ordered from the current process (master), then receive the ones from the other processes
        processes_sorted_points[0] = local_points;
        for (int process = 1; process < comm_size; process++)
        {
            // first quarter
            int first_half = num_points_normal_processes / 4;
            recvPointsPacked(processes_sorted_points[process], first_half, process, 1, MPI_COMM_WORLD);
            // second quarter
            int second_half = first_half * 2;
            recvPointsPacked(processes_sorted_points[process] + first_half, first_half, process, 1, MPI_COMM_WORLD);
            // third quarter
            recvPointsPacked(processes_sorted_points[process] + second_half, first_half, process, 1, MPI_COMM_WORLD);
            // fourth quarter
            recvPointsPacked(processes_sorted_points[process] + second_half + first_half, num_points_normal_processes - second_half - first_half, process, 1, MPI_COMM_WORLD);
        }

        all_points = processes_sorted_points[1];
        for (int process = 1; process < comm_size; process++)
            free(processes_sorted_points[process]);
        free(processes_sorted_points);
    }

    // POINT 2 - compute min distance for each process
    double local_dmin = INT_MAX;
    Pairs *pairs = (Pairs *)malloc(sizeof(Pairs));

    pairs->points1 = (Point *)malloc((num_points_local_process) * sizeof(Point));
    pairs->points2 = (Point *)malloc((num_points_local_process) * sizeof(Point));
    for (int i = 0; i < (num_points_local_process); i++)
    {
        pairs->points1[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        pairs->points2[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
    }

    pairs->num_pairs = 0;
    pairs->min_distance = INT_MAX;
    if (num_points_local_process > 1)
    {
        recSplit(local_points, num_points_local_process, pairs, rank_process);
        local_dmin = pairs->min_distance;
    }
    printf("PROCESS:%d DMIN:%f\n", rank_process, local_dmin);
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
    {
        printf("Memory free\n");
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Time elapsed: %f\n", cpu_time_used);
    }

    // Finalize the MPI application
    MPI_Finalize();
    return 0;
}