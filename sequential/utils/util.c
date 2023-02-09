#include <math.h>
#include <stdlib.h>
#include "util.h"

double distance(Point p1, Point p2){
    double squared_sum = 0;
    int num_dimensions = p1.num_dimensions;
    for (int dimension = 0; dimension < num_dimensions; dimension++)
        squared_sum += pow((p1.coordinates[dimension] - p2.coordinates[dimension]), 2);
    
    return sqrt(squared_sum);
}

void merge(Point *points, int start_index, int middle_index, int end_index, int dim){
    int index_left_array, index_right_array, next_value_to_change;
    int num_points_left_array = middle_index - start_index + 1;
    int num_points_right_array = end_index - middle_index;

    Point *left_array = (Point *)malloc(num_points_left_array * sizeof(Point));
    Point *right_array = (Point *)malloc(num_points_right_array * sizeof(Point));
    for (index_left_array = 0; index_left_array < num_points_left_array; index_left_array++)
        left_array[index_left_array] = points[start_index + index_left_array];
    
    for (index_right_array = 0; index_right_array < num_points_right_array; index_right_array++)
        right_array[index_right_array] = points[middle_index + 1 + index_right_array];
    
    index_left_array = 0;
    index_right_array = 0;
    next_value_to_change = start_index;
    while (index_left_array < num_points_left_array && index_right_array < num_points_right_array){
        if (left_array[index_left_array].coordinates[dim] <= right_array[index_right_array].coordinates[dim]){
            points[next_value_to_change] = left_array[index_left_array];
            index_left_array++;
        }
        else{
            points[next_value_to_change] = right_array[index_right_array];
            index_right_array++;
        }
        next_value_to_change++;
    }
    while (index_left_array < num_points_left_array){
        points[next_value_to_change] = left_array[index_left_array];
        index_left_array++;
        next_value_to_change++;
    }
    while (index_right_array < num_points_right_array){
        points[next_value_to_change] = right_array[index_right_array];
        index_right_array++;
        next_value_to_change++;
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

double isMIN(double a, double b){
    return a < b ? a : b;
}

double isMINof3(double a, double b, double c){
        double min = a < b ? a : b;
        return min < c ? min : c;
}

double recSplit(Point* points, int dim){
    if (dim == 2){
        return distance(points[0], points[1]);
    }
    else if (dim == 3){
        return isMINof3(distance(points[0], points[1]), distance(points[0], points[2]), distance(points[1], points[2]));
    }
    else{
        int mid = dim / 2;
        double d1 = recSplit(points, mid);
        double d2 = recSplit(points + mid, dim - mid);
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
            }
        }
        free(strip);
        return min;
    }
}