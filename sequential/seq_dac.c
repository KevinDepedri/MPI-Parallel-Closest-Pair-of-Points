// closest pair of points in k dimensions using divide and conquer
#include <stdio.h>
#include <stdlib.h>
#include "utils/util.h"

#define BOUND 100;

double isMIN(double a, double b){
    return a < b ? a : b;
}

double recSplit(Point* points, int dim){
    // recurrent function to split the space in half
    // when there are two points it returns the distance between them
    // then, when going back, it takes the minimum distance between the two recursion
    // and the distance between the two points that are in the middle of the space
    // and returns the minimum of the three

    if (dim == 2){
        return distance(points[0], points[1]);
    }
    else if (dim == 3){
        return isMIN(distance(points[0], points[1]), distance(points[0], points[2]));
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
                if (distance(strip[i], strip[k]) < min){
                    min = distance(strip[i], strip[k]);
                }
            }
        }
        free(strip);
        return min;
    }
}

int main(){
    Point *points;
    int num_points, num_dimensions;

    FILE* fp = fopen("../point_generator/points.txt", "r");
    if (fp == NULL){
        perror("Error opening file\n");
        return 1;
    }

    // Read the number of points and dimensions from the first line of the file
    fscanf(fp, "%d %d", &num_points, &num_dimensions);

    // Allocate memory for the points
    points = (Point *)malloc(num_points * sizeof(Point));
    for (int i = 0; i < num_points; i++){   
        points[i].num_dimensions = num_dimensions;
        points[i].coordinates = (int *)malloc(num_dimensions * sizeof(int));
        for (int j = 0; j < num_dimensions; j++){
            fscanf(fp, "%d", &points[i].coordinates[j]);
        }
    }
    fclose(fp);

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
    getchar();

    return 0;
}
