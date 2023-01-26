// closest pair of points in k dimensions using divide and conquer
#import <stdio.h>
#import <stdlib.h>
#import "utils/util.h"

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
    else{
        int mid = dim/2;
        double d1 = recSplit(points, mid);
        double d2 = recSplit(points + mid, dim - mid);
        double d = isMIN(d1, d2);
        Point* middle = (Point*)malloc(sizeof(Point));
        middle->num_dimensions = points[0].num_dimensions;
        middle->coordinates = (int*)malloc(middle->num_dimensions * sizeof(int));
        for (int i = 0; i < middle->num_dimensions; i++){
            middle->coordinates[i] = (points[mid].coordinates[i] + points[mid - 1].coordinates[i])/2;
        }
        double d3 = d;
        for (int i = 0; i < dim; i++){
            if (distance(points[i], *middle) < d3){
                d3 = distance(points[i], *middle);
            }
        }
        return isMIN(d, d3);
    }
}

int main(){
    Point *points;
    int num_points, num_dimensions;

    FILE* fp = fopen("../point_generator/points.txt", "r");
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

    // sort the points by the first dimension
    mergeSort(points, num_points, 0);

    // find the closest pair of points
    double d = recSplit(points, num_points);

    printf("The closest pair of points is separated by a distance of %f", d);

    // free the memory
    for (int i = 0; i < num_points; i++)
    {
        free(points[i].coordinates);
    }
    free(points);

    //wait for user input
    getchar();

    return 0;
}
