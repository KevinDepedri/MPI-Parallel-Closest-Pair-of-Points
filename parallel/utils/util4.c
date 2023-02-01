#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "util4.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2147483647

extern Point *global_points1, *global_points2;
extern int global_num_pairs;
extern double global_pair_distance;


double distance(Point p1, Point p2){
    double squared_sum = 0;
    int dimensions = p1.num_dimensions;
    for (int i = 0; i < dimensions; i++)
        squared_sum += pow((p1.coordinates[i] - p2.coordinates[i]), 2);
    
    return sqrt(squared_sum);
}

void merge(Point *points, int start_index, int middle_index, int end_index, int dim){
    int i, j, k;
    int num_points_left_array = middle_index - start_index + 1;
    int num_points_right_array = end_index - middle_index;
    // allocate temp arrays
    Point *left_array = (Point *)malloc(num_points_left_array * sizeof(Point));
    Point *right_array = (Point *)malloc(num_points_right_array * sizeof(Point));
    for (i = 0; i < num_points_left_array; i++)
        left_array[i] = points[start_index + i];
    
    for (j = 0; j < num_points_right_array; j++)
        right_array[j] = points[middle_index + 1 + j];
    
    i = 0;
    j = 0;
    k = start_index;
    while (i < num_points_left_array && j < num_points_right_array){
        if (left_array[i].coordinates[dim] <= right_array[j].coordinates[dim]){
            points[k] = left_array[i];
            i++;
        }
        else{
            points[k] = right_array[j];
            j++;
        }
        k++;
    }
    while (i < num_points_left_array){
        points[k] = left_array[i];
        i++;
        k++;
    }
    while (j < num_points_right_array){
        points[k] = right_array[j];
        j++;
        k++;
    }

    free(left_array);
    free(right_array);
}

void mergeSortRec(Point *points, int start_index, int end_index, int axis){
    if (start_index < end_index){
        int middle_index = start_index + (end_index - start_index) / 2;
        mergeSortRec(points, start_index, middle_index, axis);
        mergeSortRec(points, middle_index + 1, end_index, axis);
        merge(points, start_index, middle_index, end_index, axis);
    }
}

void mergeSort(Point *points, int num_points, int axis){
    mergeSortRec(points, 0, num_points - 1, axis);
}

void printPoint(Point p)
{
    printf("(");
    for (int i = 0; i < p.num_dimensions; i++)
    {
        if (i != p.num_dimensions-1)
            printf("%d, ", p.coordinates[i]);
        else
            printf("%d", p.coordinates[i]);

    }
    printf(")");
}

void print_points(Point *point_list, int num_points, int rank_process){
    printf("PROCESS: %d\n", rank_process);
    for (int point = 0; point < num_points; point++){
        printf("-----Point %d: (", point);
        
        for (int dimension=0; dimension < point_list[point].num_dimensions; dimension++){
            if (dimension != point_list[point].num_dimensions - 1)
                printf("%d, ", point_list[point].coordinates[dimension]);
            
            else
                printf("%d)\n", point_list[point].coordinates[dimension]);
            
        }
    }
}

void sendPointsPacked(Point* points, int numPoints, int destination, int tag, MPI_Comm comm) {
  int bufferSize = numPoints * (sizeof(int) + sizeof(int)*points[0].num_dimensions);
  void *buffer = malloc(bufferSize);
  int position = 0;

  for (int point = 0; point < numPoints; point++) {
    MPI_Pack(&points[point].num_dimensions, 1, MPI_INT, buffer, bufferSize, &position, comm);
    MPI_Pack(points[point].coordinates, points[point].num_dimensions, MPI_INT, buffer, bufferSize, &position, comm);
  }

  MPI_Send(buffer, position, MPI_PACKED, destination, tag, comm);
  free(buffer);
}

void recvPointsPacked(Point* points, int numPoints, int source, int tag, MPI_Comm comm) {
  MPI_Status status;
  MPI_Probe(source, tag, comm, &status);

  int count;
  MPI_Get_count(&status, MPI_PACKED, &count);
  void* buffer = malloc(count);

  MPI_Recv(buffer, count, MPI_PACKED, source, tag, comm, MPI_STATUS_IGNORE);

  int position = 0;
  for (int i = 0; i < numPoints; i++) {
    MPI_Unpack(buffer, count, &position, &points[i].num_dimensions, 1, MPI_INT, comm);
    points[i].coordinates = (int*)malloc(points[i].num_dimensions * sizeof(int));
    MPI_Unpack(buffer, count, &position, points[i].coordinates, points[i].num_dimensions, MPI_INT, comm);
  }
  free(buffer);
}

double isMIN(double a, double b){
    return a < b ? a : b;
}

double isMINof3(double a, double b, double c){
        double min = a < b ? a : b;
        return min < c ? min : c;
}

double sequential_closestpair_recursive(Point* points, int dim){
    if (dim == 2){
        double dist = distance(points[0], points[1]);
        if(dist < global_pair_distance){
            global_pair_distance = dist;
            global_points1[0] = points[0];
            global_points2[0] = points[1];
            global_num_pairs = 1;

        }
        else if(dist == global_pair_distance){
            global_points1[global_num_pairs] = points[0];
            global_points2[global_num_pairs] = points[1];
            global_num_pairs++;
        }
        return dist;
    }
    else if (dim == 3){
        double dist1 = distance(points[0], points[1]);
        double dist2 = distance(points[0], points[2]);
        double dist3 = distance(points[1], points[2]);
        double dist = isMINof3(dist1, dist2, dist3);
        if(dist1 < global_pair_distance){
            global_pair_distance = dist;
            global_points1[0] = points[0];
            global_points2[0] = points[1];
            global_num_pairs = 1;

        }
        else if(dist1 == global_pair_distance){
            global_points1[global_num_pairs] = points[0];
            global_points2[global_num_pairs] = points[1];
            global_num_pairs++;
        }
        if(dist2 < global_pair_distance){
            global_pair_distance = dist;
            global_points1[0] = points[0];
            global_points2[0] = points[2];
            global_num_pairs = 1;

        }
        else if(dist2 == global_pair_distance){
            global_points1[global_num_pairs] = points[0];
            global_points2[global_num_pairs] = points[2];
            global_num_pairs++;
        }
        if(dist3 < global_pair_distance){
            global_pair_distance = dist;
            global_points1[0] = points[1];
            global_points2[0] = points[2];
            global_num_pairs = 1;

        }
        else if(dist3 == global_pair_distance){
            global_points1[global_num_pairs] = points[1];
            global_points2[global_num_pairs] = points[2];
            global_num_pairs++;
        }
        return dist;
    }
    else{
        int mid = dim / 2;
        double d1 = sequential_closestpair_recursive(points, mid);
        double d2 = sequential_closestpair_recursive(points + mid, dim - mid);
        double d = isMIN(d1, d2);
        Point *strip = (Point *)malloc(dim * sizeof(Point));
        int j = 0;
        for (int i = 0; i < dim; i++){
            if (abs(points[i].coordinates[0] - points[mid].coordinates[0]) < d){
                strip[j] = points[i];
                j++;
            }
        }
        mergeSort(strip, j, 1);
        
        double min = d;
        for (int i = 0; i < j - 1; i++){
            for (int k = i + 1; k < j && (strip[k].coordinates[1] - strip[i].coordinates[1]) < d; k++){
                double dd = distance(strip[i], strip[k]);
                if (dd < min){
                    min = dd;
                }
                if(dd < global_pair_distance){
                    global_pair_distance = dd;
                    global_points1[0] = strip[i];
                    global_points2[0] = strip[k];
                    global_num_pairs = 1;

                }
                else if(dd == global_pair_distance){
                    global_points1[global_num_pairs] = strip[i];
                    global_points2[global_num_pairs] = strip[k];
                    global_num_pairs++;
                }
                
            }
        }
        free(strip);
        return min;
    }
}

double parallel_closestpair(Point *all_points, int num_points, int num_dimensions, int rank_process, int comm_size, 
                            int num_points_normal_processes, int num_points_master_process){

    Point *local_points;
    int num_points_local_process;

    // POINT 1 AND 2 - divide points and send them with thei out_of_region valie
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
            free(points_to_send[point].coordinates);

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

    // POINT 3 - compute min distance for eahc process
    double local_dmin = INT_MAX;
    if (num_points_local_process > 1)
        local_dmin = sequential_closestpair_recursive(local_points, num_points_local_process);
    printf("PROCESS:%d DMIN:%f\n", rank_process, local_dmin);

    // POINT 4 - allreduce to find the global dmin
    double global_dmin;
    MPI_Allreduce(&local_dmin, &global_dmin, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    if (rank_process == MASTER_PROCESS)
        printf("GLOBAL DMIN: %f\n", global_dmin);
    
    // POINT 5 - compute the boundary between processes
    double left_boundary = -INT_MAX, right_boundary = INT_MAX;

    if (rank_process != 1 && num_points_local_process != 0)
        left_boundary = (left_x_out_of_region + local_points[0].coordinates[AXIS])/2.0;

    if(rank_process != MASTER_PROCESS && (rank_process != comm_size - 1 || num_points_master_process != 0))
        right_boundary = (right_x_out_of_region + local_points[num_points_local_process-1].coordinates[AXIS])/2.0;
    
    // POINT 6 - get the points in the strips for each process
    // get for both left and write side, points where x_i - dmin <= boundary
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
                left_strip_points[num_points_left_strip].coordinates[dimension] = local_points[point].coordinates[dimension];

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
                right_strip_points[num_points_right_strip].coordinates[dimension] = local_points[point].coordinates[dimension];

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
                local_strip_points[i].coordinates[dimension] = right_strip_points[i].coordinates[dimension];
        }

        for (int i = 0; i < num_points_received; i++)
        {
            local_strip_points[i+num_points_right_strip].num_dimensions = num_dimensions;
            local_strip_points[i+num_points_right_strip].coordinates = (int *)malloc(num_dimensions * sizeof(int));
            for (int dimension = 0; dimension < num_dimensions; dimension++)
                local_strip_points[i+num_points_right_strip].coordinates[dimension] = received_points[i].coordinates[dimension];
        }

        // POINT 9 - reorder point in the strip over y
        int j = num_points_right_strip + num_points_received- 1;
        mergeSort(local_strip_points, j, 1);
        
        //10. COMPUTE MIN DISTANCE FOR EACH STRIP (line 44 of sequential_recursive)
        double min = global_dmin;
        for (int i = 0; i < j - 1; i++){
            for (int k = i + 1; k < j && (local_strip_points[k].coordinates[1] - local_strip_points[i].coordinates[1]) < global_dmin; k++){
                double dd = distance(local_strip_points[i], local_strip_points[k]);
                if (dd < min)
                    min = dd;
            }
        }
        global_dmin = min;
        free(local_strip_points);

    }
    
    //11. REDUCE ALL DISTANCES BACK TO MASTER PROCESS
    double super_final_dmin;
    MPI_Allreduce(&global_dmin, &super_final_dmin, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

    return super_final_dmin;
}