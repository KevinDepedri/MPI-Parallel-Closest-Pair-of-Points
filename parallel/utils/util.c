#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

double distance(Point p1, Point p2){
    double squared_sum = 0;
    int dimensions = p1.num_dimensions;
    for (int i = 0; i < dimensions; i++){
        squared_sum += pow((p1.coordinates[i] - p2.coordinates[i]), 2);
    }
    return sqrt(squared_sum);
}

void merge(Point *points, int start_index, int middle_index, int end_index, int dim){ //void merge(Point *points, int l, int m, int r, int dim
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

void print_points(Point *point_list, int num_points){
        for (int point = 0; point < num_points; point++){
            printf("Point %d: (", point);
            
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