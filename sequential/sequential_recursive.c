#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils/util.h"
#include <time.h>


int main(int argc, char *argv[]){
    clock_t start, end;
    double cpu_time_used;
    start = clock();

    Point *points;
    int num_points, num_dimensions;
    if(argc != 2){
        perror("Error: no file name or path has been provided as first argument (points input file)\n");
        return -1;
    }

    // Open input point file on master process
    FILE *point_file = fopen(argv[1], "r");  
    if (point_file == NULL){
        perror("Error opening file\n");
        return 1;
    }

    // Read the number of points and dimensions from the first line of the file
    fscanf(point_file, "%d %d", &num_points, &num_dimensions);
    if(num_points < 2){
        printf("Error: the number of points must be greater than 1");
        return 1;
    }
    
    // Allocate memory for the points
    points = (Point *)malloc(num_points * sizeof(Point));
    for (int i = 0; i < num_points; i++){
        points[i].num_dimensions = num_dimensions;
        points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));

        for (int j = 0; j < num_dimensions; j++){
            fscanf(point_file, "%d", &points[i].coordinates[j]);
        }
    }
    fclose(point_file);

    // sort the points by the first dimension
    mergeSort(points, num_points, 0);
    
    // find the closest pair of points
    double d = recSplit(points, num_points);

    // free the memory
    for (int i = 0; i < num_points; i++){
        free(points[i].coordinates);
    }
    free(points);

    //wait for user input
    printf("The closest pair of points is separated by a distance of %f", d);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("\nTime elapsed: %f\n", cpu_time_used);
    getchar();

    return 0;
}
