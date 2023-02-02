#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils/util3.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2147483647
#define VERBOSE 0

/* COSE DA FARE
0. Importare file da argomento
1. Sistemare notazione funzioni e variabili
2. Verificare che ogni malloc abbia il proprio free (effettuando test sul codice per essere sicuri che non cambino i risultati/crash del codice)
3. Definire come gestire print/VERBOSE
4. Valutare come migliorare la leggibilitá del codice
5. Verificare usando distance.py che i risultati siano corretti
*/

int main(int argc, char *argv[])
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    MPI_Init(&argc, &argv);
    int rank_process, comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_process);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    char path[] = "../point_generator/1H2d.txt";
    //char path[] = argv[1]

    int num_points, num_dimensions;

    if (rank_process == MASTER_PROCESS)
    {
        // Open input point file on master process
        FILE *point_file = fopen(path, "r"); 
        if (point_file == NULL)
        {
            perror("Error opening file on master process\n");
            return -1;
        }

        // Read the number of points and dimensions from the first line of the file
        fscanf(point_file, "%d %d", &num_points, &num_dimensions);

        // Give error if the points or processes are not enough
        if (num_points < 2)
        {
            perror("Error: the number of points must be greater than 1\n");
            return -1;
        }
        if (num_points < comm_size)
        {
            perror("Error: the number of points must be greater than the number of processes\n");
            return -1;
        }
        if (comm_size < 2)
        {
            perror("Error: cannot run parallel application on 1 process\n");
            return -1;
        }
        fclose(point_file);
    }
    
    // Send num_points to all the processes, it is used to compute the num_points_...
    MPI_Bcast(&num_points, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    MPI_Bcast(&num_dimensions, 1, MPI_INT, MASTER_PROCESS, MPI_COMM_WORLD);
    

    // Points are divided equally on all processes exept master process which takes the remaing points
    int num_points_normal_processes, num_points_master_process, num_points_local_process;
    num_points_normal_processes = num_points / (comm_size-1);
    num_points_master_process = num_points % (comm_size-1);
    Point *local_points;

    Point *all_points = NULL;
    all_points = parallel_mergesort(all_points, path, rank_process, comm_size, VERBOSE);
    if (rank_process == MASTER_PROCESS)
    {
        if (all_points == NULL)
        {
            perror("Error in executing parallel_order_points\n");
            return -1;
        }
    }

    // if(comm_size <= 2)
    // {
    //     if(rank_process == MASTER_PROCESS){
    //         printf("Launching sequential algorithm...\n");
    //         printf("SUPER FINAL GLOBAL DMIN: %f\n", recSplit(all_points, num_points).min_distance);
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD);

    //     free(all_points);

    //     printf("Memory free\n");
    //     end = clock();
    //     cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    //     printf("Time elapsed: %f\n", cpu_time_used);

    //     MPI_Finalize();
    //     return 0;
    // }

    // POINT 1 AND 2 - divide points and send them with their out_of_region values to each process
    int left_x_out_of_region = INT_MAX, right_x_out_of_region = INT_MAX;
    if (rank_process == MASTER_PROCESS)
    {
        // Transfer total number of points and the correct slice of points to work on for every process
        Point *points_to_send;
        points_to_send = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        int left_to_send, right_to_send;
        for (int process = 1; process < comm_size; process++)
        {
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
            sendPointsPacked(points_to_send, num_points_normal_processes, process, 1, MPI_COMM_WORLD);
            
            // Transfer also the left and right out_of_region x coordinate for each process
            if (process == 1)
            {
                left_to_send = INT_MAX;
                right_to_send = all_points[num_points_normal_processes*(process)].coordinates[AXIS];
            }
            else{
                left_to_send = all_points[num_points_normal_processes*(process-1)-1].coordinates[AXIS]; 
                if (process != comm_size - 1 || num_points_master_process != 0) //rank_process != 0 && (rank_process != comm_size - 1 || num_points_master_process != 0)
                    right_to_send = all_points[num_points_normal_processes*(process)].coordinates[AXIS];
                else
                    right_to_send = INT_MAX;
            }
            MPI_Send(&left_to_send, 1, MPI_INT, process, 2, MPI_COMM_WORLD);
            MPI_Send(&right_to_send, 1, MPI_INT, process, 3, MPI_COMM_WORLD);
        }

        // Free points_to_send once the send for all the processes is done 
        for (int point = 0; point < num_points_normal_processes; point++)
        {
            free(points_to_send[point].coordinates);
        }
        free(points_to_send);

        // Store the points of the master process and its out_of_region values
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

    // POINT 3 - compute min distance for each process
    double local_dmin = INT_MAX;
    Pairs *pairs;
    pairs = (Pairs *)malloc(sizeof(Pairs));

    pairs->points1 = (Point *)malloc((num_points_local_process) * sizeof(Point));
    pairs->points2 = (Point *)malloc((num_points_local_process) * sizeof(Point));
    for (int i = 0; i < (num_points_local_process); i++)
    {
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

    // POINT 4 - allreduce to find the global dmin
    double global_dmin;
    MPI_Allreduce(&local_dmin, &global_dmin, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
        printf("---GLOBAL DMIN: %f\n", global_dmin);
    
    // POINT 5 - compute the boundary between processes
    double left_boundary = -INT_MAX, right_boundary = INT_MAX;

    if (rank_process != 1 && num_points_local_process != 0)
        left_boundary = (left_x_out_of_region + local_points[0].coordinates[AXIS])/2.0;

    if(rank_process != MASTER_PROCESS && (rank_process != comm_size - 1 || num_points_master_process != 0))
        right_boundary = (right_x_out_of_region + local_points[num_points_local_process-1].coordinates[AXIS])/2.0;
    
    // POINT 6 - get the points in the strips for each process, get for both left and right side, points where x_i - dmin <= boundary
    int num_points_left_strip = 0, num_points_right_strip = 0;
    Point *left_strip_points = (Point *)malloc(num_points_local_process * sizeof(Point));
    Point *right_strip_points = (Point *)malloc(num_points_local_process * sizeof(Point));

    for (int point = 0; point < num_points_local_process; point++)
    {
        if (local_points[point].coordinates[AXIS] < left_boundary + global_dmin)
        {
            left_strip_points[num_points_left_strip].num_dimensions = num_dimensions;
            left_strip_points[num_points_left_strip].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
            {
                left_strip_points[num_points_left_strip].coordinates[dimension] = local_points[point].coordinates[dimension];
            }
            num_points_left_strip++;
        }
        else
            break;
    }
    for(int point = num_points_local_process - 1; point >= 0; point--){
        if (local_points[point].coordinates[AXIS] > right_boundary - global_dmin)
        {
            right_strip_points[num_points_right_strip].num_dimensions = num_dimensions;
            right_strip_points[num_points_right_strip].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
            {
                right_strip_points[num_points_right_strip].coordinates[dimension] = local_points[point].coordinates[dimension];
            }
            num_points_right_strip++;
        }
        else
            break;
    }

    // POINT 7 AND 8 - get point of left strip move the point to the left process, merge the moved left points with the right points of the target process
    if (rank_process != 1  && rank_process != MASTER_PROCESS)
    {
        // Send the number of points to the left process
        MPI_Send(&num_points_left_strip, 1, MPI_INT, rank_process-1, 0, MPI_COMM_WORLD);
        
        // Send the points to the left process
        if (num_points_left_strip != 0)
            sendPointsPacked(left_strip_points, num_points_left_strip, rank_process-1, 1, MPI_COMM_WORLD);
    }
    if(rank_process == MASTER_PROCESS){
        // Send the number of points to the left process
        MPI_Send(&num_points_left_strip, 1, MPI_INT, comm_size-1, 0, MPI_COMM_WORLD);
       
        // Send the points to the left process
        if (num_points_left_strip != 0)
            sendPointsPacked(left_strip_points, num_points_left_strip, comm_size-1, 1, MPI_COMM_WORLD);
    }

    if(rank_process != MASTER_PROCESS){
        // Receive the number of points from the next process
        int num_points_received;
        if(rank_process != comm_size - 1)
            MPI_Recv(&num_points_received, 1, MPI_INT, rank_process+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        else
            MPI_Recv(&num_points_received, 1, MPI_INT, MASTER_PROCESS, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Receive the points
        Point *received_points = (Point *)malloc(num_points_received * sizeof(Point));
        if (num_points_received != 0){
            if(rank_process != comm_size - 1)
                recvPointsPacked(received_points, num_points_received, rank_process+1, 1, MPI_COMM_WORLD);
            else
                recvPointsPacked(received_points, num_points_received, MASTER_PROCESS, 1, MPI_COMM_WORLD);
        }
        
        
        // merge right_strip_points and received_points
        Point *local_strip_points = (Point *)malloc((num_points_right_strip + num_points_received) * sizeof(Point));
        for (int i = 0; i < num_points_right_strip; i++)
        {
            local_strip_points[i].num_dimensions = num_dimensions;
            local_strip_points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
            {
                local_strip_points[i].coordinates[dimension] = right_strip_points[i].coordinates[dimension];
            }
        }
        for (int i = 0; i < num_points_received; i++)
        {
            local_strip_points[i+num_points_right_strip].num_dimensions = num_dimensions;
            local_strip_points[i+num_points_right_strip].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
            {
                local_strip_points[i+num_points_right_strip].coordinates[dimension] = received_points[i].coordinates[dimension];
            }
        }

        // POINT 9 - reorder point in the strip over y
        int j = num_points_right_strip + num_points_received;
        mergeSort(local_strip_points, j, 1);
        
        //10. COMPUTE MIN DISTANCE FOR EACH STRIP (line 44 of sequential_recursive)
        double min = global_dmin;
        for (int i = 0; i < j - 1; i++){
            for (int k = i + 1; k < j && abs(local_strip_points[k].coordinates[1] - local_strip_points[i].coordinates[1]) <= global_dmin; k++){
                double dd = distance(local_strip_points[i], local_strip_points[k]);
                if (dd < min){
                    min = dd;
                }
                if(dd < pairs->min_distance){
                    pairs->min_distance = dd;
                    pairs->points1[0] = local_strip_points[i];
                    pairs->points2[0] = local_strip_points[k];
                    pairs->num_pairs = 1;
                }
                else if(dd == pairs->min_distance){
                    pairs->points1[pairs->num_pairs] = local_strip_points[i];
                    pairs->points2[pairs->num_pairs] = local_strip_points[k];
                    pairs->num_pairs++;
                }
            }
        }
        local_dmin = min;
        free(local_strip_points);
    }
    
    //11. REDUCE ALL DISTANCES BACK TO MASTER PROCESS
    MPI_Allreduce(&local_dmin, &global_dmin, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

    int loca_number_pairs_min_distance = 0, number_pairs_min_distance = 0;
    if(global_dmin == pairs->min_distance){
         for(int i = 0; i < pairs->num_pairs; i++){
            //  if( (i == 0) || ( (differPoint(pairs->points1[i],pairs->points2[i-1]) && differPoint(pairs->points2[i],pairs->points1[i-1])) && 
            //                    (differPoint(pairs->points1[i],pairs->points1[i-1]) || differPoint(pairs->points2[i],pairs->points2[i-1])) ) ){
                if (VERBOSE==0){
                    printPoint(pairs->points1[i]);
                    printPoint(pairs->points2[i]);
                    printf("\n");
                }
                loca_number_pairs_min_distance++;
            //  }
        }
    }
    MPI_Reduce(&loca_number_pairs_min_distance, &number_pairs_min_distance, 1, MPI_INT, MPI_SUM, MASTER_PROCESS, MPI_COMM_WORLD);
    if (rank_process==MASTER_PROCESS)
        printf("Found a total of %d pairs of points with dmin=%f\n", number_pairs_min_distance, global_dmin);
    
    //12. RETURN MIN DISTANCE
    if (rank_process == MASTER_PROCESS)
        printf("---NEW GLOBAL DMIN: %f\n", global_dmin);

    // Free all points
    if (rank_process == MASTER_PROCESS)
    {
        for (int point = 0; point < num_points; point++)
        {
            free(all_points[point].coordinates);
        }
    }
    free(all_points);

    // Free local points (verify how to implement it correctly)
    // free(local_points); 

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