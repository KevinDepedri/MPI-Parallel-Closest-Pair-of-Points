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
    if(argc < 2){
        if (rank_process == MASTER_PROCESS)
            perror("ERROR: no file name or path has been provided as first argument (points input file)\n");
        return -1;
    }

    int MERGESORT_VERBOSE = 0, ENUMERATE_PAIRS_OF_POINTS = 0, PRINT_PAIRS_OF_POINTS = 0, INVALID_FLAG = 0;
    for (size_t option_id = 2; option_id < argc; option_id++){
        switch (argv[option_id][1]){
            case 'v': MERGESORT_VERBOSE = 1; break;
            case 'e': ENUMERATE_PAIRS_OF_POINTS = 1; break;
            case 'p': ENUMERATE_PAIRS_OF_POINTS = 1; PRINT_PAIRS_OF_POINTS = 1; break;
            default: INVALID_FLAG = 1; break;
        }
    }
    if (INVALID_FLAG == 1){
        if (rank_process == MASTER_PROCESS)
            perror("ERROR: the only valid flag arguments are:\n \t-v : verbose enabled during mergesort part of the algorithm\n \t-e : enumerate the pairs of point with smallest distance\n \t-p : print the pairs of point with smallest distance\n");
        return -1;
    }

    // Get the total number of points and the number of dimensions
    int num_points = 0, num_dimensions = 0;
    if (rank_process == MASTER_PROCESS)
    {
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
            perror("ERROR: use the 2cores file\n");
            return -1;
        }
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
    int index_first_point = 0, index_last_point = 0;

    Point *local_points = NULL;

    if (rank_process == MASTER_PROCESS)
    {
        // Open input point file on master process
        // FILE *point_file = fopen(path, "r"); 
        FILE *point_file = fopen(argv[1], "r"); 
        if (point_file == NULL){
            perror("ERROR opening file on master process\n");
            return -1;
        }

        // Read the number of points and dimensions from the first line of the file
        fscanf(point_file, "%d %d", &num_points, &num_dimensions);

        // Points are divided equally on all processes exept master process which takes the remaing points
        num_points_normal_processes = num_points / (comm_size-1);
        num_points_master_process = num_points % (comm_size-1);

        // Read the points and store them in the master process
        all_points = (Point *)malloc(num_points * sizeof(Point));
        for (int point = 0; point < num_points; point++){
            all_points[point].num_dimensions = num_dimensions;
            all_points[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                fscanf(point_file, "%d", &all_points[point].coordinates[dimension]);
        }
        fclose(point_file);

        if (MERGESORT_VERBOSE == 1){
            printf("INPUT LIST OF POINTS:\n");
            printPointsFromProcess(all_points, num_points, rank_process);    
        }        
        
        // Transfer total number of points and the correct slice of points to work on for every process
        Point *points_to_send = NULL;
        points_to_send = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        for (int point = 0; point < num_points_normal_processes; point++)
                points_to_send[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));

        for (int process = 1; process < comm_size; process++){
            for (int point = 0; point < num_points_normal_processes; point++){
                points_to_send[point].num_dimensions = num_dimensions;
                
                for (int dimension = 0; dimension < num_dimensions; dimension++)
                    points_to_send[point].coordinates[dimension] = all_points[point+num_points_normal_processes*(process-1)].coordinates[dimension];
            }
            MPI_Send(&num_points_normal_processes, 1, MPI_INT, process, 0, MPI_COMM_WORLD);
            sendPointsPacked(points_to_send, num_points_normal_processes, process, 1, MPI_COMM_WORLD);
        }

        // Free points to send once the send for all the processes is done 
        for (int point = 0; point < num_points_normal_processes; point++)
            free(points_to_send[point].coordinates);
        free(points_to_send);

        // Store the points of the master process
        num_points_local_process = num_points_master_process;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        int starting_from = num_points_normal_processes * (comm_size - 1);
        for (int point = 0; point + starting_from < num_points; point++){
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
        recvPointsPacked(local_points, num_points_local_process, 0, 1, MPI_COMM_WORLD);
    }

    if (MERGESORT_VERBOSE == 1){
        // Compute the number of points that will be read by each core and print it
        if (rank_process != MASTER_PROCESS)
        {
            index_first_point = num_points_local_process * (rank_process-1);
            index_last_point = index_first_point + num_points_local_process - 1;
            printf("[NORMAL-PROCESS %d/%d] received %d points from point %d to point %d\n", rank_process, comm_size, num_points_local_process, index_first_point, index_last_point);
        }
        else
        {
            index_first_point = num_points_normal_processes * (comm_size-1);
            if (num_points_local_process > 0){
                index_last_point = index_first_point + num_points_local_process - 1;
                printf("[MASTER-PROCESS %d/%d] received %d points from point %d to point %d\n", rank_process, comm_size, num_points_local_process, index_first_point, index_last_point);
            }
            else{
                printf("[MASTER-PROCESS %d/%d] received %d points (point equally divided on the other cores)\n", rank_process, comm_size, num_points_local_process);
            }
        }

        // Print received points
        printPointsFromProcess(local_points, num_points_local_process, rank_process);  
    }

    // Sort the received points in each core
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
            processes_sorted_points[process] = (Point *)malloc(num_points_normal_processes * sizeof(Point));

        // Save first the points ordered from the current process (master), then receive the ones from the other processes
        processes_sorted_points[0] = local_points;
        for (int process = 1; process < comm_size; process++)
            recvPointsPacked(processes_sorted_points[process], num_points_normal_processes, process, 0, MPI_COMM_WORLD);

        // Merge the ordere points from all the processes
        int *temporary_indexes;
        temporary_indexes = (int *)malloc(comm_size * sizeof(int));
        for (int process = 0; process < comm_size; process++)
            temporary_indexes[process] = 0;

        // For each position of the final ordered array of points define an initial minimum value and the process that encompasses this value
        for (int point = 0; point < num_points; point++){        
            int min = INT_MAX;
            int process_with_minimum_value = 0;

            // For that previous position look in all the processes to find the minimum value
            for (int process = 0; process < comm_size; process++){
                // For each specific process look to its first point (since it is the lowest), if there are still points not used in that process 
                if ((process == 0 && temporary_indexes[process] < num_points_master_process) || (process != 0 && temporary_indexes[process] < num_points_normal_processes)){
                    // If a new minimum is reached then save it as minimum and save the process that led to it
                    if (processes_sorted_points[process][temporary_indexes[process]].coordinates[AXIS] < min){
                        min = processes_sorted_points[process][temporary_indexes[process]].coordinates[AXIS];
                        process_with_minimum_value = process;
                    }
                }
            }
            // One a value is confirmed as the lowest between all the compared ones, then save it in the fianl array of ordered points
            all_points[point] = processes_sorted_points[process_with_minimum_value][temporary_indexes[process_with_minimum_value]];
            temporary_indexes[process_with_minimum_value]++;
        }

        if (MERGESORT_VERBOSE == 1){
            printf("\nORDERED POINTS:\n");
            printPointsFromProcess(all_points, num_points, rank_process); 
        }

        free(temporary_indexes);
        for (int process = 1; process < comm_size; process++)
            free(processes_sorted_points[process]);
        free(processes_sorted_points);
    }
    
    if (rank_process != MASTER_PROCESS)
        for (int point = 0; point < num_points_local_process; point++)
            free(local_points[point].coordinates);
    free(local_points);

    // Pairs will store the pairs of points with minimum distance and their distance data
    Pairs *pairs = NULL;

    // If the code is ran on two processes, then it run the sequential version of the problem.
    // Indeed, in this parallel implementation process 0 is supposed to be the MASTER_PROCESS, which just supervises the operations, manages the transfer 
    // of data and carries out computation over a reduced number of points (the reminder dividing the points on all the other processes). 
    // For this reason, when working with 2 processes MASTER_PROCESS will be idle. Therefore, all the computation will be done sequentially on process 1.
    // For the same reason working on just one process is not feasible since the MASTER_PROCESS is not designed to carry out all the computations alone.
    if(comm_size == 2)
    {
        if(rank_process == MASTER_PROCESS){
            pairs = (Pairs *)malloc(sizeof(Pairs));

            pairs->points1 = (Point *)malloc((num_points) * sizeof(Point));
            pairs->points2 = (Point *)malloc((num_points) * sizeof(Point));
            for (int i = 0; i < (num_points); i++){
                pairs->points1[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
                pairs->points2[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            }
            pairs->num_pairs = 0;
            pairs->min_distance = INT_MAX;

            printf("Launching sequential algorithm...\n");
            recSplit(all_points, num_points, pairs, rank_process);
            double global_dmin = pairs->min_distance;
            printf("---GLOBAL DMIN: %f\n", global_dmin);

            getUniquePairs(pairs, global_dmin, rank_process, comm_size, ENUMERATE_PAIRS_OF_POINTS, PRINT_PAIRS_OF_POINTS);
          
            // Free all the memory previously allocated
            // Free all_points, its internal parameter are deallocated only by MASTER_PROCESS since it is the only process which has them
            printf("Free all points...\n");
            for (int point = 0; point < num_points; point++)
                free(all_points[point].coordinates);
            free(all_points);
            // pairs internal parameter are already deallocated calling free on all_points since they are the same
            printf("Free pairs...\n");
            free(pairs);
        }   

        // Use a barrier to alling all the cores and then get the execution time
        MPI_Barrier(MPI_COMM_WORLD);
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
    if(rank_process == MASTER_PROCESS)
        printf("Launching parallel algorithm...\n");

    // POINT 1 - divide points and send them with their out_of_region values to each process
    int left_x_out_of_region = INT_MAX, right_x_out_of_region = INT_MAX;
    if (rank_process == MASTER_PROCESS)
    {
        // Transfer total number of points and the correct slice of points to work on for every process
        Point *points_to_send = NULL;
        points_to_send = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        for (int point = 0; point < num_points_normal_processes; point++)
            points_to_send[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));

        int left_x_to_send = 0, right_x_to_send = 0;
        for (int process = 1; process < comm_size; process++){

            for (int point = 0; point < num_points_normal_processes; point++){
                points_to_send[point].num_dimensions = num_dimensions;
            
                for (int dimension = 0; dimension < num_dimensions; dimension++)
                    points_to_send[point].coordinates[dimension] = all_points[point+num_points_normal_processes*(process-1)].coordinates[dimension];
            }
            MPI_Send(&num_points_normal_processes, 1, MPI_INT, process, 0, MPI_COMM_WORLD);
            sendPointsPacked(points_to_send, num_points_normal_processes, process, 1, MPI_COMM_WORLD);
            
            // Transfer also the left and right out_of_region x coordinate for each process
            if (process == 1){
                left_x_to_send = INT_MAX;
                right_x_to_send = all_points[num_points_normal_processes*(process)].coordinates[AXIS];
            }
            else{
                left_x_to_send = all_points[num_points_normal_processes*(process-1)-1].coordinates[AXIS]; 
                if (process != comm_size - 1 || num_points_master_process != 0)
                    right_x_to_send = all_points[num_points_normal_processes*(process)].coordinates[AXIS];
                else
                    right_x_to_send = INT_MAX;
            }
            MPI_Send(&left_x_to_send, 1, MPI_INT, process, 2, MPI_COMM_WORLD);
            MPI_Send(&right_x_to_send, 1, MPI_INT, process, 3, MPI_COMM_WORLD);
        }

        // Free points_to_send once the send for all the processes is done 
        for (int point = 0; point < num_points_normal_processes; point++)  // SHOUDLN'T THIS BE A PROBLEM? WE COULD REMOVE DATA FROM ALL_POINTS WHICH HAS THE SAME ADDRESSES
            free(points_to_send[point].coordinates);
        free(points_to_send);

        // Store the points of the master process and its out_of_region values
        num_points_local_process = num_points_master_process;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        int starting_from = num_points_normal_processes * (comm_size - 1);

        for (int point = 0; point + starting_from < num_points; point++){
            local_points[point].num_dimensions = num_dimensions;
            local_points[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));

            for (int dimension = 0; dimension < num_dimensions; dimension++)
                local_points[point].coordinates[dimension] = all_points[point + starting_from].coordinates[dimension];
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

    // POINT 3 - Allreduce the local_dmin of each process to find the global_dmin and give this value to all the processes
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
        for (int point = 0; point < num_points_right_partial_strip; point++){
            local_strip_points[point].num_dimensions = num_dimensions;
            local_strip_points[point].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                local_strip_points[point].coordinates[dimension] = right_partial_strip_points[point].coordinates[dimension];
        }
        // Then push the values recieved from the process on the right
        for (int point = 0; point < num_points_received; point++){
            local_strip_points[point+num_points_right_partial_strip].num_dimensions = num_dimensions;
            local_strip_points[point+num_points_right_partial_strip].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                local_strip_points[point+num_points_right_partial_strip].coordinates[dimension] = received_points[point].coordinates[dimension];
        }

        // POINT 7 - Reorder point in the local_strip_points over y. Then each process compute the min distance for its strip
        mergeSort(local_strip_points, num_points_local_strip, 1);
        double dmin_local_strip = global_dmin;

        // Compare each point with the next point ordered according to y. As soon as the distance is greater than dmin_local_strip then break that cycle.
        for (int point = 0; point < num_points_local_strip - 1; point++){
            for (int next_point = point + 1; next_point < num_points_local_strip && abs(local_strip_points[next_point].coordinates[1] - local_strip_points[point].coordinates[1]) <= global_dmin; next_point++){
                double current_distance = distance(local_strip_points[point], local_strip_points[next_point]);
                
                // If the new distance is smaller than the dmin_local_strip then update it, then save a new min_distance in pairs and push the two points inside pairs
                if (current_distance < dmin_local_strip){ // IS IT REDUNDAND?? WE CAN MERGE THIS IF WITH THE ONE BELOW AND PUT THE CONDITIONS IN &&
                    dmin_local_strip = current_distance;
                }
                if(current_distance < pairs->min_distance){
                    pairs->min_distance = current_distance;
                    pairs->points1[0] = local_strip_points[point];
                    pairs->points2[0] = local_strip_points[next_point];
                    pairs->num_pairs = 1;
                }
                // If the new distance is equal to the smalles one found, then push the two points inside pairs and increment the num_pairs found with that distance
                else if(current_distance == pairs->min_distance){
                    pairs->points1[pairs->num_pairs] = local_strip_points[point];
                    pairs->points2[pairs->num_pairs] = local_strip_points[next_point];
                    pairs->num_pairs++;
                }
            }
        }
        // Update each local_dmin with the new dmin_local_strip just found. 
        local_dmin = dmin_local_strip;

        // Free received_points and local_strip_points
        for (int point = 0; point < num_points_received; point++)
            free(received_points[point].coordinates);
        free(received_points);
        free(local_strip_points);
    }
    
    // POINT 8 - Allreduce the local_dmin of each process to find the global_dmin and give this value to all the processes. Then work on the pairs of points
    MPI_Allreduce(&local_dmin, &global_dmin, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
        printf("---NEW GLOBAL DMIN: %f\n", global_dmin);

    getUniquePairs(pairs, global_dmin, rank_process, comm_size, ENUMERATE_PAIRS_OF_POINTS, PRINT_PAIRS_OF_POINTS);

    // Free all the memory previously allocated
    // Free all_points, its internal parameter are deallocated only by MASTER_PROCESS since it is the only process which has them
    if (rank_process == MASTER_PROCESS)
        printf("Free all points...\n");
    if (rank_process == MASTER_PROCESS)
        for (int point = 0; point < num_points; point++)
            free(all_points[point].coordinates);
    free(all_points);

    // local_points internal parameter for MASTER_PROCESS are already deallocated calling free on all_points since they are the same inside MASTER_PROCESS
    if (rank_process == MASTER_PROCESS)
        printf("Free local points...\n");
    if (rank_process != MASTER_PROCESS)
        for (int point = 0; point < num_points_local_process; point++)
            free(local_points[point].coordinates);
    free(local_points);

    // left_partial_strip_points internal parameter are already deallocated calling free on local_points since they are the same
    if (rank_process == MASTER_PROCESS)
        printf("Free left strip points...\n");
    free(left_partial_strip_points);

    // right_partial_strip_points internal parameter are already deallocated calling free on local_points since they are the same
    if (rank_process == MASTER_PROCESS)
        printf("Free right strip points...\n");
    free(right_partial_strip_points);

    // pairs internal parameter are already deallocated calling free on local_points since they are the same
    if (rank_process == MASTER_PROCESS)
        printf("Free pairs...\n");
    free(pairs);
    
    // Use a barrier to alling all the cores and then get the execution time
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
    {
        printf("Memory free\n");
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Time elapsed: %f\n", cpu_time_used);
    }

    // Finalize the MPI application
    MPI_Finalize();
    return 0;
}