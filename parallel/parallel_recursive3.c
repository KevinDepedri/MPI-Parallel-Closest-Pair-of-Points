#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/util.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2147483647
#define VERBOSE 0
#define PRINT_PAIRS_OF_POINTS 1

/* COSE DA FARE
0. Importare file da argomento
1. Sistemare notazione funzioni e variabili
2. Verificare che ogni malloc abbia il proprio free (ALMOST DONE)
3. Definire come gestire print/VERBOSE
4. Valutare come migliorare la leggibilitá del codice
*/

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
    char path[] = "../point_generator/10K5d.txt";
    // char path[] = argv[1];

    // Get the total number of points and the number of dimensions
    int num_points = 0, num_dimensions = 0;
    if (rank_process == MASTER_PROCESS)
    {
        // Open input point file on master process
        FILE *point_file = fopen(path, "r"); 
        if (point_file == NULL){
            perror("Error opening file on master process\n");
            return -1;
        }

        // Read the number of points and dimensions from the first line of the file
        fscanf(point_file, "%d %d", &num_points, &num_dimensions);

        // Give error if the points or processes are not enough
        if (num_points < 2){
            perror("Error: the number of points must be greater than 1\n");
            return -1;
        }
        if (num_points < comm_size){
            perror("Error: the number of points must be greater than the number of processes\n");
            return -1;
        }
        fclose(point_file);
    }
    
    // Get the points data for all the points and order them according to x coordinate
    Point *all_points = NULL;
    all_points = parallel_mergesort(all_points, path, rank_process, comm_size, VERBOSE); // PUT HERE BACK THE NON COMPRESSED VERSION
    if (rank_process == MASTER_PROCESS)
    {
        if (all_points == NULL){
            perror("Error in executing parallel_mergesort\n");
            return -1;
        }
    }

    // Points are divided equally on all processes exept master process which takes the remaing points
    int num_points_normal_processes = 0, num_points_master_process = 0, num_points_local_process = 0;
    Point *local_points = NULL;
    // Pairs will store the pairs of points with minimum distance and their distance data
    Pairs *pairs = NULL;

    // If the code is ran on two or less processes, then run the sequential version of the problem.
    // Indeed, in this implementation process 0 is supposed to be the MASTER_PROCESS, which just supervises the operations, manages the transfer 
    // of data and carries out computation over a reduced number of points (the reminder dividing the points on all the other processes). 
    // For this reason, when working with 2 processes MASTER_PROCESS will be idle. Therefore, all the computation will be done sequentially on process 1.
    if(comm_size <= 2)
    {
        pairs = (Pairs *)malloc(sizeof(Pairs));

        pairs->points1 = (Point *)malloc((num_points) * sizeof(Point));
        pairs->points2 = (Point *)malloc((num_points) * sizeof(Point));
        for (int i = 0; i < (num_points); i++){
            pairs->points1[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            pairs->points2[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        }

        pairs->num_pairs = 0;
        pairs->min_distance = INT_MAX;

        if(rank_process == MASTER_PROCESS){
            printf("Launching sequential algorithm...\n");
            recSplit(all_points, num_points, pairs, rank_process);
            printf("SUPER FINAL GLOBAL DMIN: %f\n", pairs->min_distance);

            if (PRINT_PAIRS_OF_POINTS == 1){
                // Insert here print of pairs??
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);

        // Free all_points and also pairs
        free(all_points);
        free(pairs);

        if(rank_process == MASTER_PROCESS){
            printf("Memory free\n");
            end = clock();
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            printf("Time elapsed: %f\n", cpu_time_used);
        }

        MPI_Finalize();
        return 0;
    }
    // If the code is ran on more than two processes, then run the parallel version of the problem.
    else
    {
        // Send num_points to all the processes, it is used to compute the num_points variables
        MPI_Bcast(&num_points, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
        MPI_Bcast(&num_dimensions, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);

        // Points are divided equally on all processes exept master process which takes the remaing points
        num_points_normal_processes = num_points / (comm_size-1);
        num_points_master_process = num_points % (comm_size-1);
    }

    if(rank_process == MASTER_PROCESS)
        printf("Launching parallel algorithm...\n");

    // POINT 1 - divide points and send them with their out_of_region values to each process
    int left_x_out_of_region = INT_MAX, right_x_out_of_region = INT_MAX;
    if (rank_process == MASTER_PROCESS)
    {
        // Transfer total number of points and the correct slice of points to work on for every process
        Point *points_to_send;
        points_to_send = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        int left_to_send, right_to_send;
        for (int process = 1; process < comm_size; process++){

            for (int point = 0; point < num_points_normal_processes; point++){
                points_to_send[point].num_dimensions = num_dimensions;
                points_to_send[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            
                for (int dimension = 0; dimension < num_dimensions; dimension++)
                    points_to_send[point].coordinates[dimension] = all_points[point+num_points_normal_processes*(process-1)].coordinates[dimension];
            }
            MPI_Send(&num_points_normal_processes, 1, MPI_INT, process, 0, MPI_COMM_WORLD);
            sendPointsPacked(points_to_send, num_points_normal_processes, process, 1, MPI_COMM_WORLD);
            
            // Transfer also the left and right out_of_region x coordinate for each process
            if (process == 1){
                left_to_send = INT_MAX;
                right_to_send = all_points[num_points_normal_processes*(process)].coordinates[AXIS];
            }
            else{
                left_to_send = all_points[num_points_normal_processes*(process-1)-1].coordinates[AXIS]; 
                if (process != comm_size - 1 || num_points_master_process != 0)
                    right_to_send = all_points[num_points_normal_processes*(process)].coordinates[AXIS];
                else
                    right_to_send = INT_MAX;
            }
            MPI_Send(&left_to_send, 1, MPI_INT, process, 2, MPI_COMM_WORLD);
            MPI_Send(&right_to_send, 1, MPI_INT, process, 3, MPI_COMM_WORLD);
        }

        // Free points_to_send once the send for all the processes is done 
        for (int point = 0; point < num_points_normal_processes; point++)  // SHOUDLN'T THIS BE A PROBLEM? WE COULD REMOVE DATA FROM ALL_POINTS WHICH HAS THE SAME ADDRESSES
            free(points_to_send[point].coordinates);
        free(points_to_send);

        // Store the points of the master process and its out_of_region values
        num_points_local_process = num_points_master_process;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        int starting_from = num_points_normal_processes * (comm_size - 1);

        for (int i = 0; i + starting_from < num_points; i++){
            local_points[i].num_dimensions = num_dimensions;
            local_points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));

            for (int dimension = 0; dimension < num_dimensions; dimension++)
                local_points[i].coordinates[dimension] = all_points[i + starting_from].coordinates[dimension];
        }
        left_x_out_of_region = all_points[starting_from-1].coordinates[AXIS];
        right_x_out_of_region = INT_MAX;
    }
    else
    {
        // Receive number of points and the points data that will be processed by this process
        MPI_Recv(&num_points_normal_processes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        num_points_local_process = num_points_normal_processes;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        recvPointsPacked(local_points, num_points_local_process, 0, 1, MPI_COMM_WORLD);

        // Receive the left and right out_of_region x coordinate for this process
        MPI_Recv(&left_x_out_of_region, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&right_x_out_of_region, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }


    // POINT 2 - compute min distance for each process
    double local_dmin = INT_MAX;
    pairs = (Pairs *)malloc(sizeof(Pairs));

    pairs->points1 = (Point *)malloc((num_points_local_process) * sizeof(Point));
    pairs->points2 = (Point *)malloc((num_points_local_process) * sizeof(Point));
    for (int i = 0; i < (num_points_local_process); i++){
        pairs->points1[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        pairs->points2[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
    }

    pairs->num_pairs = 0;
    pairs->min_distance = INT_MAX;

    if (num_points_local_process > 1){
        recSplit(local_points, num_points_local_process, pairs, rank_process);
        local_dmin = pairs->min_distance;
    }
    printf("PROCESS:%d DMIN:%f\n", rank_process, local_dmin);


    // POINT 3 - allreduce to find the global dmin
    double global_dmin = INT_MAX;
    MPI_Allreduce(&local_dmin, &global_dmin, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
        printf("---GLOBAL DMIN: %f\n", global_dmin);
    

    // POINT 4 - compute the boundary between processes
    double left_boundary = -INT_MAX, right_boundary = INT_MAX;

    if (rank_process != 1 && num_points_local_process != 0)
        left_boundary = (left_x_out_of_region + local_points[0].coordinates[AXIS])/2.0;

    if(rank_process != MASTER_PROCESS && (rank_process != comm_size - 1 || num_points_master_process != 0))
        right_boundary = (right_x_out_of_region + local_points[num_points_local_process-1].coordinates[AXIS])/2.0;
    

    // POINT 5 - get the points in the lateral strips for each process, get for both left side strip and right side strip
    int num_points_left_partial_strip = 0, num_points_right_partial_strip = 0;
    Point *left_partial_strip_points = (Point *)malloc(num_points_local_process * sizeof(Point));
    Point *right_partial_strip_points = (Point *)malloc(num_points_local_process * sizeof(Point));

    // To get left_partial_strip_points check each point in the ordered list of local points from left to right. As soon as one points is out we can break
    for (int point = 0; point < num_points_local_process; point++){
        if (local_points[point].coordinates[AXIS] <= left_boundary + global_dmin){

            left_partial_strip_points[num_points_left_partial_strip].num_dimensions = num_dimensions;
            left_partial_strip_points[num_points_left_partial_strip].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                left_partial_strip_points[num_points_left_partial_strip].coordinates[dimension] = local_points[point].coordinates[dimension];
            
            num_points_left_partial_strip++;
        }
        else
            break;
    }

    // To get left_partial_strip_points check each point in the ordered list of local points from right to left. As soon as one points is out we can break
    for(int point = num_points_local_process - 1; point >= 0; point--){
        if (local_points[point].coordinates[AXIS] >= right_boundary - global_dmin){

            right_partial_strip_points[num_points_right_partial_strip].num_dimensions = num_dimensions;
            right_partial_strip_points[num_points_right_partial_strip].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                right_partial_strip_points[num_points_right_partial_strip].coordinates[dimension] = local_points[point].coordinates[dimension];
            
            num_points_right_partial_strip++;
        }
        else
            break;
    }


    // POINT 6 - move points of left side strip to the point to the left process. Then merge the moved left points with the right points of the target process
    if (rank_process != 1  && rank_process != MASTER_PROCESS)
    {
        // Send the number of points to the left process
        MPI_Send(&num_points_left_partial_strip, 1, MPI_INT, rank_process-1, 0, MPI_COMM_WORLD);
        
        // Send the points to the left process
        if (num_points_left_partial_strip != 0)
            sendPointsPacked(left_partial_strip_points, num_points_left_partial_strip, rank_process-1, 1, MPI_COMM_WORLD);
    }
    if(rank_process == MASTER_PROCESS){
        // Send the number of points to the left process
        MPI_Send(&num_points_left_partial_strip, 1, MPI_INT, comm_size-1, 0, MPI_COMM_WORLD);
       
        // Send the points to the left process
        if (num_points_left_partial_strip != 0)
            sendPointsPacked(left_partial_strip_points, num_points_left_partial_strip, comm_size-1, 1, MPI_COMM_WORLD);
    }

    if(rank_process != MASTER_PROCESS)
    {
        // Receive the number of points from the right process
        int num_points_received;
        if(rank_process != comm_size - 1)
            MPI_Recv(&num_points_received, 1, MPI_INT, rank_process+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        else
            MPI_Recv(&num_points_received, 1, MPI_INT, MASTER_PROCESS, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Receive the points from the right process
        Point *received_points = (Point *)malloc(num_points_received * sizeof(Point));
        if (num_points_received != 0){
            if(rank_process != comm_size - 1)
                recvPointsPacked(received_points, num_points_received, rank_process+1, 1, MPI_COMM_WORLD);
            else
                recvPointsPacked(received_points, num_points_received, MASTER_PROCESS, 1, MPI_COMM_WORLD);
        }
        
        // Merge right_partial_strip_points of the current process with the received_points coming from the right process in the local_strip_points
        int num_points_local_strip = num_points_right_partial_strip + num_points_received;
        Point *local_strip_points = (Point *)malloc((num_points_local_strip) * sizeof(Point));

        // First push the values from the right strip
        for (int i = 0; i < num_points_right_partial_strip; i++){
            local_strip_points[i].num_dimensions = num_dimensions;
            local_strip_points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                local_strip_points[i].coordinates[dimension] = right_partial_strip_points[i].coordinates[dimension];
        }
        // Then push the values recieved from the process on the right
        for (int i = 0; i < num_points_received; i++){
            local_strip_points[i+num_points_right_partial_strip].num_dimensions = num_dimensions;
            local_strip_points[i+num_points_right_partial_strip].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                local_strip_points[i+num_points_right_partial_strip].coordinates[dimension] = received_points[i].coordinates[dimension];
        }
        free(received_points);


        // POINT 7 - Reorder point in the local_strip_points over y. Then each process compute the min distance for its strip
        mergeSort(local_strip_points, num_points_local_strip, 1);
        
        double dmin_local_strip = global_dmin;
        for (int i = 0; i < num_points_local_strip - 1; i++){
            for (int k = i + 1; k < num_points_local_strip && abs(local_strip_points[k].coordinates[1] - local_strip_points[i].coordinates[1]) <= global_dmin; k++){
                double current_distance = distance(local_strip_points[i], local_strip_points[k]);
                if (current_distance < dmin_local_strip){
                    dmin_local_strip = current_distance;
                }
                if(current_distance < pairs->min_distance){
                    pairs->min_distance = current_distance;
                    pairs->points1[0] = local_strip_points[i];
                    pairs->points2[0] = local_strip_points[k];
                    pairs->num_pairs = 1;
                }
                else if(current_distance == pairs->min_distance){
                    pairs->points1[pairs->num_pairs] = local_strip_points[i];
                    pairs->points2[pairs->num_pairs] = local_strip_points[k];
                    pairs->num_pairs++;
                }
            }
        }
        local_dmin = dmin_local_strip;
        free(local_strip_points);
    }
    
    //11. REDUCE ALL DISTANCES BACK TO MASTER PROCESS
    MPI_Allreduce(&local_dmin, &global_dmin, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
        printf("---NEW GLOBAL DMIN: %f\n", global_dmin);

    if (PRINT_PAIRS_OF_POINTS == 1){
        int loca_number_pairs_min_distance = 0, number_pairs_min_distance = 0;

        if (global_dmin == pairs->min_distance)
        {
            // clean pairs from repeated points (even if inverted)
            int *cleaned_list_indexes = (int *)malloc(pairs->num_pairs * sizeof(int));
            int cleaned_list_size = 0;
            for (int i = 0; i < pairs->num_pairs; i++)
            {
                int flag = 0;
                for (int j = i + 1; j < pairs->num_pairs; j++)
                {
                    if (!((differPoint(pairs->points1[i], pairs->points1[j]) && differPoint(pairs->points2[i], pairs->points2[j])) &&
                        (differPoint(pairs->points1[i], pairs->points2[j]) || differPoint(pairs->points2[i], pairs->points1[j]))))
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0)
                {
                    cleaned_list_indexes[cleaned_list_size] = i;
                    cleaned_list_size++;
                }
            }
            for(int i = 0; i < cleaned_list_size; i++){
                if (VERBOSE == 0)
                {
                    printPoint(pairs->points1[cleaned_list_indexes[i]]);
                    printPoint(pairs->points2[cleaned_list_indexes[i]]);
                    printf("\n");
                }
                loca_number_pairs_min_distance++;
            }
        }
        MPI_Reduce(&loca_number_pairs_min_distance, &number_pairs_min_distance, 1, MPI_INT, MPI_SUM, MASTER_PROCESS, MPI_COMM_WORLD);
        if (rank_process==MASTER_PROCESS)
            printf("Found a total of %d pairs of points with DMIN=%f\n", number_pairs_min_distance, global_dmin);
    }

    // Free all points and local points
    if (rank_process == MASTER_PROCESS)
    {   
        // Deallocating the internal parameter on all_points leads to the same effect also on local_points
        for (int point = 0; point < num_points; point++)
            free(all_points[point].coordinates);
    }
    free(all_points);
    free(local_points); 
    free(pairs);
    free(left_partial_strip_points);
    free(right_partial_strip_points);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
    {
        printf("Memory free\n");
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Time elapsed: %f\n", cpu_time_used);
    }

    MPI_Finalize();
    return 0;
}