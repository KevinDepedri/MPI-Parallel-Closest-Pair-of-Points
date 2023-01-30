#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utils/util.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2000000000

int main(int argc, char *argv[])
{
    int rank_process, comm_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_process);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int num_points, num_dimensions;
    int num_points_normal_processes, num_points_master_process, num_points_local_process;
    int index_first_point, index_last_point = 0;

    Point *all_points;
    Point *local_points;

    if (rank_process == MASTER_PROCESS)
    {
        FILE *point_file = fopen("../point_generator/1hundred.txt", "r"); 
        if (point_file == NULL)
        {
            perror("Error opening file on master process\n");
            return 1;    
        }

        // Read the number of points and dimensions from the first line of the file
        fscanf(point_file, "%d %d", &num_points, &num_dimensions);

        // Give error if the point are not enough
        if (num_points < 2)
        {
            perror("Error: the number of points must be greater than 1\n");
            return 1;
        }
        if (num_points < comm_size)
        {
            perror("Error: the number of points must be greater than the number of processes\n");
            return 1;
        }
        if (comm_size < 2)
        {
            perror("Error: cannot run the parallel program over just 1 process\n");
            return 1;
        }
        

        // Compute how many points we need to transfer 
        // Points are divided equally on all processes exept master process which takes the remaing points
        num_points_normal_processes = num_points / (comm_size-1);
        num_points_master_process = num_points % (comm_size-1);

        // Read the points and store them in this core
        all_points = (Point *)malloc(num_points * sizeof(Point));
        for (int point = 0; point < num_points; point++)
        {
            all_points[point].num_dimensions = num_dimensions;
            all_points[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
            {
                fscanf(point_file, "%d", &all_points[point].coordinates[dimension]);
            }
        }
        fclose(point_file);

        printf("INPUT LIST OF POINTS:\n");
        print_points(all_points, num_points, rank_process); 
        
        // Transfer number of points, number of dimensions and points to every process
        Point *points_to_send;
        points_to_send = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        for (int process = 1; process < comm_size; process++)
        {
            // Copy the points to send to the process
            for (int point = 0; point < num_points_normal_processes; point++)
            {
                points_to_send[point].num_dimensions = num_dimensions;
                points_to_send[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
                for (int dimension = 0; dimension < num_dimensions; dimension++)
                {
                    points_to_send[point].coordinates[dimension] = all_points[point+num_points_normal_processes*(process-1)].coordinates[dimension];
                }
            }
            MPI_Send(&num_points_normal_processes, 1, MPI_INT, process, 0, MPI_COMM_WORLD);
            // MPI_Send(&num_dimensions, 1, MPI_INT, process, 1, MPI_COMM_WORLD);
            sendPointsPacked(points_to_send, num_points_normal_processes, process, 3, MPI_COMM_WORLD);
        }

        // Free points to send
        for (int point = 0; point < num_points_normal_processes; point++)
        {
            free(points_to_send[point].coordinates);
        }
        free(points_to_send);

        // Store the points of the master process
        num_points_local_process = num_points_master_process;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        int starting_from = num_points_normal_processes * (comm_size - 1);
        for (int i = 0; i + starting_from < num_points; i++)
        {
            local_points[i].num_dimensions = num_dimensions;
            local_points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
            {
                local_points[i].coordinates[dimension] = all_points[i + starting_from].coordinates[dimension];
            }
        }
    }
    else
    {
        // Receive info on numbers
        MPI_Recv(&num_points_normal_processes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // MPI_Recv(&num_dimensions, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("REC2: %d\n", num_dimensions);

        // All the other processes receives their points
        num_points_local_process = num_points_normal_processes;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        recvPointsPacked(local_points, num_points_local_process, 0, 3, MPI_COMM_WORLD);
    }

    // Compute the number of points that will be read by each core
    if (rank_process != MASTER_PROCESS)
    {
        index_first_point = num_points_local_process * (rank_process-1);
        index_last_point = index_first_point + num_points_local_process - 1;
        printf("[NORMAL-PROCESS %d/%d] received %d points from point %d to point %d\n", rank_process, comm_size, num_points_local_process, index_first_point, index_last_point);
    }
    else
    {
        index_first_point = num_points_normal_processes * (comm_size-1);
        if (num_points_local_process > 0)
            {index_last_point = index_first_point + num_points_local_process - 1;
            printf("[MASTER-PROCESS %d/%d] received %d points from point %d to point %d\n", rank_process, comm_size, num_points_local_process, index_first_point, index_last_point);
        }
        else
        {
            printf("[MASTER-PROCESS %d/%d] received %d points (point equally divided on the other cores)\n", rank_process, comm_size, num_points_local_process);
        }
    }

    // Print received points
    print_points(local_points, num_points_local_process, rank_process);  
    // Sort the points in each core
    mergeSort(local_points, num_points_local_process, AXIS);


    if (rank_process != MASTER_PROCESS)
    {
        // All the processes send their ordered points to the master process
        sendPointsPacked(local_points, num_points_local_process, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        // The master process gather all the points from the other processes
        Point **processes_sorted_points;
        processes_sorted_points = (Point **)malloc(comm_size * sizeof(Point *));

        processes_sorted_points[0] = (Point *)malloc((num_points_local_process) * sizeof(Point));
        for (int process = 1; process < comm_size; process++)
        {
            processes_sorted_points[process] = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        }

        // Save first the points ordered from the current process (master), then receive the ones from the other processes
        processes_sorted_points[0] = local_points;
        for (int process = 1; process < comm_size; process++)
        {
            recvPointsPacked(processes_sorted_points[process], num_points_normal_processes, process, 0, MPI_COMM_WORLD);
        }

        // Merge the points from all the cores
        int *temporary_indexes;
        temporary_indexes = (int *)malloc(comm_size * sizeof(int));
        for (int process = 0; process < comm_size; process++)
        {
            temporary_indexes[process] = 0;
        }


        for (int point = 0; point < num_points; point++)
        {        
            int min = INT_MAX; //processes_sorted_points[1][temporary_indexes[1]].coordinates[AXIS];
            int process_with_minimum_value = 0;

            for (int process = 0; process < comm_size; process++)
            {

                if ((process == 0 && temporary_indexes[process] < num_points_master_process) || (process != 0 && temporary_indexes[process] < num_points_normal_processes))
                {

                    if (processes_sorted_points[process][temporary_indexes[process]].coordinates[AXIS] < min)
                    {
                        min = processes_sorted_points[process][temporary_indexes[process]].coordinates[AXIS];
                        process_with_minimum_value = process;
                    }
                    
                }

            }
            // printf("Lowest %d coordinate: %d", point, processes_sorted_points[process_with_minimum_value][temporary_indexes[process_with_minimum_value]].coordinates[AXIS]);
            all_points[point] = processes_sorted_points[process_with_minimum_value][temporary_indexes[process_with_minimum_value]];
            temporary_indexes[process_with_minimum_value]++;
        }

        // Print the sorted points ignoring verbose
        printf("ORDERED POINTS:\n");
        print_points(all_points, num_points, rank_process); 

        // Free processes_sorted_points HERE!!!!!!!!!!!!!!!!!!
    }

    // // Free all points
    // for (int point = 0; point < num_points; point++){
    //     free(all_points[point].coordinates);
    // }
    // free(all_points);

    // // Free local points
    // for (int point = 0; point < num_points_local_process; point++){
    //     free(local_points[point].coordinates);
    // }
    // free(local_points);

    MPI_Finalize();
    return 0;
}