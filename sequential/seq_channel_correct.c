#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils/util.h"

# define BOUND 100;

double get_min(double value1, double value2){
        return value1 < value2 ? value1 : value2;
}

double get_min_of_3(double value1, double value2, double value3){
        double min = value1 < value2 ? value1 : value2;
        return min < value3 ? min : value3;
}

double closestPair(Point *X, Point *Y, int num_points_array, int start_index){
    if (num_points_array == 2){ 
        // printf("2 POINTS\n"); 
        return distance(X[start_index], X[start_index+1]);
        }
    if (num_points_array == 3){ 
        // printf("3 POINTS\n"); 
        return get_min_of_3(distance(X[start_index], X[start_index+1]), distance(X[start_index], X[start_index+2]), distance(X[start_index+1], X[start_index+2]));
        }

    int half_num_points = num_points_array/2;
    int remainder = (num_points_array % 2 != 0);
    int num_points_new_array_1 = half_num_points + remainder;
    int num_points_new_array_2 = half_num_points;

    // printf("%d %d+%d %d %d\n",num_points_array, half_num_points, remainder, num_points_new_array_1, num_points_new_array_2);
   
    double dmin_left  = closestPair(X, Y, num_points_new_array_1, 0);
    double dmin_right = closestPair(X, Y, num_points_new_array_2, num_points_new_array_1);
    double dmin = get_min(dmin_left, dmin_right);

    for (int i = start_index; i < num_points_array; i++){
        for (int j = 0: )
    }


    return dmin;
}

int main(int argc, char *argv[]){   
    Point *points;
    int num_points, num_dimensions;

    FILE* fp = fopen("../point_generator/points_channel_test.txt", "r");
    if (fp == NULL)
    {
        perror("Error opening file\n");
        return 1;
    }

    // Read the number of points and dimensions from the first line of the file
    fscanf(fp, "%d %d", &num_points, &num_dimensions);

    // Allocate memory for the points
    points = (Point *)malloc(num_points * sizeof(Point));
    for (int i = 0; i < num_points; i++)
    {   
        points[i].num_dimensions = num_dimensions;
        points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        for (int j = 0; j < num_dimensions; j++)
        {
            fscanf(fp, "%d", &points[i].coordinates[j]);
        }
    }
    fclose(fp);


    // Print the points
    printf("POINTS FROM FILE\n");
    for (int i = 0; i < num_points; i++)
    {
        printf("Point %d: (", i);
        for (int j = 0; j < num_dimensions; j++)
        {   
            if (j != num_dimensions-1){
                printf("%d, ", points[i].coordinates[j]);
            }else{
                printf("%d", points[i].coordinates[j]);
            }
        }
        printf(")\n");
    }

    // Order the points along X and y using MergeSort (PRINT JUST TO CHECK)
    Point *pointsX = points;
    mergeSort(pointsX, num_points, 0);
    printf("POINTS FROM X ORDERED LIST\n");
    for (int i = 0; i < num_points; i++)
    {
        printf("Point %d: (", i);
        for (int j = 0; j < num_dimensions; j++)
        {   
            if (j != num_dimensions-1){
                printf("%d, ", pointsX[i].coordinates[j]);
            }else{
                printf("%d", pointsX[i].coordinates[j]);
            }
        }
        printf(")\n");
    }
    Point *pointsY = points;
    mergeSort(pointsY, num_points, 1);
    printf("POINTS FROM Y ORDERED LIST\n");
    for (int i = 0; i < num_points; i++)
    {
        printf("Point %d: (", i);
        for (int j = 0; j < num_dimensions; j++)
        {   
            if (j != num_dimensions-1){
                printf("%d, ", pointsY[i].coordinates[j]);
            }else{
                printf("%d", pointsY[i].coordinates[j]);
            }
        }
        printf(")\n");
    }

    // COMPUTE DISTANCE JUST TO CHECK
    printf("DISTANCES JUST TO CHECK\n");
    int comparisons = 0;
    for (int i = 0; i < num_points; i++){
        for (int j = i; j < num_points; j++){
            if (i != j){
                comparisons += 1;
                printf("Comparison %d - Left distance between point %d and %d: %f\n", comparisons, i, j, distance(points[i], points[j]));
            }
        }
    }

    // Apply recursive algo
    double dmin = closestPair(pointsX, pointsY, num_points, 0);
    printf("\nLowest distance between %d points: %f\n", num_points, dmin);

    // Free memory for initial points list
    for (int i =0; i< num_points ;i++){
        free(points[i].coordinates);
    }
    free(points);
    printf("\nMemory free\n");

    return 0;
}