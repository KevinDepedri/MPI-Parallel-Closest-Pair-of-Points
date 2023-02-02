#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "util3.h"

#define AXIS 0
#define MASTER_PROCESS 0
#define INT_MAX 2147483647


double distance(Point p1, Point p2){
    double squared_sum = 0;
    int dimensions = p1.num_dimensions;
    for (int i = 0; i < dimensions; i++){
        squared_sum += pow((p1.coordinates[i] - p2.coordinates[i]), 2);
    }
    return sqrt(squared_sum);
}

int differPoint(Point p1, Point p2){
    for (int i = 0; i < p1.num_dimensions; i++)
    {
        if (p1.coordinates[i] != p2.coordinates[i])
            return 1;
    }
    return 0;
}

void merge(Point *points, int start_index, int middle_index, int end_index, int dim){
    int i, j, k;
    int num_points_left_array = middle_index - start_index + 1;
    int num_points_right_array = end_index - middle_index;
    // allocate temp arrays
    Point *left_array = (Point *)malloc(num_points_left_array * sizeof(Point));
    Point *right_array = (Point *)malloc(num_points_right_array * sizeof(Point));
    for (i = 0; i < num_points_left_array; i++){
        left_array[i] = points[start_index + i];
    }
    for (j = 0; j < num_points_right_array; j++){
        right_array[j] = points[middle_index + 1 + j];
    }
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

void print_points(Point *point_list, int num_points, int rank_process){
    printf("PROCESS: %d\n", rank_process);
    for (int point = 0; point < num_points; point++){
        printf("-----Point %d: (", point);
        
        for (int dimension=0; dimension < point_list[point].num_dimensions; dimension++){
            if (dimension != point_list[point].num_dimensions - 1){
                printf("%d, ", point_list[point].coordinates[dimension]);
            }
            else
            {
                printf("%d)\n", point_list[point].coordinates[dimension]);
            }
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


void update_pair_pointer(Point point1, Point point2, Pairs* p){
    double current_distance = distance(point1, point2);
    if(current_distance < p->min_distance){
        p->min_distance = current_distance;
        p->points1[0] = point1;
        p->points2[0] = point2;
        p->num_pairs = 1;
    }
    else if (current_distance == p->min_distance)
    {   
        p->points1[p->num_pairs] = point1;
        p->points2[p->num_pairs] = point2;
        p->num_pairs++;
    }
    return;
}

void recSplit(Point* points, int dim, Pairs* p){
    if (dim == 2){
        update_pair_pointer(points[0], points[1], p);
        return;
    }
    else if (dim == 3){
        update_pair_pointer(points[0], points[1], p);
        update_pair_pointer(points[0], points[2], p);
        update_pair_pointer(points[2], points[1], p);
        return;
    }
    else{
        int mid = dim / 2;
        recSplit(points, mid, p);
        recSplit(points + mid, dim - mid, p);
        double d = p->min_distance;
        
        Point *strip = (Point *)malloc(dim * sizeof(Point));
        int j = 0;
        for (int i = 0; i < dim; i++){
            if (abs(points[i].coordinates[0] - points[mid].coordinates[0]) < d){ //Here the strip is assumed to be on the MID POINT, 
                strip[j] = points[i];                                            //In the parallel scenario we put in between two points.
                j++;                                                             //IS IT THE SAME??
            }
        }
        
        mergeSort(strip, j, 1);
        for (int i = 0; i < j - 1; i++){
            for (int k = i + 1; k < j && (strip[k].coordinates[1] - strip[i].coordinates[1]) < d; k++){ //Do we NEED ABSOLUTE VALUE HERE??
                double current_distance = distance(strip[i], strip[k]);
                if (current_distance < p->min_distance){
                    p->min_distance = current_distance;
                    p->points1[0] = strip[i];
                    p->points2[0] = strip[k];
                    p->num_pairs = 1;
                }
                else if (current_distance == p->min_distance){
                    p->points1[p->num_pairs] = strip[i];
                    p->points2[p->num_pairs] = strip[k];
                    p->num_pairs++;
                }
            }
        }
        free(strip);
    }
}

Point *parallel_mergesort(Point *all_points, char path[], int rank_process, int comm_size, int verbose){

    int num_points, num_dimensions;
    int num_points_normal_processes, num_points_master_process, num_points_local_process;
    int index_first_point, index_last_point = 0;

    Point *local_points;

    if (rank_process == MASTER_PROCESS)
    {
        // Open input point file on master process
        FILE *point_file = fopen(path, "r"); 
        if (point_file == NULL)
        {
            perror("Error opening file on master process\n");
            return NULL;    
        }

        // Read the number of points and dimensions from the first line of the file
        fscanf(point_file, "%d %d", &num_points, &num_dimensions);

        // Points are divided equally on all processes exept master process which takes the remaing points
        num_points_normal_processes = num_points / (comm_size-1);
        num_points_master_process = num_points % (comm_size-1);

        // Read the points and store them in the master process
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

        if (verbose == 1)
        {
            printf("INPUT LIST OF POINTS:\n");
            print_points(all_points, num_points, rank_process);    
        }        
        
        // Transfer total number of points and the correct slice of points to process for every process
        Point *points_to_send;
        points_to_send = (Point *)malloc(num_points_normal_processes * sizeof(Point));
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
        }

        // Free points to send once the send for all the processes is done 
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
        // Receive total number of points and the points that will be processed by this process
        MPI_Recv(&num_points_normal_processes, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        num_points_local_process = num_points_normal_processes;
        local_points = (Point *)malloc(num_points_local_process * sizeof(Point));
        recvPointsPacked(local_points, num_points_local_process, 0, 1, MPI_COMM_WORLD);
    }

    if (verbose == 1){
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
        {
            processes_sorted_points[process] = (Point *)malloc(num_points_normal_processes * sizeof(Point));
        }

        // Save first the points ordered from the current process (master), then receive the ones from the other processes
        processes_sorted_points[0] = local_points;
        for (int process = 1; process < comm_size; process++)
        {
            recvPointsPacked(processes_sorted_points[process], num_points_normal_processes, process, 0, MPI_COMM_WORLD);
        }

        // Merge the ordere points from all the processes
        int *temporary_indexes;
        temporary_indexes = (int *)malloc(comm_size * sizeof(int));
        for (int process = 0; process < comm_size; process++)
        {
            temporary_indexes[process] = 0;
        }

        // For each position of the final ordered array of points define an initial minimum value and the process that encompasses this value
        for (int point = 0; point < num_points; point++)
        {        
            int min = INT_MAX;
            int process_with_minimum_value = 0;

            // For that previous position look in all the processes to find the minimum value
            for (int process = 0; process < comm_size; process++)
            {
                // For each specific process look to its first point (since it is the lowest), if there are still points not used in that process 
                if ((process == 0 && temporary_indexes[process] < num_points_master_process) || (process != 0 && temporary_indexes[process] < num_points_normal_processes))
                {
                    // If a new minimum is reached then save it as minimum and save the process that led to it
                    if (processes_sorted_points[process][temporary_indexes[process]].coordinates[AXIS] < min)
                    {
                        min = processes_sorted_points[process][temporary_indexes[process]].coordinates[AXIS];
                        process_with_minimum_value = process;
                    }

                }

            }
            // One a value is confirmed as the lowest between all the compared ones, then save it in the fianl array of ordered points
            all_points[point] = processes_sorted_points[process_with_minimum_value][temporary_indexes[process_with_minimum_value]];
            temporary_indexes[process_with_minimum_value]++;
        }

        if (verbose == 1){
            printf("ORDERED POINTS:\n");
            print_points(all_points, num_points, rank_process); 
        }

        // Free processes_sorted_points
        free(processes_sorted_points); //TODO: COORDINATES??
    }

    free(local_points); //TODO: COORDINATES??

    return all_points;
}