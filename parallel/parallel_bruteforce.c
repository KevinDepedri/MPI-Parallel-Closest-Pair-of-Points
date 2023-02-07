// mpi version of sequential_bruteforce.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/util.h"

#define MASTER_PROCESS 0
#define INT_MAX 2147483647


int main(int argc, char *argv[])
{
    // Initialize variables to measure the time required
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    int rank_process, comm_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_process);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    // Parse the input arguments
    if(argc < 2){
        if (rank_process == MASTER_PROCESS)
            perror("ERROR: no file name or path has been provided as first argument (points input file)\n");
        return -1;
    }

    int ENUMERATE_PAIRS_OF_POINTS = 0, PRINT_PAIRS_OF_POINTS = 0, INVALID_FLAG = 0;
    for (size_t option_id = 2; option_id < argc; option_id++) {
        switch (argv[option_id][1]) {
            case 'e': ENUMERATE_PAIRS_OF_POINTS = 1; break;
            case 'p': ENUMERATE_PAIRS_OF_POINTS = 1; PRINT_PAIRS_OF_POINTS = 1; break;
            default: INVALID_FLAG = 1; break;
        }   
    }
    if (INVALID_FLAG == 1){
        if (rank_process == MASTER_PROCESS)
            perror("ERROR: the only valid flag arguments are:\n \t-e : enumerate the pairs of point with smallest distance\n \t-p : print the pairs of point with smallest distance\n");
        return -1;
    }

    int num_points;
    Point *points;
    if (rank_process == 0)
    {
        int num_dimensions;
        // Open input point file on master process
        FILE *point_file = fopen(argv[1], "r"); 
        if (point_file == NULL){
            perror("ERROR opening file on master process\n");
            return -1;
        }
        // Read the number of points and dimensions from the first line of the file
        fscanf(point_file, "%d %d", &num_points, &num_dimensions);

        // Give error if the points or processes are not enough
        if (num_points < 2){
            perror("ERROR: the number of points must be greater than 1\n");
            return -1;
        }
        if (num_points < comm_size){
            perror("ERROR: the number of points must be greater than the number of processes\n");
            return -1;
        }
        if (comm_size < 2){
            perror("ERROR: cannot run parallel application over just 1 process\n");
            return -1;
        }

        // Allocate memory for the points
        points = (Point *)malloc(num_points * sizeof(Point));
        for (int i = 0; i < num_points; i++)
        {
            points[i].num_dimensions = num_dimensions;
            points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int j = 0; j < num_dimensions; j++)
                fscanf(point_file, "%d", &points[i].coordinates[j]);
            
        }
        fclose(point_file);
    }
    MPI_Bcast(&num_points, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int local_num, starting_index;

    if (rank_process == 0)
    {
        for (int i = 1; i < comm_size; i++){
            // send first quarter of the points
            int first_half = num_points / 4;
            sendPointsPacked(points, first_half, i, 1, MPI_COMM_WORLD);
            // send second quarter of the points
            int second_half = first_half * 2;
            sendPointsPacked(points + first_half, first_half, i, 1, MPI_COMM_WORLD);
            // send third quarter of the points
            sendPointsPacked(points + second_half, first_half, i, 1, MPI_COMM_WORLD);
            // send fourth quarter of the points
            sendPointsPacked(points + second_half + first_half, num_points - second_half - first_half, i, 1, MPI_COMM_WORLD);
            //sendPointsPacked(points, num_points, i, 1, MPI_COMM_WORLD);
        }
        local_num = num_points % (comm_size - 1);
        starting_index = num_points - local_num;
    }
    else
    {
        local_num = num_points / (comm_size - 1);
        points = (Point *)malloc(num_points * sizeof(Point));
        // receive the first quarter of the points
        int first_half = num_points / 4;
        recvPointsPacked(points, first_half, 0, 1, MPI_COMM_WORLD);
        // receive the second quarter of the points
        int second_half = first_half * 2;
        recvPointsPacked(points + first_half, first_half, 0, 1, MPI_COMM_WORLD);
        // receive the third quarter of the points
        recvPointsPacked(points + second_half, first_half, 0, 1, MPI_COMM_WORLD);
        // receive the fourth quarter of the points
        recvPointsPacked(points + second_half + first_half, num_points - second_half - first_half, 0, 1, MPI_COMM_WORLD);
        //recvPointsPacked(points, num_points, 0, 1, MPI_COMM_WORLD);
        starting_index = (rank_process - 1) * local_num;
    }
    double min_distance = INT_MAX;
    int *min_points1 = (int *)malloc(num_points * sizeof(int));
    int *min_points2 = (int *)malloc(num_points * sizeof(int));
    int points1_count = 0;
    int points2_count = 0;
    int count = 0;
    if (local_num > 0){
        for (int i = starting_index; i < starting_index + local_num; i++){
            for (int j = i + 1; j < num_points; j++){
                double dd = distance(points[i], points[j]);
                if (dd < min_distance){
                    min_distance = dd;
                    min_points1[0] = i;
                    min_points2[0] = j;
                    count = 1;

                    points1_count = 1;
                    points2_count = 1;
                }
                else if (dd == min_distance){
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
    if (ENUMERATE_PAIRS_OF_POINTS){
        if (min_distance == global_min){
            if (PRINT_PAIRS_OF_POINTS){
                printf("[R %d] Points that got the minimum distance:\n", rank_process);
                for (int i = 0; i < points1_count; i++){
                    printPoint(points[min_points1[i]]);
                    printf(" - ");
                    printPoint(points[min_points2[i]]);
                    printf("\n");
                }
            }
            printf("[R %d] Number of pairs with minimum distance: %d\n", rank_process, points1_count);
        }
    }
    if (rank_process == 0){
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