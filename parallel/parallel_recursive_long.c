#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/util.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2147483647
#define VERBOSE 0
#define ENUMERATE_PAIRS_OF_POINTS 1
#define PRINT_PAIRS_OF_POINTS 0

/* COSE DA FARE
0. Importare file da argomento
1. Sistemare notazione funzioni e variabili
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
    char path[] = "../point_generator/1H2d.txt";
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
        if (comm_size < 2){
            perror("Error: cannot run parallel application over just 1 process\n");
            return -1;
        }
        fclose(point_file);
    }
    // Send num_points to all the processes, it is used to compute the num_points variables
    MPI_Bcast(&num_points, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    MPI_Bcast(&num_dimensions, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    
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
            // Pairs will store the pairs of points with minimum distance and their distance data
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

                getUniquePairs(pairs, global_dmin, rank_process, comm_size, ENUMERATE_PAIRS_OF_POINTS, ENUMERATE_PAIRS_OF_POINTS);
            
            free(pairs);
          
            // Free all points and pairs
            for (int point = 0; point < num_points; point++)
                free(all_points[point].coordinates);
            free(all_points);
            free(pairs);
        }   

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
    else
    {

        // Points are divided equally on all processes exept master process which takes the remaing points
        num_points_normal_processes = num_points / (comm_size-1);
        num_points_master_process = num_points % (comm_size-1);
    }

    if(rank_process == MASTER_PROCESS)
        printf("Launching parallel algorithm...\n");

    Point *local_points = NULL;

    // POINT 1 - divide points and send them with their out_of_region values to each process
    int left_x_out_of_region = INT_MAX, right_x_out_of_region = INT_MAX;
    if (rank_process == MASTER_PROCESS)
    {
        // Transfer total number of points and the correct slice of points to work on for every process
        Point *points_to_send;
        points_to_send = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        int left_x_to_send = 0, right_x_to_send = 0;
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
        for (int point = 0; point < num_points_received; point++)
            free(received_points[point].coordinates);
        free(received_points);


        // POINT 7 - Reorder point in the local_strip_points over y. Then each process compute the min distance for its strip
        mergeSort(local_strip_points, num_points_local_strip, 1);
        double dmin_local_strip = global_dmin;

        // Compare each point with the next point ordered according to y. As soon as the distance is greater than dmin_local_strip then break that cycle.
        for (int i = 0; i < num_points_local_strip - 1; i++){
            for (int k = i + 1; k < num_points_local_strip && abs(local_strip_points[k].coordinates[1] - local_strip_points[i].coordinates[1]) <= global_dmin; k++){
                double current_distance = distance(local_strip_points[i], local_strip_points[k]);
                
                // If the new distance is smaller than the dmin_local_strip then update it, then save a new min_distance in pairs and push the two points inside pairs
                if (current_distance < dmin_local_strip){ // IS IT REDUNDAND?? WE CAN MERGE THIS IF WITH THE ONE BELOW AND PUT THE CONDITIONS IN &&
                    dmin_local_strip = current_distance;
                }
                if(current_distance < pairs->min_distance){
                    pairs->min_distance = current_distance;
                    pairs->points1[0] = local_strip_points[i];
                    pairs->points2[0] = local_strip_points[k];
                    pairs->num_pairs = 1;
                }
                // If the new distance is equal to the smalles one found, then push the two points inside pairs and increment the num_pairs found with that distance
                else if(current_distance == pairs->min_distance){
                    pairs->points1[pairs->num_pairs] = local_strip_points[i];
                    pairs->points2[pairs->num_pairs] = local_strip_points[k];
                    pairs->num_pairs++;
                }
            }
        }
        // Update each local_dmin with the new dmin_local_strip just found. Then free local_strip_points
        local_dmin = dmin_local_strip;
        // for (int point = 0; point < num_points_local_strip; point++)  // Using this we have crazy point coordinates with 1Hh2d using 6 cores
        //     free(local_strip_points[point].coordinates);
        free(local_strip_points);
    }
    
    // POINT 8 - Allreduce the local_dmin of each process to find the global_dmin and give this value to all the processes. Then work on the pairs of points
    MPI_Allreduce(&local_dmin, &global_dmin, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
        printf("---NEW GLOBAL DMIN: %f\n", global_dmin);

    getUniquePairs(pairs, global_dmin, rank_process, comm_size, ENUMERATE_PAIRS_OF_POINTS, ENUMERATE_PAIRS_OF_POINTS);

    // Free all points, its internal parameter are deallocated only by MASTER_PROCESS since it is the only process which has them
    if (rank_process == MASTER_PROCESS)
        for (int point = 0; point < num_points; point++)
            free(all_points[point].coordinates);
    free(all_points);

    // Free local points, its internal parameter are deallocated only by the processes different from MASTER_PROCESS. since it is the only process which has them
    // This because the one inside master process are deallocated once free(all_points) is performed since they are built as a copy of the addresse of all_points. 
    // Instead local_points in other processes are rebuilt by MPI coying the values. For this reason they need to be deallocated singularly.
    // if (rank_process != MASTER_PROCESS)

    for (int point = 0; point < num_points_local_process; point++)
        free(local_points[point].coordinates);
    free(local_points);

    // Left and right point are copy of local points and so are already deallocated in their internal parameters by the calls above
    for (int point = 0; point < num_points_left_partial_strip; point++)
        free(left_partial_strip_points[point].coordinates);
    free(left_partial_strip_points);

    for (int point = 0; point < num_points_right_partial_strip; point++)
        free(right_partial_strip_points[point].coordinates);
    free(right_partial_strip_points);
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